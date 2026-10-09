# Phase 4B visual convergence — 2026-10-09

Phase 4B remains open. This continuation implements native factor-only PBR delivery, independent bench LODs and inherited sitting, and measures lighter graphics options in the actual viewer. Production Lumi, Harbor composition and a lit night scene are still required.

## Tested state

Starting commits: viewer `1825d16852fde63c8f2adbcf610fd5163ce2e7e5`, MutSea `a20dfa7c6212efcda228f5069d18e8042a00d337`, Studio `2fef66b333e664bb07ba2394123b1382b0ec7d02`. Existing viewer destination-discovery changes remain separate and unstaged. No push or remote deployment is included.

Universal macOS build and **117/117 CTest entries passed** (119.18 seconds). Executed ARM64 test binary SHA-256: `d7e7fb8a51a5b5dc867b01aa0dd399b47e568c85e44828f2eb9d8708d38c7f10`. The app is `/Users/wenyan/Finalverse/dev/phase4b/Finalverse FirstLook.app`. Intel execution was not tested. Approximately 13 GiB remained available; active packages, Blender sources, profiles and database backups were preserved.

## Individual graphics trials

The existing native harness now accepts a unique subset of its eight views. These trials use **CAM_Lumi, CAM_Pavilion, CAM_Waterfront**, in that order, with 15 seconds warm-up and 15 seconds sample per view. All have identical camera/FOV, pinned sky/water, 1280 × 738 raw pixels and cached object counts 348 / 354 / 367. Recorded graphics confirm only the named controls differ. UI/HUD are excluded from PNGs. Frame intervals are CPU-observed, not GPU timestamps.

| Feature trial | Weighted FPS | Change from baseline | Mean frame ms across views | Visual decision |
|---|---:|---:|---:|---|
| Baseline | 58.88 | — | 16.99 | FXAA, no shadows/SSAO/anisotropy; inherited ACES |
| Sun/moon shadows, resolution scale 0.5 | 56.76 | −3.59% | 17.63 | Visible ground contact and depth; keep for combination test |
| SSAO | 48.85 | −17.04% | 20.49 | Subtle contact improvement; reject for this lighter profile |
| SMAA | 57.83 | −1.78% | 17.29 | Small edge improvement; test with shadows |
| 4× anisotropic filtering | 55.85 | −5.14% | 17.94 | Small terrain/filtering benefit; first view slower, verify combined cost |
| Reflection probe detail 1 | 55.04 | −6.52% | 18.18 | No clear benefit in this sparse scene; retain detail 0 |
| Tone-map control 0 | 59.09 | +0.36% | 16.92 | No obvious improvement; retain existing control 1 |

Weighted FPS means total frames divided by total measured seconds. Mean-ms column is the mean of the three camera means. Camera medians and p95, PNGs and CSVs are retained under `dev/phase4b/visual-ab-*-20261009`. These are single runs with variable frame pacing and avatar pose/bake readiness, not isolated GPU cost estimates or a statistical confidence interval. All seven runs passed the same camera/environment/count comparison. PBR cost was not isolated: the matched scene contains the new PBR bench in every trial. Do not attribute the difference from the older 64.11 FPS scene baseline to materials alone.

The earlier shadows+SSAO candidate at 48.55 FPS remains rejected. The new combination keeps modest shadows, SMAA and 4× filtering, with SSAO off and probe detail 0. Its complete matched eight-camera run has now been reviewed:

| Camera | Baseline FPS | Light candidate FPS | Change | Baseline / candidate p95 ms |
|---|---:|---:|---:|---:|
| Arrival | 58.7 | 72.2 | +22.97% | 24.97 / 26.25 |
| Harbor | 61.7 | 65.7 | +6.54% | 24.88 / 25.89 |
| Lumi | 62.5 | 73.3 | +17.29% | 24.73 / 20.42 |
| Lumi Home | 64.4 | 70.6 | +9.59% | 24.39 / 24.82 |
| Garden | 61.0 | 73.1 | +19.91% | 24.46 / 23.97 |
| Pavilion | 61.9 | 53.0 | −14.36% | 24.32 / 31.98 |
| Waterfront | 58.2 | 59.7 | +2.55% | 24.60 / 25.71 |
| Sunset | 63.8 | 57.1 | −10.43% | 23.07 / 30.58 |

Weighted FPS is **61.53 → 65.60 (+6.62%)**. Build/channel, complete fixture apart from output directory, each camera, sky/water, resolution and cached counts match. Only the five declared shadow/AA/filtering controls differ. This does not prove a general speedup: frame pacing varies and Pavilion/Sunset regress with worse p95. Shadows visibly ground Lumi, the home and pavilion, but the pavilion becomes too dark and the low-resolution Garden roof shadows show streaking. Keep this an opt-in experiment, with product defaults unchanged. Balanced's final visual/performance gate remains open.

All sixteen native images were reviewed in two paired contact sheets under `dev/phase4b/visual-convergence-paired-sheet-{1,2}.png`; source PNGs/CSVs remain in the two `visual-convergence-*-20261009` directories. The sheets resize evidence for review without changing scene content. They are technical graphics comparisons, not the finished Harbor hero before/after sheet.

## Material and interaction evidence

Bench v0.1.6 contains 13 parts, warm wood/dark metal factors, no textures or fullbright and **1,404 / 754 / 286 / 130** triangles. MutSea stores native AT_MATERIAL assets, binds inherited RenderMaterials face parameters, and serves them through ViewerAsset to the inherited glTF/shader path. Native tests check actual payload factors and separate mesh LOD blocks. See MutSea `docs/mutsea/content-import.md` for the exact delivery boundary and remaining textured-material work.

The live PostgreSQL fixture verified all thirteen mesh/PBR bindings and semantic records, two delivered material assets, thirteen delivered mesh/LOD hashes, native sit position/orientation, seated state, and occupied-seat Undo refusal without an added intent. PostgreSQL-plus-simulator restart and guarded Undo passed; a fresh bench is retained at the approved creator site. Warm wood and a darker frame are visible in the native Waterfront image. Close sitting pose, collision/walking and live LOD transitions remain separate visual gates.

MutSea validation: 29 kernel/catalog tests, 20 package validator tests, 11 native tests against the exact runtime assemblies and 51 existing world/navigation tests passed. The protocol probe uses its documented Unix-compatible SDK separately from simulator assemblies. The local reusable image includes the active PostgreSQL service components; its final component/image receipt and consistent predeployment backup remain private. Remote runtime is unchanged.

The tree continuation raises coverage to **30 kernel / 21 package / 12 native bench / 14 native tree / 51 world tests**. The branched tree has three parts and 1,280 / 704 / 278 / 128 triangles. Whole-part collision in v0.1.7 was rejected and quarantined; v0.1.8 pins a 0.32 × 0.32 × 2.9 m shaft hull and native noncolliding foliage children. Actual mesh/material assets, semantic records and all three physics types survived PostgreSQL-plus-simulator restart and guarded Undo. A reusable `pbr-v018` image and private backups preserve the active module/PG service components.

The v0.1.8 native diagnostic close-up shows a readable branched silhouette and restrained opaque foliage, but most blossoms are buried in overlapping crowns. The medium/far diagnostic views were blocked by an inherited crown at (155,101,30); they do not pass distance acceptance. All failed/occluded evidence is preserved. Studio v0.1.9 places blossoms beyond the analytic canopy union surface; a relevant three-view 10-second-window retest uses clear viewpoints to the east. These asset-diagnostic coordinates differ from the canonical graphics benchmark and must not be compared against its FPS.

The v0.1.9 retest completed all three views with native PNG/CSV evidence: near (157,114,27.8), middle (181,103,31), distant (214,68,41), looking at the tree anchored at (160,120,25). Blooms are visible and the silhouette remains present across distance, without the inherited crown blocking the target. The faceted canopy and geometric blossoms remain a stylized draft, not the reference image's production foliage quality. Exact selected LOD indices and continuous transition popping were not instrumented; this review does not close that gate. v0.1.9's three mesh/material/semantic/physics records passed live import. v0.1.8 remains the verified full database-restart/Undo trial for the same collider contract.

After the collider deployment, the real protocol probe again verified all **40 original Harbor IDs/transforms**, HTTP login, UDP streaming and logout. Lumi's agent ID, display name, avatar binding, home ID, memory and profile match the pre-tree snapshot. The seven-part embodiment remains a placeholder.

## Environment acceptance

Day, Golden Hour and Night are local `ENV_LOCAL` previews, not region publication. Relevant native images, frame statistics and environment payloads are retained under `dev/phase4b/visual-env-*-20261009`.

- Day is readable, with clear sky, foliage and water. Primitive content remains visible.
- Golden Hour gives a warm edge and water reflection, but its foreground face/foliage fill is too dark for the hero first impression. Revise before publication.
- Night exposes inadequate world-light hierarchy. Add bounded native path/interior/pavilion/beacon lights; emissive factors alone do not illuminate the scene.

Preserve the numerical library's original version and its separate inherited asset provenance. No region environment, canonical Harbor identity or Lumi logical identity has been replaced by these previews.

## Launch and GUI discipline

`launch_harbor_benchmark.py` verifies the exact executable hash, isolated profile, explicit loopback grid map, owned private session, no cached credential-saving flags and a new designated-region evidence directory. Thirteen Python launch/process/comparison tests pass. Use its private session file; credentials never belong in command arguments or reports.

`check_harbor_benchmark_process.py` verifies the exact active executable, expected grid/session/spec arguments and completed native evidence before a GUI handoff. It is read-only. Bind the UI only after a guarded launch; never call a launch-capable UI binding again after Quit, because it can reopen the app with cached defaults. Native Quit performs the grid logout handshake. A tested signal-based stop skipped that handshake and left a stale disposable session, so it was removed.

During shutdown the UI tool reopened a cached login screen. It was closed without submitting login; the current cached-profile log showed login-show/wait states, zero auth-init mentions and zero login-failure messages. These observations do not resolve the earlier cached Agni credential-provenance incident. The earlier disposable-password disclosure remains rotated/remediated; no new secret is included in this continuation's reports.

The canonical camera was verified in an extended active sample. Its reset to the human's normal camera after a completed run is expected; it is not a camera mismatch. An initial mistaken observation was corrected, and no camera implementation change was made.

### Faster screen-test workflow

At the user's request, prefer **10-second warm-up + 10-second sample**, one or two relevant views for content iterations (20–40 seconds of sampling windows). Use all eight at milestones; avoid rerunning already-passed build/protocol suites without a related change. The native harness writes PNG/CSV/results unattended. It requires foreground focus and restarts the current sampling window if focus, dimensions or cached count changes; this can extend elapsed time. Select the running guarded app once, activate it, then leave it alone. Do not repeatedly take UI screenshots or reopen a terminated bundle.

Review native images in batches. Use `harbor_benchmark.py compare BASELINE/results.xml CANDIDATE/results.xml --changes RenderAnisotropic RenderAnisotropicLevel RenderFSAAType RenderShadowDetail RenderShadowResolutionScale` for this trial. The command refuses incomplete evidence, changed fixture/build/cameras/environment/pixels/counts or undeclared graphics changes. Use GUI only for interaction/pose acceptance and native logout. A startup AX error is not evidence of a failed viewer; inspect the process/evidence before retrying a UI binding.

## Remaining delivery

The full matched graphics comparison is complete; it does not justify a universal default. Close bench walking/sitting-pose and exact live LOD transition gates, then vegetation quality, a reviewed pavilion-scale placement/collision contract and beacon review. The current small-asset contract must not be silently widened for buildings. Follow [Lumi's character specification](lumi-character-spec.md) for the pinned legal authoring/rig compatibility spike and [OAR roadmap](../world/oar-import-roadmap.md) for later conversion; neither document counts as a production asset.

| Requested gate | Current evidence / remaining work |
|---|---|
| A1 viewer tests | Pass: universal build, 117 CTest entries |
| A2 lighter comparison | Pass: seven individual trials and matched full eight-view pair |
| A3 environment previews | Captured/reviewed all three; Golden Hour fill and Night world lights need revision |
| A4 accepted Balanced profile | Open: provisional combination has Pavilion/Sunset regressions and shadow/readability artifacts |
| A5 first native PBR asset | Pass: bench material assets/bindings and actual native rendering |
| A6 color space | Factor path verified; textured normal/ORM paths not in this release contract |
| A7 bench collision behavior | Open: native convex payload exists; avatar walking/blocking test remains |
| A8 bench seating | Native response, seated state and occupied Undo refusal pass; visible pose remains |
| A9 bench/tree LOD | Distinct native blocks/counts and delivered hashes pass; exact live level transitions remain |
| A10 blossom-tree art | v0.1.9 near/middle/distant native review complete; visible blooms/silhouette improved, production foliage quality remains open |
| A11 pavilion | Open: building-scale contract/material/walking review not implemented |
| A12 beacon | Open: hero landmark has not passed native evaluation |
| A13 Lumi strategy | Selected and documented; exact legal source pin/rig spike still precedes production embodiment |
| A14 Phase 3 preservation | Original 40 Harbor IDs/transforms and Lumi identity/home/binding/memory/profile match |
| A15 credential discipline | Current guarded launches/private evidence checked; earlier Agni provenance incident remains unresolved |

The final before/after hero sheet waits for real changes to Lumi, home/garden, pavilion, landmark and composition. This technical delivery does not satisfy that visual milestone.
