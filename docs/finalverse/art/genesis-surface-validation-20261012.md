# Genesis surfaces and live region convergence — 2026-10-12

**Phase 4B remains OPEN.** The latest user instruction supersedes the historical closure/carry-forward decision. This increment establishes bounded avatar surface delivery and checks existing region travel. It does not accept every Genesis design or release gate.

## Starting source state

| Repository | Branch | Starting commit |
|---|---|---|
| Viewer | `codex/phase4b-destinations-import` | `06a873c793` |
| MutSea | `codex/phase4b-destinations-import` | `7496b738c7` |
| Studio | `main` | `228e4eb` |

The existing viewer destination/network/UI WIP was retained without staging or resetting it. AI gateway `0a720d9` and Composer M1/M2 were preserved. This pass adds no character, changes no renderer and touches no public deployment.

## Region travel, verified live

Use the local Harbor grid, open **World Map**, search the exact region name and select **Teleport**. The three configured regions share the same grid; no separate login is needed.

| Region | Native parts streamed | Outbound / return | UUID |
|---|---|---|---|
| OpenVCE Plaza | 4,090 | 0.324 / 0.279 s | `fc6342cb-6166-4d4e-b04a-b50701f398e8` |
| Harbor Template | 669 | 0.290 / 0.265 s | `66fe6ea8-f4f3-4170-a5ff-c7391997faca` |
| MutSea Harbor | Return destination; Lumi visible | Both returns pass | `a9d44538-26ca-4586-a8a9-e551722ffa15` |

Receipts assert both the region name and UUID, the expected stream and Lumi on return. The probe's read-only teleport/appearance modes now work with AI services offline and wait for normal logout. OpenVCE is the passive, script-free derivative of the restored OAR; no other downloaded OAR is configured as a region. Harbor Template is the reusable original world instance. OpenVCE's whole-archive redistribution rights remain unresolved. The original isolated OAR fixture disabled both script engines; the current multiregion Harbor simulator has its native engine enabled for Harbor, so do not claim a globally disabled engine here. OpenVCE task scripts/inventory were removed and its region scripts disabled.

## Hair/alpha diagnostic and character identity

The earlier shared viewer fix `6adb096935` restored the inherited hidden-layer bake rule: a fully hidden layer publishes `IMG_INVISIBLE`, instead of uploading an empty image and rendering system hair over the mesh. Costume geometry and attachment rendering were ruled out; bake timing contributed but was not the root defect. This pass's second observer reverified the five hidden head/upper/lower/eyes/hair slots, before and after restart. The probe now waits for the final appearance rather than accepting an early default texture packet.

Elara stays the canonical female. The retired provisional `elara-male@0.1.0` still refuses provisioning with Auren named as successor, with no silent alias. An operator explicitly changes an assignment; snapshot-backed replacement restores the previous appearance and retains old inventory folders/items. Female Ignis remains `0.2.0`, with garnet/gold/rose rather than the male obsidian base. Studio registry v0.3 retains exactly nine non-selectable templates and all historical compiled pins.

## Implemented surface contract

- Compiled AvatarPackage schema 3 adds optional face `base_color_texture` and `textures[]`; schemas 1/2 remain byte-identical when surfaces are absent.
- Only original, hash-pinned, offline-authored RGB8 native JPEG2000 codestreams: one tile, zero offsets, no subsampling, at most 1024×1024 / 1 MiB each, 4 MiB total, 1–8 surfaces.
- The interchange PNG is package-local, pinned, matching bounded RGB8 dimensions. glTF images/texture indices/UV0 bindings must close exactly. External URLs and extra images are rejected.
- Runtime stores native texture assets with content-derived UUIDs and binds inherited PBR base-colour images plus native TextureEntry fallback. Static small-content packages remain factor-only.
- The codec header/hash check is for trusted offline authored content; it is not a public arbitrary-image security validator. Studio independently decodes and verifies pixel-exact PNG/JPEG2000 equality. No normal/ORM maps are implemented.

Auren **0.2.0** revises his original 0.1.0 body with woven slate cloth, brushed bronze and soft leather: three 256×256 original neutral periodic RGB tiles, **111,201 native bytes** total. Existing colour factors retain his chestnut/slate-blue/bronze identity. Source provenance appends the original surface recipe and source-manifest pin. glTF BIN, geometry, skin, native LLMesh and alpha bytes remain identical.

| Fact | Verified value |
|---|---|
| Compiled pin | `e0604d12690704db1a677decb525791632052bcc05d28d51a810c98f635212a4` |
| Mesh pin | `cfaa19b8c74011bc9dd05e4142647c0c90e4df0b20c1959a948de4555a580891` |
| Mesh bytes / joints | 485,317 / 60 |
| LOD triangles | 24,233 / 7,275 / 2,441 / 979 |
| Independent decoder max rest error | 1.51 × 10⁻⁵ m |
| Texture UUIDs | `ac392c1d-c36e-bdb9-8234-d82e5f80ca06`, `b673d09d-12f9-8c54-fa0e-a172dfdd134c`, `7d31dd61-a7c0-0c90-617d-1bbca162fea6` |

## Native evidence and limits

Private evidence root: `~/Finalverse/dev/phase4b/convergence-20261012/`; `evidence-index.json` identifies captures explicitly (host clock changes make mtime-based selection unreliable).

- `observer-auren-surfaces-final.json`: another logged-in account receives the attachment at Pelvis (10), all five invisible bakes, all three effective face texture IDs and exact native GetTexture payload hashes.
- `observer-auren-after-restart.json`: the same mesh, alpha and three downloaded hashes after normal simulator shutdown/start on existing PostgreSQL storage. No container/data reset.
- Quick capture `95bd3600-82ae-41e4-9915-8bb52a7a7580`: grounded native walk/approach, exact bench Sit and grounded Stand, **13.767 s**, with front/approach images reviewed. `aa51ee63-2850-48f6-a5f2-e48b605fbce5` is the close-up.
- A long initial bench route timed out after 180 s while the viewer was throttled in the background; its trace contains only 27 samples. Retained as a failure, not a pass. Native World Map positioned near the bench before the successful bounded sequence. No setting was changed to suppress throttling.
- The first warm launch failed on stale presence and the warm capture requested the wrong window size. A normal quit/relaunch removed the stale login; the later warm spec omitted the size assertion. Single captures were valid. A fixed bench camera after restart showed no avatar because login used the home location; it is retained as framing evidence, not avatar proof.
- Actual post-restart avatar capture `1c6f6742-945c-48f4-8b88-a80066b1f1ed` proves surfaces still render, but exposes **compressed/crossed legs immediately after login**, with `STAND_3` (`42b46214…`) active alongside base stand. After restoration, Elara 0.1.0 also shows compressed legs with only base stand (`2408fe9e…`); this broadens the issue beyond one idle variant. The geometry/skin bytes are unchanged. Reproduce on prior factor-only bodies and compare initial rig/animation state with the valid post-Sit/Stand state before accepting full animation compatibility. A 1.5 s native walk on restored Elara then produced extended legs in capture `559c741b-28ac-4866-9f2a-6fb0f725ee10` (an idle upper-body lean remained). This supports a fresh-login initialization/state issue; its precise cause is not yet established. Do not hide it by disabling native idle motions.

The earlier all-nine gallery, eight worn walk/Sit/Stand sheets and static LOD distance checks remain evidence for native compatibility. They do not cover continuous LOD transitions or all inherited idle variants. Face/skin/hair artwork and final surface art for all nine remain open; simple cloth tiles do not complete H9.

## Checks

- Avatar compiler: **28** cases pass, including prior native motions/worn contracts, tampered texture hashes/headers/precision/dimensions/budgets, bounded RGB8 PNG, unresolved bindings, URLs and extra images.
- Studio surface/registry: **6** cases pass; original Genesis build table/retirement/registry: **8** pass.
- Existing content kernel: **45** pass; world/navigation: **56** pass; viewer Python harness: **33** pass (earlier in this pass).
- Avatar compiler/shared module build: zero warnings/errors. CitizenSmoke builds with one preexisting nullable warning in Composer's `known.Value`; no errors.
- Fresh factor-only Auren compiled JSON matches the retained original pin exactly. Independent native LLMesh decoder passes for the surface revision.
- No new viewer C++ was changed or rebuilt; native validation uses QuickCheck r3 SHA `7a8fadb695da93f8a9165a2e8a4f2ffbb5595831d0d70746f92829a206f8806a`.

## Rollback and next gates

Before mutation: private full PostgreSQL dump `before-avatar-surfaces-all.sql` (143,979,144 bytes; SHA `60c3945e…`), `world-journals-before.tar`, original runtime config and module `f01755dd…` are retained. The new module is `544df793…`. Existing container TTY/stdin and both networks remain intact. Only FirstLook's Genesis assignment was changed for QA; restoring the original config uses the existing explicit wardrobe replacement, preserving all inventory. `observer-elara-restored-r2.json` verifies the original Elara mesh and five invisible bakes; `runtime-restored.ini` is byte-identical to the original config (SHA `36cff595…`). The private evidence index records both. The initial restoration probe used a nonexistent package path and failed before inspection; its corrected retry passed.

Next order: resolve fresh-login rig/idle compression on the shared humanoid pipeline; finish reusable texture/face art across the existing nine; complete continuous LOD and controlled performance checks; polish Harbor hero content through the same approved Place/WorldTemplate contracts; complete self-serve appearance and human newcomer/visitor-Lumi validation before acquisition work. Keep AI Place Composer M1/M2 as a preserved local R&D milestone. No push or public deployment.


## Local milestone commits

- MutSea `d83773771e`: bounded original native avatar surfaces and stricter read-only appearance/travel probes.
- Studio `63e7ae5`: Auren 0.2.0 source reviser, registry v0.3 and preservation tests.
- Viewer: this documentation commit only; existing destination/network/UI work remains uncommitted.

Nothing was pushed or deployed publicly. The viewer remains on the original Elara assignment for local inspection; the simulator is running with the validated shared module and original configuration.
