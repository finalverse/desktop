# Visible bench and fast checks — 2026-10-10

The bench seat defect is fixed in **MutSea Starter Library bench v0.1.8**. This is a focused Phase 4B acceptance, not completion of the hero experience. Production Lumi, pavilion, scene composition, the continuous collision route and complete lighting remain open.

## Source and boundary

This unlocked continuation starts at viewer `ea58a91a512c3437b7a0be290e109e2ad519b1c3`, MutSea `f3dcd7f1ee435f873da853eb1043217148c8746b`, Studio `72945c2aaebbfba6093f3f7aca4cc7c068ed36a5`. The tested ARM64 executable remains SHA-256 `633e186f687d42bb3561221db97e169960f11fbd29f4c996fa12f0d336da2da7` in `dev/phase4b/Finalverse QuickCheck.app`. This pass changes Python launch validation and original Studio seat metadata; it does not rebuild C++, change the inherited renderer/animation/physics implementation, or alter MutSea source. Existing viewer destination work remains separate and dirty. Nothing is pushed.

The only world changes are isolated fixture imports/retirements through existing content preview, commit, occupied-seat policy, fingerprint checks, Undo and WorldLine. The original 40 Harbor entity IDs/transforms, canonical Lumi identity, home and memory remain unchanged. Trial bench instances receive new import IDs; this is not presented as an atomic canonical-entity migration.

## What the live viewer found

| Package | Root-relative target | Result |
|---|---|---|
| v0.1.6 | `[0, 0.225, 0.10]` | Native Sit confirms, but the torso clips through the seat |
| v0.1.7 | `[0, 0.075, 0.50]` | Torso clears, but close review shows floating; rejected |
| v0.1.8 | `[0, 0.075, 0.30]` | Front and side review show seat contact and backrest clearance; Stand returns to ground |

The inherited `ScenePresence.HandleAgentSit` applies its existing sit adjustment, and the default native SIT pose also changes the body relative to the avatar origin. The correction belongs in asset metadata. Rotation remains `[0, 0, -sqrt(0.5), sqrt(0.5)]`, facing the front of the bench. No simulator sit formula or avatar animation was patched.

The current fixture root is `fbc98973-9c8c-f856-689e-56ad00dc920c` at approximately `[176, 119.775, 25.48]`. Confirmed seated avatar origin is `[176, 119.85, 26.13]`; Stand is approximately `[176, 119.20, 25.984]`. The report observes parent/root UUID, grounded/autopilot/seated flags and actual animation IDs. Protocol confirmation alone did not certify alignment. The visible body contact is accepted for the current human test avatar; different body shapes and Lumi need separate checks. The near front approach frame crops the head; the side frame supplies the full-body view. Earlier longer approach evidence includes the inherited walk animation and visible stride.

## Reusable content

Studio's `scripts/bench_seating.py` shares the corrected definition with the Blender generator and produces a new metadata package from retained, hash-checked original payloads. `exports/bench-pbr-v0.1.8/bench` retains the same editable `.blend`, interchange geometry, material library and independently reduced LOD inputs as v0.1.6. Its provenance pins the earlier manifest hash. Native compiled mesh bytes differ only in the inherited serializer's date field; all other bytes and native material payloads match. Budget remains **13 parts, 2 materials, no textures, 1,404 / 754 / 286 / 130 triangles**.

Blender startup was sampled waiting in `NSFileManager.mountedVolumeURLsIncludingResourceValuesForKeys` → `__getattrlist`, before the Python script ran. Its task-specific process was terminated. Diagnostics remain private. No mounted volume was detached and no unrelated build/source was removed. The metadata revision avoids that dependency for an unchanged mesh; it is not a successful new Blender regeneration.

## Timing and evidence

All paths below are under `/Users/wenyan/Finalverse/dev/phase4b`, outside Git. Capture directories contain private `results.xml`, JSON, PNGs and frame CSVs.

| Check | Wall time | Evidence |
|---|---:|---|
| First single view | 47.573 s | `quick-visual-20261010/capture-c5fd4cb0-0bfb-427a-be71-2163c8ffd455` |
| Second single view | 72.438 s | `quick-visual-20261010/capture-8d167e08-b46d-4bbc-9b85-b935328b019a` |
| Settled single view | 10.399 s | `quick-visual-20261010/capture-169c7ebc-0875-428e-a9e6-5be8085837be` |
| Settled repeat | 10.593 s | `quick-visual-20261010/capture-e9a3aa22-174a-4c6e-aedc-f5e00db05147` |
| Original low seat | 33.739 s | `quick-visual-20261010/capture-1bdae465-8a4b-4541-a8fd-2da0d4c8315b` |
| Rejected high seat, close | 20.979 s | `quick-visual-20261010/capture-ab778f30-03cb-4d16-80fb-ac8a0ce2859b` |
| Corrected front Sit/Stand | 21.330 s | `quick-visual-20261010/capture-2ea287cd-63dd-440d-b18c-d204708e8220` |
| Corrected side Sit/Stand | 21.279 s | `quick-visual-20261010/capture-bc873963-daa4-4404-b07f-a4555428e5d4` |

The initial five requests shared PID 52374. Corrected front/side checks shared PID 88694 and used actual **1280 × 738** captures after a valid Day warm-start. Native restoration is true; a returned third-person view and restored FOV were also checked during the initial session. Quick capture does not resize or force an environment. Earlier requests retained their observed 1260 × 718 size. Focus loss, resize or any cached object count change resets sampling; the evidence does not isolate which caused each early delay.

There were **zero GUI clicks per queued camera or bench sequence**. GUI calls remain necessary for occasional startup notices, a restoration spot check and normal logout before catalog reload/restart. The new catalog is intentionally loaded at simulator startup, so these content revisions required reloads. Do not treat those launches as the ordinary ten-second check.

A five-second *normal* warm-start specification was rejected before applying its requested environment. That trial's bench images use the recorded existing region environment and are not Day performance measurements. Launcher checks now prevent that invalid launch; the corrected v0.1.8 warm-start uses native 10/10-second windows and completes. The trial spec and logs remain retained.

The final front sample measured 106.94 FPS seated / 94.99 standing. These are short diagnostic samples of a different framing and content state, not an improvement over the earlier controlled eight-view baseline. No broad benchmark or global graphics-default change was made. The transient profile still has shadow detail 1 at scale 0.5, FXAA, SSAO off and restrained reflection detail.

## Persistence, semantics and tests

- Package/hash/semantic validation and compilation passed for v0.1.8.
- **13/13 native content tests passed**, including sit target XML persistence and refusal to erase later seat edits.
- **5/5 Studio revision tests passed**: payload preservation/provenance, corruption refusal, existing output preservation, version monotonicity and symlink refusal.
- **28/28 viewer Python harness tests passed**, up from 24. The added checks reject short normal windows, invalid camera geometry/FOV, a string in place of the native UUID type, and launch-time interaction.
- The earlier **117/117 CTest entries** and **13/13 changed C++ benchmark cases** remain the unchanged binary baseline. They were not redundantly rerun for a Python/asset-metadata-only change.
- After the final simulator restart, all **13 meshes/LOD assets, 13 semantic components, 13 material face bindings and 2 material assets** were reverified, together with native Sit and occupied-seat Undo refusal. Receipt: `bench-seat-v018-post-restart-20261010.json.verify.json`.
- Original **40 Harbor IDs, positions and scales** passed fresh real HTTP-login/UDP/protocol regression; 100 components were received and normal logout confirmed. Private log: `bench-seat-v018-protocol-post-restart-20261010.log`.
- The active simulation/assets/inventory/appearance services use `MutSea.Data.PGSQL.dll`; PostgreSQL is healthy. The standalone GridService's in-memory region directory is separate from the persistent simulation data store. No database provider was changed in this pass.
- Private SQL/WorldLine rollback snapshots were retained before catalog reload. Earlier trial instances were removed by authorized guarded Undo; no volumes were deleted. Current remote credential scan passes.

## Remaining acceptance

B1/B2 pass for this bench and test avatar. B3 remains open: separate four-path collision proofs and visible native approach exist, but the complete Arrival → tree → bench → pavilion run has not passed. B4 awaits a production pavilion. B5–B12 and B14–B20 remain open, as do final scene/performance acceptance and the historical credential-provenance incident. Existing Lumi relationships, navigation and WorldLine proofs remain preserved; production embodiment has not changed. See the [full B1–B28 table](phase4b-gates-20261010.md).
