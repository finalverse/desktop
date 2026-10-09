# Phase 4B continuation — 2026-10-10

Phase 4B remains open. This pass implements the requested persistent local visual-check hook and explicit native bench approach/Sit/Stand acceptance sequence. The Mac is locked; no new viewer launch, screen capture, measured quick-check duration or visible seating acceptance is claimed.

## Repositories and package

| Repository | Start | State before this pass | Push |
|---|---|---|---|
| Viewer `dev/worktrees/viewer-ai-world` | `1500762635649d7de9e60c3c83ad3179f5cd3ea1` | Destination/discovery work dirty; preserved separately | None |
| MutSea `dev/worktrees/mutsea-worldops` | `f3dcd7f1ee435f873da853eb1043217148c8746b` | Clean; source unchanged in this pass | None |
| Studio `studio` | `72945c2aaebbfba6093f3f7aca4cc7c068ed36a5` | Clean; source unchanged in this pass | None |

All paths are below `/Users/wenyan/Finalverse`. The viewer branch is `codex/phase4b-destinations-import`. The package `/Users/wenyan/Finalverse/dev/phase4b/Finalverse QuickCheck.app` contains an ARM64 executable with SHA-256 `633e186f687d42bb3561221db97e169960f11fbd29f4c996fa12f0d336da2da7`; deep/strict ad-hoc verification passed. It includes preserved destination WIP, as did the preceding test package. Prior universal packages are intact. Intel execution/build acceptance for this increment is not claimed.

Xcode reported build success; its process remained in the observed exiting state and the idle CMake wrapper needed termination. See the [build limitation and commands](../rendering/quick-visual-check.md). The original SDK-cache wait, interrupted universal attempt, narrower ARM64 build and diagnostics remain outside Git. Disk space was about 60 GiB at continuation start; no user source or unrelated builds were removed.

## Tests and evidence

- **117/117 CTest entries passed**, 121.75 s. The changed benchmark target was rebuilt with **13/13 C++ cases**. Other prebuilt regression executables were exercised; this was not an ALL_BUILD of every test target.
- **24/24 Python tests passed** for launch/session guards, comparison validity, queue safety and process correlation.
- Actual package signature and ARM64 architecture verified; guarded loopback launch preflight passed without launching.
- Fresh bench native preflight verified **13 mesh/LOD assets, 13 semantic components, 13 material face bindings, 2 material assets**, native seat success and occupied Undo refusal. Receipt: `dev/phase4b/bench-seat-preflight-20261010.json.verify.json`. It uses a new copied receipt; prior evidence is intact.
- Remote credential guard passed. No credential values, private session XML or capability URLs were printed or committed.

The current local simulator is the existing PostgreSQL fixture on loopback port 18094, with inherited BulletSim/Meshmerizer. No server module, physics default, remote deployment, logical Lumi state or scene geometry was changed in this pass. The preflight used the designated test account with appearance sending disabled.

## Faster verification

Previously a focused capture still needed a new launch/login; native GUI inventory itself took 21.44 s in this pass and reported the Mac locked. The new [quick harness](../rendering/quick-visual-check.md) queues repeated requests in the same guarded session. Single/trio/full previews use 1/3/8 canonical views with fresh private evidence and JSON summaries. Default active capture targets are 10/30/80 s. Actual elapsed timings and camera restoration remain to be verified live.

Bench mode uses inherited grounded autopilot, native Sit, seated capture, native Stand and standing capture. The streamed owned root must match its receipt position. Position/rotation, seat parent and active animation IDs are recorded. The design requires no GUI clicks for that sequence; none were performed while locked. Camera checks produce no WorldLine events. Final controlled performance still uses the existing matched methodology after hero acceptance; no eight-camera benchmark was repeated here.

## Graphics and content

A private transient shadow-only session is prepared for the live interaction check: sun/moon shadows at resolution scale 0.5, existing FXAA/tone mapping, SSAO off and no added reflection detail. Prior controlled feature evidence gives Pavilion 58.04 → 57.91 FPS (−0.22%), versus the rejected combination's −14.36%. The shadow-only three-view weighted result was 58.88 → 56.76 FPS; Lumi p95 worsened 20.77 → 32.07 ms. These are earlier single-run observations, not measurements of the new package. Product defaults remain unchanged; final profile acceptance remains open.

Bench/tree/lantern content and earlier native persistence/Undo proofs remain intact. Production Lumi, pavilion, beacon, Home/Garden/waterfront composition and full lighting acceptance remain unfinished. The pinned CC0 MPFB body is an editable authoring base whose game-engine skeleton has zero matching native bone names; it must not be labeled a production native avatar or replace the canonical citizen yet. No OAR research/import was performed.

## Acceptance gates

`FAIL — open` means the gate is not yet accepted, including missing visual evidence; it does not imply a new regression. Earlier narrow proofs are called out separately.

| Gate | Status | Evidence / remaining work |
|---|---|---|
| B1 visible sit | FAIL — open | Native protocol passes; approach/seated frames await unlock |
| B2 stand | FAIL — open | Native probe succeeds; visible return/alignment await unlock |
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
| B26 regression tests | PASS, current increment | 117 CTest entries, 13 changed C++ cases, 24 Python tests |
| B27 credentials clean | FAIL — open | Current remote scan passes; historical cached Agni provenance incident remains unresolved |
| B28 PostgreSQL restart | PASS, existing fixture | Earlier restart/Undo proofs retained; fresh bench preflight succeeds |

## Resume

After manual unlock, use the prepared guarded launch with the private queue and Day warm-start fixture. Verify two short captures in the same PID, then the close bench sequence; review its three images together. Save actual duration and restoration evidence before accepting the harness and visible seat. Continue the requested order through conservative graphics, Golden Hour/Night, native Lumi and hero scene content. Phase 4C and OAR imports remain outside this critical path.
