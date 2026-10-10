# Genesis Lumi native embodiment — validation, 2026-10-10

Lumi (canonical citizen `b151468b-cbcc-4af2-ae2d-6967e8938bb4`) now has an original, reference-faithful skinned body. It renders in the native viewer as a **native animated object (animesh)** driven by the viewer's own avatar skeleton and native library animations. Her identity, memory, goals, home relationship and WorldLine are unchanged. This is a working first production representation, not the finished hero character: textures, Lumi-specific gestures, Sit and final art polish remain open (see the end of this document).

## Why animesh, not an NPC avatar

Source inspection showed both sides already implement animated objects:

- **Server.** `SceneObjectPart.Animations`, `Shape.AnimeshEnabled` (extra-params 0x70) and `ObjectAnimation` packets. PostgreSQL persists the animation set (`sopanims`).
- **Viewer.** `LLControlAvatar` plus joint overrides through `updateAttachmentOverrides`.

Animesh keeps Lumi a single persistent scene entity with her canonical root ID, so the existing deterministic citizen navigation, ownership and backup all keep working. An NPC ScenePresence would have needed a new identity, appearance baking and a physics-driven controller.

## Native contract (verified from viewer source)

| Fact | Source | Consequence |
|---|---|---|
| Runtime pelvis is pinned to `mRoot`; for animesh `mRoot` = object position | `llavatarappearance.cpp` SL-315, `llcontrolavatar.cpp` | Object root = pelvis; `root_height` = pelvis height (0.20 m for Lumi) |
| Pose blending ignores a pelvis position override; other joints honour `alt_inverse_bind_matrix` translations as local overrides (>0.1 mm); `JOINT_COUNT_REQUIRED_FOR_FULLRIG = 1`; ≤110 joints/mesh | `llpose.cpp`, `llvoavatar.cpp`, `lljoint.h` | Short body via local joint overrides, pelvis override (0,0,0), scale lock |
| Weights: ≤4 × (U8 joint, U16 LE), 0xFF terminator, clamped 0.001–0.999, renormalised at skin time | `llvolume.cpp` | Encoder writes normalised U16 influences |
| Matrices row-vector, `[j*4+k]`, translation at 12–14 | `llmodel.cpp` `LLMeshSkinInfo` | Inverse binds `T(-J)`; bind shape `S(domain)·T(pelvis)` |
| Header keys: four LODs, `skin`, `physics_convex`, offsets after header, version ≤ 999 | `llmeshrepository.h` | Header written explicitly |
| All 133 bones have identity rest rotation and unit scale | pinned `avatar_skeleton.xml` | Translation-only rig; checked |

## Pipeline

1. **Author** (Studio): `scripts/build_lumi.py`.
   - Implicit-surface model measured from the pinned `Lumi_logo-00.png`; no pixels are embedded.
   - Surface-nets extraction; explicit-band eye caps for the pupil and iris; a mouth lifted 1.5 mm off the true face surface so it survives every LOD.
   - Deterministic region weights over 30 native joints; four LODs; skinned glTF with native joint names.
   - `native_rig.py` mirrors the viewer's skeleton and keyframe rules. `preview_avatar.py` re-skins the glTF with real native keyframes for QA.
2. **Compile** (MutSea): `tools/MutSea.AvatarCompile`.
   - Validates the pinned skeleton, hierarchy, translation-only joints, inverse binds, weights, LOD counts, material order, license and the non-selectable flag.
   - Emits a skinned `.llmesh`, factor-PBR `.llmaterial` assets (via the existing `ContentMaterialAsset`) and `compiled.json` (`AvatarCompiled`, explicit avatar bounds; small-asset limits untouched).
3. **Verify independently**: `tools/MutSea.Content/verify_avatar_mesh.py` is a Python decoder written from the viewer C++. It re-skins the rest pose with the override chain and compares it with the authored glTF.
4. **Represent** (MutSea `Finalverse/World/CitizenRepresentation.cs`): a pinned operator configuration triggers a journaled, snapshot-first migration on prims-loaded. Recovery is conservative and rollback is explicit (see the World README).

## Package and budgets

| Item | v0.1.0 | v0.1.1 (deployed) |
|---|---|---|
| Triangles LOD0/1/2/3 | 12,020 / 3,956 / 1,328 / 470 | same (byte-identical mesh `d3440fdf…`) |
| Joints / materials / textures | 30 / 7 factor-PBR / 0 bytes | same |
| Native mesh | 245,118 bytes | same |
| Domain (prim scale) | 0.487 × 0.922 × 1.804 m; height 1.10 m | same |
| Change | — | Mint albedo sRGB (180,222,191), lower emissive |

The independent decode reproduces the authored rest pose within **8.4 µm** across all four LODs, with weight error ≤ 4.7×10⁻⁴.

## Evidence (all private, outside Git, under `~/Finalverse/dev/phase4b/`)

- `genesis-20261010/lumi-v0.1.0-r3`, `lumi-v0.1.1` (+ `-compiled`). Drafts r1/r2 were rejected art iterations and were never compiled or deployed.
- `lumi-animesh-20261010/`: pre-migration `pg_dumpall` (`ea5f73fe…`), custom dump (`78428baa…`), journals, runtime SQLite, config before and after, rollback DLL, every probe receipt.
- `quick-visual-lumi-20261010/`: native viewer captures and review sheets. These include the front (`capture-78f7a557…`), side/rear/three-quarter/close sheet, the front-on walk sheet, LOD distance sheet, home front and close-up (v0.1.1), and greeting frames.

## Gates

| Gate | Status | Evidence / limit |
|---|---|---|
| G1 repo state recorded | PASS | HEADs verified; viewer WIP untouched |
| G2 existing work preserved | PASS | Original 40 Harbor objects untouched; backups first |
| G3 builder succeeds | PASS | Deterministic rebuild (identical mesh hash) |
| G4 geometry valid | PASS | Watertight/outward unit test; independent decode |
| G5 four LODs | PASS | Strictly reducing; all materials at every LOD |
| G6 skeleton/weights | PASS | Pinned skeleton, ≤4 normalised influences |
| G7 native compiler | PASS | Baseline + 10 rejection cases (11 tests) |
| G8 mesh loads | PASS | Viewer renders; ViewerAsset delivery hash matches |
| G9 native animations load | PARTIAL | stand/walk visibly applied; `hello` delivered on the wire, not visible on Lumi (below) |
| G10 representation swap | PASS | Journal intent→result; PostgreSQL 7 prims → 1 at ground+0.20 |
| G11 identity preserved | PASS | `CitizenSmoke verify` vs the pre-migration receipt (identity, controller, embodiment ID, home relationship, memories, goals) |
| G12 navigation | PASS | Walks home from (164,144) and (152,138); goal completed; lift-invariance tests |
| G13 state transitions | PARTIAL | idle→turn→walk→idle, talk, greet and face-outward observed server-side; **Sit not implemented** |
| G14 PostgreSQL persistence | PASS | PG + simulator restart: identity and animesh checks, no re-migration |
| G15 restart | PASS | Simulator restart recognises the body, resets to idle |
| G16 rollback | PASS | Exact snapshot restore (same 7 component IDs), identity verify, then re-apply |
| G17 second client | PASS | Probe UDP single root + mesh hash; wire `ObjectAnimation` [stand]→[hello] +277 ms→[stand] |
| G18 viewer renders Lumi | PASS | Front/side/rear/three-quarter/close |
| G19 native animation visuals | PARTIAL | Stand and walk (front-on mid-stride) PASS; greet FAIL (occluded by her own head); talk/turn not visually confirmed |
| G20 LOD transitions | PARTIAL | Silhouette retained at 6/14/25/43 m; popping not reviewed |
| G21 materials | PASS | v0.1.1 mint reads as the reference |
| G22 performance | PARTIAL | One capture with Lumi in view: mean 13.2 ms, p95 24.0 ms, 76 fps. Not a controlled comparison |
| G23 viewer regressions | PASS | 32 Python tests; no C++ change |
| G24 MutSea regressions | PASS | 55 world (4 new), 31 content, 21 validator, OAR suite |
| G25 citizen services | NOT TESTED (unchanged) | Agent-runtime and gateway code unchanged; live behaviour exercised by probes |
| G26 package validates | PARTIAL | `AvatarCatalog` contract; not yet listed in the shared content catalog UI/registry |
| G27 evidence archived | PASS | Private evidence directories above |
| G28 report statuses | PASS | This table |

## Findings and remaining defects

1. **Native human gestures don't read on Lumi's anatomy.**
   - Layered stand+hello renders offline match the viewer frames: her short right arm rises under her own side lobe.
   - The server and wire are correct.
   - Fix: original Lumi keyframe animations (greet flap below the lobes, talk head-bob, listen tilt), which needs package-supplied, validated animation assets. Next increment.
2. **Generic-plan exposure (fixed in r2).** Once Lumi was a single prim, the generic WorldPlan kernel would have treated her as editable, permitting rename, rescale or move. The embodiment is now excluded explicitly.
3. **Pre-existing agent-runtime race (not caused by this work).**
   - After a simulator restart, an observation fetched before `navigate_home` can be applied after the goal is recorded. The runtime then blocks "no matching navigation" (`runtime.rs:333`).
   - It was seen once on a zero-length route.
4. **Runtime goal retention.** The runtime keeps only the 8 most recent goals (`runtime.rs:147`). `CitizenSmoke verify` now encodes that rule instead of failing on retention.
5. **Not done:**
   - Sit for Lumi (the native sit drops the pelvis 0.47 m and needs an object offset);
   - textures (eye detail, cheek blush);
   - a Lumi-specific animation set;
   - controlled performance comparison; LOD popping review;
   - an Elara humanoid proof;
   - shared-registry listing.
