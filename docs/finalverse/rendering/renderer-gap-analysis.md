# Phase 4B renderer gap analysis

Source audit, 2026-10-09; viewer `de1a355f44` plus preserved destination work. This is an audit of implemented paths, not evidence that every path works on this grid.

| Visible issue / desired capability | Classification | Source finding | Next acceptance |
|---|---|---|---|
| Flat architecture | Configuration + content | `pipeline.cpp` implements directional/spot shadows and SSAO. The isolated profile disables both. | Matched-camera shadows/SSAO comparison with frame times; acne, bias and edge inspection. |
| Highlight rolloff | Configuration | `llviewershadermgr.cpp` and `postDeferredTonemap.glsl` already select Khronos Neutral or ACES. Exposure is bounded in `pipeline.cpp`. | Skin, day, sunset and night comparison; no new tone mapper justified. |
| Jagged edges | Configuration | `RenderFSAAType` selects none/FXAA/SMAA; `RenderFSAASamples` is a **quality index 0–3**, not MSAA sample count. | Use inherited SMAA; inspect geometry and fine foliage at actual raw resolution. |
| Texture clarity | Configuration + content | Boolean `RenderAnisotropic` and separate level control exist. | 4× filtering candidate, memory and oblique surface checks. |
| Materials look like flat paint | Material pipeline + content | Viewer has glTF metallic/roughness, normal, emissive and alpha; initial MutSea ContentCompile emits untextured legacy face colors. | Explicit importer material/texture mapping and native persistence before claiming PBR delivery. |
| Reflections | Lighting + configuration | Inherited probes and water reflection paths exist; probe detail -1 disabled, 0 static, 1 dynamic, 2 realtime. | Start with static probes; inspect water/glass under the three environments. |
| Primitive Lumi | Content + embodiment | Current citizen is a linked proof figure, not a rigged avatar. Viewer already supports rigged meshes. | Original rigged template, skeleton, animation and persistence acceptance. |
| Harbor is sparse | Content | Existing paths, primitive shelter, home and limited small imports. | Modular coastal kit with stable semantic identities; no whole-scene overwrite. |
| LOD cost/popping | Content pipeline | Native compile adapter points all four LODs at the same high-resolution geometry. | Real reduced LOD payloads, transitions and collision verification. |
| Modern native backends/GI | Render architecture | Current deferred OpenGL pipeline remains the compatibility runtime. | Later research; no Phase 4B backend replacement. |

## Tier 1 candidate

An opt-in session profile is evaluated before product UI/default adoption: draw distance 128 m, sun/moon shadows (`RenderShadowDetail=1`), SSAO, SMAA medium (`RenderFSAAType=2`, quality index 1), 4× anisotropy, static probes, ACES and exposure 1.0. These settings are candidates until actual images and foreground sample windows justify them. Never overwrite the user's saved graphics file to run this comparison.

Frame timings are CPU-observed frame intervals, not GPU timestamps. Capture eight native screenshots after a per-view warm-up and sample window. Cached object count is explicitly not an authoritative scene population; RSS/CPU requires a separate process sample. Builds, background throttling, different resolution or different environment invalidate a fair comparison.

## Color path and limitations

The final shader reads linear scene color, applies the selected tone mapper, then converts to sRGB. Optional inherited legacy gamma handling remains. Blender Principled nodes are not a portable shader program: export only declared channels and texture transforms, record color-space conventions and compare in this viewer. Do not add saturation or gamma changes to conceal missing lighting/geometry.

The universal macOS binary builds two architectures; this development Mac verifies ARM64 execution only. Intel deployment floor and prebuilt-library warnings require separate release qualification. No universal FPS guarantee follows from one M1 fixture.
