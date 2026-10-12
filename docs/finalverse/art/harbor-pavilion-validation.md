# Harbor Pavilion v2 — first hero place, validation, 2026-10-11

The Harbor Pavilion replaces the v1 sandbox pavilion at (128,192). It is the first **PlaceTemplate**: an original composition of operator-approved small assets, provisioned as one journaled operation with retirement, compensation and rollback.

It is the pattern for the remaining hero places: beacon, arrival, Lumi Home, Garden and waterfront.

## Contract

| Layer | Where | What |
|---|---|---|
| Small assets | Studio `scripts/build_starter.py`, then `validate.py`, then `MutSea.ContentCompile` | Each module stays within the existing 4 m / 32-part / 4,096-triangle contract, with PBR materials (library 0.1.1), reduced LODs and `part_of` semantics |
| PlaceTemplate | Studio `scripts/build_place.py` → `place.json` | Placements pin `package_id@version` plus the manifest SHA-256 read from the approved catalog. Each has an offset and yaw in bounds; the place adds semantics and entrances |
| Provisioning | MutSea `PlaceKernel` / `PlaceProvisioning` | Whole-place check, then snapshot-and-retire of the replaced groups, then all placements created in one journaled operation. Failure compensates; rollback is fingerprint-guarded |
| Offline gate | `MutSea.ContentNativeTests <catalog> --places <root>` | Loads the real catalog with the native material validator, and every place |
| Agent view | `CitizenSmoke semantic` | Each part's semantics plus place context: title, role, entrances |

## Modules (`mutsea.starter.*` 0.2.0, CC0, factor PBR)

| Module | Parts | Triangles (high / medium / low / lowest) |
|---|---|---|
| pavilion-deck | 2 | 216 / 116 / 44 / 20 |
| pavilion-column | 4 | 1,044 / 572 / 228 / 100 |
| pavilion-roof | 2 | 120 / 64 / 24 / 12 |
| pavilion-skylight | 9 | 876 / 470 / 178 / 82 |
| pavilion-fascia | 1 | 108 / 58 / 22 / 10 |
| pavilion-rail | 9 | 412 / 214 / 86 / 34 |
| pavilion-steps | 2 | 216 / 116 / 44 / 20 |
| pavilion-table | 4 | 1,108 / 606 / 240 / 108 |

All eight pass `validate.py`, the content compiler, and 13 native fingerprint tests each against the staged catalog. Three first builds were 4.02–4.04 m wide and were rejected by the 4 m part-scale bound; they were trimmed to 4 m.

## The place (`mutsea.starter.harbor-pavilion` 0.2.0)

- **Size:** 12 × 12 m deck (3 × 3 tiles) at a 0.30 m deck height, within 11 m half extents.
- **Structure:** six columns, eight roof panels around a raised glass skylight, twelve bronze fascia strips, and eight balustrades that leave the south (plaza) and north (lake) steps open.
- **Gathering:** the gathering table sits under the skylight. Four approved benches (0.1.8) on the diagonals face the table, which keeps the north–south walk clear.
- **Surroundings:** four path lanterns (0.1.2, the place's 4 native lights) flank the steps; four coastal trees (0.1.9) stand at the outer corners.
- **Budget:** 59 placements, 27,808 triangles. Place manifest `42f1b682…`.
- **Provisioning config:** `128,192,25.0`, yaw 0.
  - Retires the v1 Floor, Roof, four Columns, three Benches and three Bench Backs.
  - Accepts the North Promenade, which runs under the deck.

## Live evidence (Harbor fixture; private, under `~/Finalverse/dev/phase4b/pavilion-v2-20261011/`)

| Check | Result |
|---|---|
| Backup before change | `before-pavilion-all.sql` (pg_dumpall, 112 MB) |
| Provision | Plan `5ddce1cd…`: 59 placements, 12 retired with XML2 snapshots; journal intent → retired → committed |
| Native render | `sheet-pavilion-v2-native-r1.png` (approach, aerial, interior, skylight), 83–92 fps, p95 15–25 ms |
| Walk / Sit / Stand | FirstLook (wearing Elara) walked from the plaza onto the deck, sat on a rotated estate bench facing the table, and stood (`sheet-pavilion-bench-walk-sit-stand.png`; seat root confirmed). Uses the new explicit `--seat-any-owner` opt-in (QuickCheck r3) |
| Agent semantics | Table, bench and deck each return their semantics with the place context: Harbor Pavilion, role, both entrances (`semantic-pavilion.json`, 3/3) |
| Rollback | Journal rollback_intent → rolled_back. All 12 v1 groups were restored by ID, the floor at its original position, and no v2 parts remained |
| Re-provision | Plan `39ec5d4b…` committed. Native capture after re-provision: `sheet-pavilion-v2-reprovisioned.png` |
| Persistence | 159 pavilion rows in the region database after provisioning; placements are force-backed-up |

## Open items

- **Interior frame time:** 62–66 fps with p95 about 28 ms inside the pavilion (glass skylight, four lights, shadows), against 83–92 fps outside. To be addressed in the final lighting and profile pass.
- **Skylight underside:** the skylight panel shows plaster underneath, while the other roof panels show a timber soffit.
- **North Promenade:** it ends about 1.3 m past the north steps. Shorten it in the arrival/promenade pass.
- **Not yet reviewed:** Golden Hour / Night presets, LOD transitions in motion, and the lake approach from the north.
- **Environment:** the host clock jumped back about 11 hours during this pass. MSBuild then skipped edited sources, so one deployed module (r10) was stale. Every build since uses `--no-incremental`, and a fresh r10b (`7b198598…`) is deployed.
