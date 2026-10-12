# Phase 4B — visual quality convergence

Plan of record, 2026-10-09. The actual Finalverse viewer is the visual acceptance surface. PostgreSQL, native world streaming, Phase 2 policy/WorldLine, Phase 3 citizens and the Phase 4A facade remain the baseline. Remote PostgreSQL is **16.15**; the separate local fixture is **16.14**. Remote LAN acceptance does not establish public world entry.

## Work boundaries

| Track | Immediate content/configuration work | Viewer support or focused code | Server support | Deferred architecture |
|---|---|---|---|---|
| A — avatars | Small contemporary shape/outfit set; canonical Lumi art direction; explicit asset/provenance dependencies | Audit classic body, mesh skinning, attachments and legacy bake adapter; retain pants-color regression | Complete owned wearables/COF/appearance provisioning; appearance delivery to another account | New avatar/skeleton/motion architecture |
| B — world assets | Bench, lantern and compact blossom tree; modular coastal kit; hero/standard/background budgets | Verify mesh/LOD rendering and selection in the packaged client | Native policy, asset storage, scene insertion, semantics and guarded Undo; bounded place-scale adapter only after the small assets pass | Full live WorldSync and general upload marketplace |
| C — materials | Nine coherent named material definitions and explicit legacy fallbacks | Audit legacy normal/specular and glTF PBR channels; document authoring/export mappings | Audit PBR material asset/override capabilities before publishing such packages | Broad material-system replacement |
| D — environment | Original golden-hour, day and night presentation; deliberate water and atmospheric parameters | Reuse `LLEnvironment`/`LLSettingsVO`, sky/water controls and existing HDR response | Inherited `EnvironmentSettings`/`ExtEnvironment` and native region persistence; authorized operator changes | New atmosphere/GI implementation |
| E — renderer | Opt-in Balanced candidate using measured graphics settings | Existing deferred pipeline, SSAO, shadows, reflection probes, SMAA/FXAA and neutral/ACES tone mapping; change a shader only for an observed isolated defect | None for local graphics choices | Metal/Vulkan replacement, new GI, neural rendering/splats |
| F — animation | Audit inherited idle/walk/turn/sit; use original or cleared compatible clips | Existing skeleton/animation controller and look-at; verify rigged attachment bounds | Avatar/citizen animation distribution and deterministic state transitions | Motion-capture service or new motion system |
| G — performance | Fixed scene/camera/environment, warm-up and sample windows; geometry/texture budgets | Reuse inherited camera pilot, statistics and local instrumentation; benchmark idle foreground rendering | Record object/avatar population and simulator readiness | Universal FPS promises or unmeasured graphics defaults |

## Execution and acceptance

1. Capture the unchanged scene with eight named camera views before additional visual edits. Record executable hash, resolution, graphics/environment settings, population, FPS/frame time and process RSS/CPU where available. Record missing measurements as unavailable, not zero.
2. Finish bench/lantern/tree viewer appearance, native sitting and collision checks. Retain restart and guarded Undo receipts. The tree's Blender preview and 3-part/1084-triangle budget do not establish visual quality or frame-rate improvement.
3. Resolve collision, sitting and proper LODs in the bounded asset seam before pavilion-scale placement. Preserve originals and canonical existing home/garden identities; do not overwrite the entire starter scene.
4. Audit avatar, material and environment support from this source revision. Produce original small material and environment libraries with explicit fallbacks and licensing.
5. Compare an opt-in Balanced profile and controlled golden-hour presentation against the same baseline cameras. Keep user graphics preferences reversible. Measure foreground samples without a competing build.
6. Upgrade hero/standard scene modules only after live acceptance, then run semantic, citizen/home/memory and facade regressions. Capture matched after views.

## Source anchors

- `pipeline.cpp`: deferred rendering, exposure, tone-map selection, shadow/SSAO and antialias passes.
- `llviewershadermgr.cpp` and `app_settings/shaders/class1/deferred/postDeferredTonemap.glsl`: existing final-frame shader paths.
- `llprimitive/llgltfmaterial.cpp`, `llgltfmateriallist.cpp`, `llmaterialeditor.cpp`: PBR representation, fetch/update and authoring surfaces.
- `llenvironment.cpp`, `llsettingsvo.cpp` and shared `llsettingssky/water/daycycle`: environment selection and native settings representation.
- `llagentpilot.cpp`: repeatable local camera recording/playback; benchmark tooling should reuse this before adding an automation interface.
- `llvoavatar.cpp`, `llmeshrepository.cpp`, `llfinalverselegacyappearance.cpp`: inherited body, mesh skinning and the repaired non-system-grid appearance seam.
- MutSea `Finalverse/World/Content*`: bounded operator catalog, native scene policy/execution, semantic mapping and guarded Undo.

The starting viewer is `de1a355f44` with preserved, uncommitted destination-discovery work. MutSea starts at `a20dfa7c62`, Studio at `118b410`. No push is part of this milestone. Release gates remain open until live viewer evidence demonstrates improved presentation, stable interaction and acceptable performance; Phase 4C stays deferred.
