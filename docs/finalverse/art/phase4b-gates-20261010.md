# Phase 4B continuation — 2026-10-10

Phase 4B remains open. After unlock, the persistent visual hook was run and a real bench alignment defect was corrected in Starter Library v0.1.8. Settled single checks took 10.40 / 10.59 seconds; corrected front/side native Sit/Stand sequences took 21.33 / 21.28 seconds without GUI clicks. Both visible seating gates pass for the current test avatar. [Detailed source, trial failures, evidence and limits](bench-interaction-20261010.md) are retained; production Lumi, hero content and full lighting remain unfinished.

**Later integrated continuation:** [the current ledger](../phase4b-integrated-plan.md) adds and records the completed passive OAR compatibility milestone and [Genesis source audit](genesis-native-pipeline.md). New tools have 26 OAR and 32 viewer Python tests; C++ binary and earlier CTest results below are unchanged. The OAR experiment runs in a separate PostgreSQL fixture; none of B3–B25's unfinished Harbor/production-avatar gates becomes a pass from that restore. The historical narrative below describes the earlier bench pass. Its former “OAR outside the critical path” ordering is superseded by the integrated brief.

## Repositories and package

| Repository | Start | Scope | Push |
|---|---|---|---|
| Viewer `dev/worktrees/viewer-ai-world` | `1500762635649d7de9e60c3c83ad3179f5cd3ea1` | Persistent hook/launch validation; destination WIP preserved separately | None |
| MutSea `dev/worktrees/mutsea-worldops` | `f3dcd7f1ee435f873da853eb1043217148c8746b` | Clean; source unchanged in this pass | None |
| Studio `studio` | `72945c2aaebbfba6093f3f7aca4cc7c068ed36a5` | Clean at start; shared seat definition/metadata authoring changed after unlock | None |

All paths are below `/Users/wenyan/Finalverse`. The viewer branch is `codex/phase4b-destinations-import`. The package `/Users/wenyan/Finalverse/dev/phase4b/Finalverse QuickCheck.app` contains an ARM64 executable with SHA-256 `633e186f687d42bb3561221db97e169960f11fbd29f4c996fa12f0d336da2da7`; deep/strict ad-hoc verification passed. It includes preserved destination WIP, as did the preceding test package. Prior universal packages are intact. Intel execution/build acceptance for this increment is not claimed.

The unlocked continuation starts at viewer `ea58a91a512c3437b7a0be290e109e2ad519b1c3`; MutSea/Studio starts are as above. This continuation changes viewer Python launch validation and Studio source/seat metadata. C++ and the packaged executable are unchanged; MutSea source remains clean. The isolated fixture catalog/bench instances changed through the existing authorized content path.

Xcode reported build success; its process remained in the observed exiting state and the idle CMake wrapper needed termination. See the [build limitation and commands](../rendering/quick-visual-check.md). The original SDK-cache wait, interrupted universal attempt, narrower ARM64 build and diagnostics remain outside Git. Disk space was about 60 GiB at continuation start; no user source or unrelated builds were removed.

## Tests and evidence

- **117/117 CTest entries passed**, 121.75 s. The changed benchmark target was rebuilt with **13/13 C++ cases**. Other prebuilt regression executables were exercised; this was not an ALL_BUILD of every test target.
- **28/28 Python tests now pass** for launch/session guards, comparison validity, queue safety, process correlation and native warm-start/camera limits. The preceding 24-test result remains earlier evidence.
- **5/5 Studio metadata-revision tests** and **13/13 native content tests** pass for the corrected package.
- Actual package signature and ARM64 architecture verified; guarded loopback launches, repeated captures, restoration spot check and visible bench interaction completed after unlock.
- Fresh bench native preflight verified **13 mesh/LOD assets, 13 semantic components, 13 material face bindings, 2 material assets**, native seat success and occupied Undo refusal. Receipt: `dev/phase4b/bench-seat-preflight-20261010.json.verify.json`. It uses a new copied receipt; prior evidence is intact.
- Remote credential guard passed. Private account/session XML, capability URLs and raw receipts remain outside Git.

The current local simulator is the existing PostgreSQL fixture on loopback port 18094, with inherited BulletSim/Meshmerizer. The corrected bench's retained geometry/materials were reimported as a new version and earlier trials retired by guarded Undo. A final simulator restart reverified all 13 mesh/LOD/semantic/material-face records, two material assets, native Sit and occupied Undo refusal. Fresh real login/UDP regression preserved the original 40 IDs/transforms; 100 components received, normal logout confirmed. No server module, physics default, remote deployment or logical Lumi state changed. Protocol probes disable appearance sending.

## Faster verification

Previously a focused capture still needed a new launch/login; native GUI inventory itself took 21.44 s before unlock. The [quick harness](../rendering/quick-visual-check.md) queues requests in the same guarded session. Single/trio/full previews use 1/3/8 canonical views with fresh private evidence and JSON summaries. Settled single checks measured 10.40 / 10.59 s; earlier requests took 47.57 / 72.44 s, so this is not a cold-login timing guarantee. Camera restoration/FOV and a returned third-person view were checked. Trio/full timings remain configured targets, not measured results.

Bench mode uses inherited grounded autopilot, native Sit, seated capture, native Stand and standing capture. The streamed owned root must match its receipt position. Position/rotation, seat parent and active animation IDs are recorded. Front and side review exposed v0.1.6 clipping and a rejected v0.1.7 overcorrection, then accepted v0.1.8 contact/clearance. No GUI clicks were needed inside any queued sequence. GUI was used for occasional startup notices, a restoration check and normal logout before required catalog reload/restart. Camera checks produce no WorldLine events; content changes do. No eight-camera benchmark was repeated.

A shortened five-second normal warm-start was rejected by native validation. The launcher now checks the native 10–60-second windows, typed UUID and camera/FOV bounds before opening the app. The later 10/10-second Day warm-start completed. Blender also blocked before script execution in macOS volume enumeration; the seat-only revision reuses hash-checked authoring payloads without rerunning it. Rejected trials and diagnostics are retained.

## Graphics and content

A private transient shadow-only session is prepared for the live interaction check: sun/moon shadows at resolution scale 0.5, existing FXAA/tone mapping, SSAO off and no added reflection detail. Prior controlled feature evidence gives Pavilion 58.04 → 57.91 FPS (−0.22%), versus the rejected combination's −14.36%. The shadow-only three-view weighted result was 58.88 → 56.76 FPS; Lumi p95 worsened 20.77 → 32.07 ms. These are earlier single-run observations, not measurements of the new package. Product defaults remain unchanged; final profile acceptance remains open.

Bench/tree/lantern content and earlier native persistence/Undo proofs remain intact. Production Lumi, pavilion, beacon, Home/Garden/waterfront composition and full lighting acceptance remain unfinished. The pinned CC0 MPFB body is an editable authoring base whose game-engine skeleton has zero matching native bone names; it must not be labeled a production native avatar or replace the canonical citizen yet. No OAR research/import was performed.

## Acceptance gates

`FAIL — open` means the gate is not yet accepted, including missing visual evidence; it does not imply a new regression. Earlier narrow proofs are called out separately.

| Gate | Status | Evidence / remaining work |
|---|---|---|
| B1 visible sit | PASS, current fixture | v0.1.8 front/side seat contact and backrest clearance; human test avatar |
| B2 stand | PASS, current fixture | Native Stand confirmed; full-body side view on ground |
| B3 collision path | FAIL — open | Four inherited walking probes pass; full continuous hero route remains |
| B4 pavilion walkability | FAIL — open | Production entrance/interior not built |
| B5 lightweight Balanced selected | FAIL — open | Shadow-only candidate prepared; not a saved default/final acceptance |
| B6 Golden Hour | FAIL — open | Foreground fill still needs revision |
| B7 Night | FAIL — open | Bounded lantern pool passes; destination lighting hierarchy incomplete |
| B8 Pavilion hotspot | FAIL — open | Prior shadow-only cost small; final pavilion absent |
| B9 production Lumi | FAIL — open | Seven-part placeholder still present; editable CC0 base only |
| B10 native rig | FAIL — open | Source skeleton incompatible; proper binding/animation validation required |
| B11 idle/walk/sit | FAIL — open | Required on production Lumi, not proven by human bench tests |
| B12 talk/listen/greeting | FAIL — open | Production avatar gesture states pending |
| B13 identity unchanged | PASS | No citizen identity/home/memory mutation in this pass |
| B14 pavilion | FAIL — open | Production social place pending |
| B15 beacon | FAIL — open | Hero landmark upgrade pending |
| B16 arrival composition | FAIL — open | Current sandbox composition not accepted |
| B17 waterfront | FAIL — open | Small assets available; final composition pending |
| B18 Lumi Home visuals | FAIL — open | Existing semantic home preserved; visual upgrade pending |
| B19 Garden visuals | FAIL — open | Existing relationship preserved; coherent garden pending |
| B20 AI recognizes hero assets | FAIL — open | Imported small-asset semantics pass; all final hero assets not present |
| B21 home relationship | PASS, existing state | Prior citizen/home proofs preserved; must verify after embodiment migration |
| B22 navigation | PASS, existing state | Prior citizen route/native walking proofs; production avatar navigation pending |
| B23 WorldLine | PASS, existing state | Existing content journal/guarded Undo preserved; no structural changes here |
| B24 improved final screenshots | FAIL — open | Final eight-view hero review not performed |
| B25 final FPS | FAIL — open | Final scene/performance run not ready |
| B26 regression tests | PASS, current increment | Unchanged binary: 117 CTest / 13 C++ cases; current 28 Python / 5 Studio / 13 native content tests |
| B27 credentials clean | FAIL — open | Current remote scan passes; historical cached Agni provenance incident remains unresolved |
| B28 PostgreSQL restart | PASS, current fixture | Final v0.1.8 bench native/semantic/material/Sit checks and original 40 transforms survive simulator restart |

## Resume

Resume from the [integrated ledger](../phase4b-integrated-plan.md): retain these bench proofs, finish collision/lighting, turn the completed isolated OAR experiment into cleared reusable content, prove native Genesis Lumi and Elara, then complete hero composition. Use the guarded queue for ordinary checks; full eight-view scene/performance acceptance follows hero completion. Phase 4C remains later work.
