# Island OAR shortlist — 2026-10-13

The supplied images show Elara and four Genesis families, not an island villa. This search follows the brief’s written tropical/coastal direction; it does not identify an archive from an image.

| Candidate | Catalog size / region | Rights notice from original catalog | Decision |
|---|---|---|---|
| Tropical, Linda Kellie | 10 MB / 1×1 | CC0 | Best candidate for private compatibility inspection; whole-package/member provenance review still precedes Starter Library publication |
| Palm Island | 19 MB / 1×1 | CC BY-NC | Exclude from commercial Starter Library unless separate permission is obtained |
| Lonely Island, J. P. van de Giessen | 3 MB / 1×1 | CC BY-NC-SA 3.0 NL | Exclude from commercial Starter Library unless separate permission is obtained |

The [original Outworldz OAR catalog](https://outworldz.com/cgi/freesculpts.plx?q=OAR-) provides actual download links and per-item hover notices. The [DreamGrid importer documentation](https://outworldz.com/Outworldz_installer/Help/Load%20OAR.htm) explains where item credits/licenses are displayed. This is a three-candidate shortlist, with **one** catalog-licensed commercial-reuse candidate, not three cleared product packages. Readme files could not be retrieved through the web reader; original HTML/hover notices are retained privately. Avoid guessing authors or stronger rights than those notices establish.

## Tropical offline result

Downloaded from the catalog’s actual link; source **10,552,621 bytes**, SHA-256 `6287a48de5e3ca999173b7a957b4e7b06ed294d6b0b7e19d41996ed6c61932f8`. MutSea’s existing bounded `quarantine.py` accepted OAR 0.8: **208 groups / 1,445 parts**, **272 textures / 22 animations / 33 script assets**. The passive derivative removes the 33 executable assets and 88 task inventories. Derivative SHA-256 `88d4f75b4643bb3d06f1f8402f867e48235b55f884c8fcbea1cb576fb76b93c0`.

Evidence: private `dev/phase4b/closeout-20261013/{island-candidates.json,tropical-download.json,tropical-quarantine/report.json}`. Original archive and catalog notices are retained outside Git. No source permissions or attribution were stripped for publication; the derivative is only a staging input. No island was imported into Harbor, no script ran and no public destination was added.

Native visual quality, mesh/material dependency closure, terrain/collision/spawn, restart, two-client travel and performance are **NOT TESTED** for Tropical. It is not yet a premium Finalverse Island or redistributable WorldTemplate. Do one isolated, script-disabled staging import after the entry blocker and World Architect slice; compare its actual source/native content before investing in villa modernization. Reuse the inherited OAR loader, PostgreSQL fixture and guarded teleport stack. Keep Harbor intact. If the archive is visually dated, retain useful cleared terrain/assets and build an original reusable island through the existing WorldTemplate/PlaceTemplate pipeline rather than spending weeks on a legacy scene.

## Isolated Tropical native gate — 2026-10-13

Advanced into a NEW private PostgreSQL16 fixture with separate network/config/data/database volumes and loopback18095; Harbor volumes/accounts/objects and newer user creations are preserved. Script engines/NPC disabled, passive task inventory removed, namespace egress firewall applied before restore and reapplied before client access after restart. Negative external TCP and positive database probes passed. Empty-instance and post-import SQL snapshots retain rollback.

The inherited loader restored208groups/1,445parts, terrain/parcels and294 passive assets. Source-to-runtime reconciliation passed all1,445 owned IDs/full local transforms. After clean native backup/shutdown/restart, the existing read-only OpenMetaverse probe logged in, streamed1,445/1,445 matching parts, fetched five largest referenced JP2 samples with exact payload hashes and logged out normally. Region DisableScripts=true and primitems=0 before/after restart. The generic archive “Starting scripts” log is not execution evidence: engines are disabled and task inventory is absent.

**Texture closure is not accepted:** among185 streamed texture UUIDs, two nonzero references are absent from the source archive and native database: `4bd0fa85-ccd2-4d7e-a42e-c43354ee5776` and `bde4c770-c610-4db6-bc07-b0c683d9be80`. Resolve with rights-cleared source content or an explicit original replacement/alias manifest before Starter Library publication; do not silently fetch inherited service assets or fabricate provenance. The five-sample delivery pass does not close these dependencies.

293/294 retained asset payloads match source hashes. Reserved WHITE_TEXTURE UUID5748decc-f629-461c-9a36-a35a221fe21f keeps a different built-in encoding (32×32 versus archive512×512). Both decoded to every pixel opaque white. Retain the source/native hashes and alias decision rather than force-overwriting a platform asset or claiming all bytes match.

Private evidence: `dev/tropical-stage-20261013/{fixture.json,native-runtime-report.json,database-check.json,white-texture-alias.json,texture-closure.json,protocol-restart.json}`. This is a separate staging grid, not a Harbor teleport destination or published WorldTemplate. No additional native GUI/art benchmark was performed. Final visual quality, collision/spawn review, measured performance, real native shoreline AI placement, redistribution provenance and reusable WorldTemplate acceptance remain open.

## Targeted original curation — 2026-10-13

Read-only per-part/default/override texture bindings traced both missing dependencies to one 14-part `Zoomtada Fire pit` group. Studio now supplies an original reusable `mutsea.starter.fire-circle@0.1.2`, composed by `mutsea.starter.coastal-fire-circle@0.1.0`. It uses seven parts, four numerical PBR materials, independently reduced LODs and one bounded ember light, with no texture or script dependencies. Editable authoring and exact provenance/payload hashes are retained.

The existing PlaceKernel replaced only that group in the separate Tropical fixture, preserving its complete hashed XML snapshot and recording the transaction in WorldLine. A native rollback restored all 14 original IDs and removed all seven new IDs. The final re-provisioned scene survived clean restart: **208 groups / 1,438 parts**, including **1,431 unchanged inherited parts** whose full local transforms, ownership, links and identities were verified. Native delivery verified all seven meshes, four materials, seven semantic components and the light. All **182 active declared texture references** resolve to nonempty native assets; five source texture payload hashes were checked. Scripts remain disabled and task inventory is empty.

The original archive still has its missing dependencies; only this explicitly curated instance passes the declared texture-presence gate. No whole-archive redistribution clearance, premium island visual/performance acceptance or Harbor teleport destination is implied. Next: one bounded native scene/arrival review, grounded shoreline creation through the existing WorldSpec/PlaceKernel path, and a reusable world manifest with exact terrain, content, dependency and provenance pins.

Private receipts: `dev/tropical-stage-20261013/{fire-circle-final-worldline.jsonl,fire-circle-final-content-receipt.json.verify.json,curated-final-protocol.json,curated-final-database-check.json,curated-final-texture-closure.json,rollback-verification.json}`. The single-asset verification receipt explicitly identifies an operator PlaceKernel transaction, not a client ContentKernel import. No repeated Genesis screenshot benchmark was performed.
