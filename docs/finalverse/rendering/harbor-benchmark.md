# Native Harbor visual benchmark

This developer harness reuses the viewer camera, frame interval and native PNG snapshot implementation. The inherited agent pilot interpolates views and moves the avatar; this static fixture keeps avatar and world state untouched. It is limited to the designated loopback Phase 4B grid, requires its region UUID, one to eight unique bounded named cameras, finite coordinates/FOV, 10–60 second warm-up/sample windows and a new output directory. It refuses existing output directories rather than overwriting evidence.

## Run

Generate a noncredential LLSD specification outside Git:

```sh
python3 scripts/finalverse/harbor_benchmark.py spec \
  --file /absolute/new-spec.xml --output-dir /absolute/new-captures
```

Normally launch the isolated app with its authorized MutSea profile and private one-session login settings. Add `--set FinalverseVisualBenchmarkFile /absolute/new-spec.xml`. The benchmark control is nonpersistent: the inherited settings loader intentionally skips nonpersistent controls in session files, so its path must be passed with `--set`. A session file still supplies ordinary graphics overrides transiently (`settings_files.xml` marks Session `persistent=false`). Never supply passwords as command-line arguments.

For an authorized private `UserLoginInfo` three-element session payload, set `AutoLogin=false`, `RememberUser=false` and `RememberPassword=false`. The inherited startup path accepts that one-session payload without showing the login panel. `AutoLogin=true` instead forces both remember flags on, even if command-line overrides previously set them false. Never use a generic app reopen to start a benchmark: explicitly select the isolated profile and grid, and verify process exit before launching the next session.

Keep the viewer in the foreground, unminimized, with the Mac unlocked. A focus loss resets the current view's warm-up and samples; object-count/resolution changes do the same. Camera changes and screenshot encoding are excluded by the next warm-up. Builds and other heavy jobs must finish before capture. Do not interact with or move the avatar during the run.

For a focused visual change use `--views CAM_Waterfront --warmup 10 --sample 10` rather than repeating all eight views. Keep the full benchmark for milestones. `--quit-after-capture` adds an explicit boolean opt-in: after successful native PNG/CSV/report writes, the viewer requests its existing save/upload/logout state machine. It does not quit on failed capture or a rejected fixture. Omitted/false keeps the previous interactive behavior. This avoids a manual Quit confirmation; it does not permit a signal-based stop or make a locked/unfocused viewer valid evidence.

Use `--size 1280 738` to request matching capture pixels through the inherited native window API after login. The macOS context starts from its XIB window; profile `WindowWidth`/`WindowHeight` alone did not establish the requested dimensions in the tested isolated app. Cocoa requests use logical points, while captures use backing pixels. The harness converts that scale, permits one bounded correction from measured dimensions to accommodate the inherited fixed titlebar allowance, waits at most five active seconds for native resize callbacks, and fails if actual raw pixels differ. Integer dimensions must be between 640–2560 by 480–1600; native minimum window dimensions may make smaller requests unattainable. It does not resize saved evidence to manufacture a match. Omitted dimensions preserve the existing window behavior.

Build only the changed targets when appropriate:

```sh
cmake --build build-darwin-universal --config RelWithDebInfo --parallel 2 \
  --target secondlife-bin PROJECT_secondlife-bin_TEST_llfinalversevisualbenchmark
ctest --test-dir build-darwin-universal -C RelWithDebInfo \
  -R finalversevisualbenchmark --output-on-failure
```

Run one build per Xcode build directory. A second build cannot attach to the same build database. The fail-closed launcher accepts only the two explicit development bundle identities, the selected executable hash, the owned private one-session file and loopback-only isolated profile. Never bind/reopen a stopped app through a UI tool: that can discard these launch arguments.

```sh
python3 scripts/finalverse/harbor_benchmark.py report /absolute/new-captures/results.xml
```

The eight PNGs omit UI/HUD; each CSV retains actual frame intervals. `results.xml` records sample FPS, mean/median/nearest-rank p95, raw pixels, camera, cached object count, whitelisted graphics and full native sky/water settings. It does not dump credentials or all viewer settings. Files are private. Capture/report I/O is not counted as steady rendering. Frame intervals are not GPU timestamps. Export CPU/RSS separately; unavailable GPU/draw-call metrics must not be fabricated.

## Compare graphics and environment

`finalverse-balanced.xml` is an opt-in **candidate** session graphics profile, not a new saved default. Merge its ten declared controls into a new private login session file; preserve the original session/profile. Match region, cameras, resolution, object/avatar population and sky/water to baseline.

The first M1 comparison rejected this candidate (~24% lower FPS). See [the actual baseline report](../art/baseline-20261009.md). Do not adopt it as a product default from its name or the presence of an XML file.

`harbor_environment.py` produces local sky/water preview specs from a completed baseline and the Studio numerical environment library. `--variant matched` pins baseline sky/water; other variants apply the original day/golden-hour/night overrides. The runner validates native settings and selects `ENV_LOCAL`; it does not publish region environment changes. Native default assets and serialization retain inherited provenance; the original numerical overrides do not establish redistribution rights to those assets. Regional publication, audit and persistence are later acceptance gates.

Ten C++ unit cases cover fixture/path/camera bounds, duplicate evidence names, NaN, sample duration, frame-statistic arithmetic, environment payloads, explicit automatic logout and bounded integer capture dimensions. Sixteen Python launch/process/comparison cases cover the private profile, executable, session and matched-evidence guards. Visual acceptance must inspect the saved images; a completed report alone does not establish attractive content or successful interaction.
