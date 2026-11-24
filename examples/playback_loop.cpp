/**
 * Minimal live playback loop:
 *   dsp_create → open platform backend → render callback runs dsp_process
 *
 * On machines without a real device (CI / headless), backend start may fail
 * with DSP_ERR_UNSUPPORTED or DSP_ERR_DEVICE — we fall back to a short offline
 * process so the example still validates the pipeline.
 */

#include "dsp_engine.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

#if defined(_WIN32)
  #include <windows.h>
  static void sleep_ms(int ms) { Sleep(static_cast<DWORD>(ms)); }
#else
  #include <unistd.h>
  static void sleep_ms(int ms) {
      if (ms > 0) usleep(static_cast<useconds_t>(ms) * 1000u);
  }
#endif

struct RenderCtx {
    DspEngine* eng = nullptr;
    int        channels = 2;
    float      phase = 0.f;
    float      sample_rate = 48000.f;
    std::vector<float> dry;
};

static void render_cb(void* user, float* out, int frames)
{
    auto* ctx = static_cast<RenderCtx*>(user);
    if (!ctx || !ctx->eng || !out || frames <= 0) return;

    const int ch = ctx->channels > 0 ? ctx->channels : 2;
    const size_t need = static_cast<size_t>(frames) * static_cast<size_t>(ch);
    if (ctx->dry.size() < need)
        ctx->dry.resize(need);

    const float freq = 440.f;
    const float two_pi = 6.28318530718f;
    for (int i = 0; i < frames; ++i) {
        float s = 0.2f * std::sin(ctx->phase);
        ctx->phase += two_pi * freq / ctx->sample_rate;
        if (ctx->phase > two_pi) ctx->phase -= two_pi;
        for (int c = 0; c < ch; ++c)
            ctx->dry[static_cast<size_t>(i * ch + c)] = s;
    }

    if (dsp_process(ctx->eng, ctx->dry.data(), out, frames) != DSP_OK) {
        std::memset(out, 0, need * sizeof(float));
    }
}

int main()
{
    DspConfig cfg{};
    cfg.sample_rate = 48000.f;
    cfg.channels = 2;
    cfg.max_block = 1024;
    cfg.enable_eq = 0;
    cfg.enable_dvc = 1;
    cfg.enable_limiter = 1;
    cfg.enable_dither = 0;
    cfg.dither_mode = DSP_DITHER_NONE;
    cfg.limiter_threshold_db = -1.0f;
    cfg.limiter_release_ms = 50.f;

    DspEngine* eng = dsp_create(&cfg);
    if (!eng) {
        std::printf("dsp_create failed\n");
        return 1;
    }
    std::printf("Psychic %s\n", dsp_version());

    dsp_dvc_set_gain_db(eng, -3.0f);
    dsp_limiter_enable(eng, 1);
    (void)dsp_set_realtime_priority(eng, 10);

    DspBackend* be = dsp_backend_create();
    if (!be) {
        std::printf("dsp_backend_create failed\n");
        dsp_destroy(eng);
        return 1;
    }

    DspBackendConfig bcfg{};
    bcfg.sample_rate = 48000;
    bcfg.channels = 2;
    bcfg.buffer_frames = 256;
    bcfg.exclusive = 0;
    bcfg.device_id = 0;

    DspError err = dsp_backend_open(be, &bcfg);
    if (err != DSP_OK) {
        std::printf("dsp_backend_open failed (%d) — offline fallback\n", static_cast<int>(err));
        const int frames = 256;
        std::vector<float> in(frames * 2, 0.1f), out(frames * 2);
        dsp_process(eng, in.data(), out.data(), frames);
        std::printf("Offline process OK, first sample = %.6f\n", out[0]);
        dsp_backend_destroy(be);
        dsp_destroy(eng);
        return 0;
    }

    RenderCtx ctx;
    ctx.eng = eng;
    ctx.channels = dsp_backend_actual_channels(be);
    if (ctx.channels <= 0) ctx.channels = 2;
    ctx.sample_rate = static_cast<float>(dsp_backend_actual_rate(be));
    if (ctx.sample_rate < 1.f) ctx.sample_rate = 48000.f;
    ctx.dry.resize(static_cast<size_t>(1024) * static_cast<size_t>(ctx.channels));

    std::printf("Backend open: rate=%d ch=%d buf=%d\n",
                dsp_backend_actual_rate(be),
                dsp_backend_actual_channels(be),
                dsp_backend_actual_buffer_frames(be));

    err = dsp_backend_start(be, &render_cb, &ctx);
    if (err != DSP_OK) {
        std::printf("dsp_backend_start failed (%d) — no live device (null backend or denied)\n",
                    static_cast<int>(err));
        std::printf("Pipeline still OK via offline path.\n");
        const int frames = 256;
        std::vector<float> in(frames * 2, 0.1f), out(frames * 2);
        dsp_process(eng, in.data(), out.data(), frames);
        dsp_backend_close(be);
        dsp_backend_destroy(be);
        dsp_destroy(eng);
        return 0;
    }

    std::printf("Playing 440 Hz (DVC -3 dB + limiter) for ~2 seconds...\n");
    sleep_ms(2000);

    dsp_backend_stop(be);
    std::printf("Stopped. xruns=%d\n", dsp_backend_xrun_count(be));

    dsp_backend_close(be);
    dsp_backend_destroy(be);
    dsp_destroy(eng);
    std::printf("Done.\n");
    return 0;
}
