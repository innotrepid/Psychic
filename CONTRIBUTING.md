# Contributing to Psychic

Thank you for your interest in improving **Psychic** (adaptive audio DSP engine).

Canonical org repo: https://github.com/Cryptic-Systems/Psychic  
The public C ABI remains `dsp_*` for compatibility with existing hosts.

## Development setup

```bash
git clone https://github.com/Cryptic-Systems/Psychic.git
cd Psychic
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DDSP_BUILD_EXAMPLES=ON
cmake --build .
ctest --output-on-failure
```

## Guidelines

- Prefer realtime-safe code on the audio path (no allocations in `dsp_process*`).
- Keep the public C ABI stable; add new `dsp_*` entry points rather than breaking existing ones.
- Add or extend unit tests under `tests/` for new behavior.
- Document host-facing behavior in `docs/RESONATE_INTEGRATION.md` when Resonate integration is affected.

## Pull requests

1. Branch from `main`.
2. Include a clear description of the change and test results.
3. Do not commit build artifacts (`build/`).

## License

By contributing, you agree that your contributions are licensed under the same terms as the project (see LICENSE).
