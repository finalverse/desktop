# Connected coastal court — native development review, 2026-10-14

World `mutsea.starter.aurens-island@0.3.0` connects the arrival garden, planted stone promenade and pavilion in one reusable bounded native place. Original terrain/water/spawn, shoreline nook and fire circle remain pinned. Prior versions and existing worlds/inventory are preserved. This improves circulation; it is not final premium hero art or market acceptance.

## Combined lighting review

The passive benchmark accepts per-camera local sky/water maps only on the exact named loopback hero fixture. It refuses this extension on the older interaction fixture, malformed maps and quick mode. Native sky/water validation runs before each change and camera switching restarts warmup. Only the private viewer's local environment changes. No region environment, model request, world transaction or public service changes. Sixteen native benchmark cases and ARM64 Release build pass; inherited deployment-target linker warnings remain.

One launch captured Day, Golden Hour and Night from the same camera: position `[103,113,33]`, focus `[128,150,24.8]`, FOV 0.90. Each uses a 20-second warmup and 15-second visible/foreground sample; raw 1280×738 pixels, 128 m draw distance, LOD factor 1.375, SSAO/AA disabled. Pixel, focus, visibility and stable-object gates remain unchanged. Native images show the connected path and intact canopies, planting, pavilion and avatar. Golden Hour is subdued rather than the desired warm hero treatment; Night exposes insufficient long-route wayfinding and interior readability. Sparse terrain, simple materials and faceted trees remain art limitations.

| Local preview | FPS | p95 frame time | Actual shadow setting |
|---|---:|---:|---:|
| Day | 119.26 | 11.398 ms | 0 |
| Golden Hour | 119.93 | 11.414 ms | 0 |
| Night | 119.65 | 11.444 ms | 0 |
| Focused Day shadow check | 119.20 | 11.154 ms | 1 |

The initial requested shadow setting was overwritten by native hardware recommendations on a new profile (`LLViewerWindow` calls `LLFeatureManager::applyRecommendedSettings` when feature/GPU state changes). Metadata caught the difference. Those three samples prove local lighting variants with shadows off. One focused repeat using the initialized profile confirmed actual `RenderShadowDetail=1` and visible sun shadows under canopies, trees and the pavilion. Its 1,788 frames cover 15.0002 seconds. Other recorded graphics values match the Day sample. Evidence is retained rather than silently relabeling the initial capture.

These are single local Apple M1 / 16 GB development samples. Similar FPS does not isolate shadow GPU cost or establish capacity; neither an uncapped GPU-time study nor statistical repeated A/B was performed. A fresh profile followed by warmup does not constitute measured cold-cache acceptance. The previously documented larger-window resize limitation remains open. No renderer/backend migration was made. The private app includes preserved unrelated viewer WIP rather than a clean distribution build.

## Native behavior and account foundation

The avatar-only probe passes ground walking from arrival through the promenade onto the pavilion deck, an offset return, canopy approach, solid post obstruction and native Sit/Stand/logout. No appearance, object edit, model request or broad Undo is sent. Actual position samples remain private.

MutSea validates selected wardrobes before identity creation and completes native home/wardrobe while accounts are inactive. A six-account concurrency smoke verifies native home, 422 linked parts, three semantic world/place contexts, eight owned COF links per account, overlapping connected holds and normal logout. This is backend provisioning evidence, not public signup, rendered crowd performance or complete visual QA for those characters.

Remaining Phase 4B gates: premium landscape/material refinement; warmer Golden Hour and readable Night circulation/interiors; continuous LOD transition/pop and wider collision coverage; public character/experience selection, admission/invitations and a real first-time user study; sustained, cold-cache, crowd/device/multi-platform load; distribution approval. Phase 4B stays open. No GitHub push or public deployment accompanied this slice.
