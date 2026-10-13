# Shoreline creation and combined island review — 2026-10-13

This increment proves AI-generated shoreline creation in the isolated Tropical grid while preserving Harbor and its existing user creations. It uses the gateway's current WorldSpec/solver and MutSea's terrain, native policy, PlaceKernel and WorldLine. No renderer replacement, new character roster or public deployment was performed.

## Verified world operation

One actual local-model call proposed two benches and soft lanterns, without paving, planting, roof or centerpiece. MutSea found a clear, dry and level shoreline site at `[193,115,22.086]`, with wet terrain 8 m away. The model supplied no coordinates. Human preview/commit created four asset placements / 34 native parts. The first build survived restart; named Undo restored the prior curated island exactly. Saving/replaying its WorldSpec required no additional inference and recreated the same layout with fresh IDs.

The final scene has 212 groups / 1,472 parts. All 1,438 pre-existing parts retained their identity, ownership and full local transforms. Native mesh/LOD/material/semantic delivery, rotated seat targets, occupied Undo refusal, lights and another clean restart passed. Whole-scene UDP matching and five source texture payload hashes passed. The original fire circle remains installed; no inherited group was retired by shoreline creation.

## One combined native review

The previous capture harness accepted only the Harbor fixture. It now also supports an explicitly identified, passive loopback staging review. The Python launcher matches the exact region UUID to the private registered account, preserves executable hash/private-profile/credential guards, and refuses Harbor identity, other grids, quick-start windows or interaction payloads. The C++ validator limits staging to three canonical cameras, and the runner independently checks the actual grid/region before moving the camera. The existing 10 s warm-up and 10 s sample minima remain intact. Staging does not gain an interaction queue or arbitrary world automation.

The signed macOS ARM64 package completed **one three-view run** at 1,280 × 738 native capture pixels:

| Capture | Review finding |
|---|---|
| `CAM_Arrival` | Imported beach/pool/building scene renders; tiled legacy materials and shoreline geometry still need art direction. Native login displayed a fallback-arrival warning. |
| `CAM_Waterfront` | Both new benches and both lanterns render on dry shoreline, with the water visible and surrounding inherited content intact. Numeric materials remain draft art. |
| `CAM_Garden` | This camera reviews the fire-circle area: original hearth/logs render on the existing floor, clear of preserved seating. The surrounding imported furniture/vegetation remains legacy content. |

All three PNGs, frame CSVs and `results.xml` were saved natively under private `dev/shoreline-create-20261013/native-combined-review/`. This is a scene review, not another Genesis avatar benchmark. The short static samples are diagnostics; they do not pass product performance or cross-platform gates.

The packaged executable SHA-256 is `4cbb5248514f190ec556b826123909c85fb70096231319081749f69c20b9eec6`; deep ad hoc signing verification passed. As with the prior local test package, the build includes preserved unrelated viewer work that is excluded from this increment's source commit.

## Checks and next gate

The Release native visual-validator CTest passes all 14 cases; 21 launcher tests pass, including staging identity/cold-start/passive-camera limits. Ten MutSea composer siting/audit tests and actual native persistence/delivery/Undo checks pass. Both native probes build without warnings/errors. No repeated avatar screenshot loop was performed.

Phase 4B remains open. Next concentrate on the reusable island/world manifest and provisioning boundary, with exact terrain/content/dependency/provenance pins. Resolve the observed home/start-location warning as part of onboarding, then validate account appearance provisioning and one real first-session flow. Final art, collision/LOD interaction, product performance and redistribution gates remain open; do not advertise this imported island as a finished premium world or publish it as a cleared WorldTemplate.
