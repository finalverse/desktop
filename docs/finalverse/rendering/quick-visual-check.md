# Repeated local visual checks

Verified on 2026-10-10 after unlock: repeated single-view requests took **10.40 / 10.59 seconds** once the scene settled, in the same viewer PID. Early requests took 47.57 / 72.44 seconds; focus loss and changing cached object counts can restart the steady sample, so the fast target is not a cold-login guarantee. Native restoration flags/FOV and a returned third-person UI view were checked. Corrected bench front/side sequences took **21.33 / 21.28 seconds**, including Sit and Stand, without GUI clicks. See the [actual bench evidence and limits](../art/bench-interaction-20261010.md).

The viewer can accept another bounded camera check while the same authorized loopback session remains logged in. This opt-in developer hook reuses native camera, snapshot and frame sampling. It does not expose a network endpoint, accept scripts or invoke AI. `FinalverseQuickVisualDirectory` is empty by default and never saved.

Create a new absolute command directory, owned by the operator with mode 0700. Start the isolated test app once with the existing guarded launcher and add `--quick-directory /absolute/private-queue`. That launch still requires the selected executable SHA-256, a loopback-only isolated profile and an owned mode-0600 one-session login file. Never reopen a stopped test app through a UI tool.

After the initial warm-start capture completes, keep the viewer foreground, visible and unminimized. Submit a check:

```sh
python3 scripts/finalverse/quick_visual_check.py \
  --app /absolute/Finalverse\ QuickCheck.app \
  --session /private/one-session.xml \
  --directory /absolute/private-queue \
  --binary-sha256 SELECTED_EXECUTABLE_HASH \
  --mode single --camera CAM_Waterfront
```

| Level | Use | Views | Default active capture time |
|---|---|---|---|
| Fast | One asset iteration | Selected canonical target | 5 s warm-up + 5 s sample |
| Feature | Meaningful content change | Arrival, target, Pavilion (Harbor supplies the third distinct view if needed) | 30 s |
| Full preview | Batch scene review at a milestone | All eight canonical views | 80 s |

Full quick preview remains a feature check. Use the existing longer matched `harbor_benchmark.py` methodology for final controlled release performance comparisons. These short samples must not be compared to older baselines as evidence of a speed improvement.

Launch readiness uses normal native **10–60 second warm-up/sample windows**. Use the queue for shorter 2–10 / 3–10 second quick windows. The launcher now checks those native limits, typed region UUID, camera geometry/FOV and environment map structure before opening the app. A shortened five-second normal warm-start was rejected by the viewer; its evidence remains retained. The launcher prevents repeating that wasted launch and refuses avatar interaction in a launch-readiness specification. Native runtime validation remains authoritative.

`--position X Y Z --focus X Y Z` overrides only the selected target; finite in-region coordinates, a nonvertical direction and bounded FOV remain mandatory. The result records the exact camera. No override changes a canonical definition on disk. Each accepted request produces a new immutable capture directory containing native PNGs, frame CSVs, `results.xml` and `summary.json`. The JSON includes elapsed wall time, camera restoration, actual pixels, environment and graphics settings. Screenshots and file writing are outside the next steady sample.

The queue polls at most four times per second only when configured and logged into the designated region. Both sides reject symlinks at the command endpoint and unowned/nonprivate files. Submission uses an atomic no-overwrite hard link; a pending or active request blocks the next submission. The viewer verifies the actual login URI and region again, keeps output inside the private queue, and never replaces an earlier capture. The CLI matches the running executable hash, exact grid, private session path and command directory without printing process arguments or credentials. Requests do not resize the window, change the environment or log out the session. The temporary camera and FOV are restored after success or failure. Focus loss restarts sampling; an active request has a 180 s wall-clock deadline.

## Native bench interaction

Use `--seat-id FIXTURE_UUID --seat-position X Y Z` with a single view and a close target camera. Add `--seat-any-owner` to sit on shared seating that the logged-in account does not own, such as estate-provisioned place benches. It is an explicit opt-in; the seat ID, region and 5 cm root-position checks still apply, and sitting changes no content. The approach point is in front of the seat in its own frame, so rotated benches are approached correctly. This is a separate explicit operator acceptance action. The viewer requires the streamed bench to belong to the logged-in operator and match its receipt's root position within 5 cm. It refuses an already seated, flying or autopilot-controlled avatar.

The sequence uses inherited grounded autopilot (45 s maximum), native Sit, a seated sample, native Stand and a standing sample. It checks actual parent/root UUID, records playing animation IDs and position/rotation, and captures approach/seated/standing frames. No teleport, geometry mutation or new character controller is involved. Native permission/collision handling remains authoritative. On failure, the harness stops only its own autopilot and requests Stand if it initiated Sit. It does not certify visual alignment merely because a protocol confirmation passed: review the three frames together for seat orientation, foot placement and clipping.

Batch the images after capture. Use GUI automation only for a concrete visual failure or a manual product flow that has no native hook.

## Local build

For the current Apple Silicon development package:

```sh
cmake --build build-darwin-universal --config RelWithDebInfo --parallel 2 \
  --target secondlife-bin PROJECT_secondlife-bin_TEST_llfinalversevisualbenchmark \
  -- SDK_STAT_CACHE_ENABLE=NO ARCHS=arm64 ONLY_ACTIVE_ARCH=YES
```

The directory retains its historical universal name, but this override produces an ARM64 executable. Prior signed universal test packages remain separate. This is not an Intel verification. The per-build SDK stat-cache override follows the [Swift Build task policy](https://github.com/swiftlang/swift-build/blob/main/Sources/SWBTaskConstruction/TaskProducers/TaskProducer.swift); it is not a global Xcode preference. On this machine the cache scanner waited in `CFRunLoopRun`, and a fresh regenerated cache's validation also timed out. Changing the cache option invalidated many compilation commands, so the first build was substantially longer than an ordinary feature increment. Retain the same override for subsequent local increments.

Xcode printed `BUILD SUCCEEDED`, produced the new executable and finished its tasks, but remained in the observed `?E` exiting state. The idle CMake wrapper was terminated after packaging and tests; do not describe its shell exit as zero. The package passed deep/strict ad-hoc signature verification. Current receipt: `dev/phase4b/quick-viewer-build-receipt-20261010.json`, outside Git.
