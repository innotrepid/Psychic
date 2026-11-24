# Psychic

**Psychic** is an adaptive **64-bit** audio DSP engine for Flutter music players and native apps — the processing brain behind hosts like Resonate.

**Version 0.6.1** — Intelligent headroom, auto-bass, auto-DVC, and full adaptive stack.

> Product name: **Psychic** · C ABI prefix: `dsp_*` (stable for existing integrations)

## Features

- **Direct Volume Control (DVC)** — clean gain with optional smoothing
- **Multi-band parametric EQ** (peak, shelves, LP/HP; up to 31 bands)
- **True-peak / lookahead / two-band limiter** with ceiling, knee, crossover
- **Preamp** (independent of DVC)
- **Speaker mode + virtual bass** (phone delivery)
- **PCM16 process path** for Media3 / JNI hosts (`dsp_process_pcm16`)
- **Lock-free control mailbox** (UI-thread safe setters)
- **Offline analysis** — `dsp_analyze_buffer` / `dsp_probe_max_bass_gain_db` (per-track safe bass)
- **Online auto-bass** — level meters + low-shelf servo under ceiling−margin
- **Auto-DVC** — short-term RMS target with bias / max boost-cut / headroom clamp
- **Shared headroom budget** — bass / preamp / gain share one pool
- **Content-aware virtual bass** + clip-aware auto-preamp
- **Crest-based limiter personality** + silence hygiene
- **Level meters** — input / true-peak / post-EQ / bass-band / headroom / short-term RMS
- **Host guide** — `docs/RESONATE_INTEGRATION.md` (JNI, cache, UI)
- **Dither** (TPDF / noise-shaped)
- **Sample-rate conversion** (linear, cubic, HQ windowed-sinc)
- **Lock-free SPSC ring buffer**
- **Double-precision internal path** (optional)
- **Real-time thread priority** helpers
- **Platform audio backends**: AAudio, WASAPI, Core Audio, ALSA
- **C ABI** + Flutter `dart:ffi` bindings
- **Unit tests + benchmarks** + GitHub Actions CI matrix

## Pipeline

`EQ → auto-bass shelf → speaker/VB → preamp → DVC (auto-gain) → limiter → dither`

## Name & API

| Layer | Name |
|-------|------|
| Product / docs | **Psychic** |
| CMake project | `Psychic` |
| Library targets | `dsp_engine` / `dsp_engine_static` (link name kept for compatibility) |
| Public headers | `dsp_engine.h` |
| C functions | `dsp_*` |

## License

See LICENSE.
