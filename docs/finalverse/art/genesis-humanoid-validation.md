# Genesis humanoids — worn native avatars, validation, 2026-10-11

The Genesis family has eight original humanoid packages:

- Elara Female and Male
- Kai Female and Male
- Terra Female and Male
- Ignis Female and Male

Each is a **worn native avatar**: one rigged mesh attachment skinned to the pinned viewer skeleton, plus an alpha wearable that hides the system body. The wearer's own agent animates it and collides with the world, using the inherited animations, Sit and walking.

Elara Female was provisioned to a real account and worn natively. All eight forms render natively on Harbor. None is selectable yet.

## Native contract (worn)

| Fact | Consequence |
|---|---|
| Rigged attachments follow the wearer's skeleton; joint position overrides come from `alt_inverse_bind_matrix` (pelvis excluded) | Bodies of any height (1.8–2.4 m) keep the native skeleton; no custom joints |
| Alpha wearable `IMG_INVISIBLE` on head 23 / upper 22 / lower 21 / eyes 24 / hair 25 | The system body and hair are hidden; the mesh is the only visible body |
| Appearance: Alpha slot `AvatarWearable.ALPHA` (13); attachment appended on Pelvis (10, `point\|0x80`) | Worn exactly as a resident's own outfit |
| Viewers trust their inventory cache by folder version, and adding items does not bump versions here | The wardrobe bumps the Current Outfit, Clothing and Genesis folder versions. The first attempt, without the bump, left the alpha unworn (6 items at cached version 7); with the bump the viewer wore 7 wearables |
| Worn bodies carry no motions or collision of their own; high-LOD budget 40,000 triangles (animesh keeps 20,000) | Enforced by `AvatarCatalog.Validate` |

## Pipeline

1. **Body** (Studio `scripts/genesis_human.py`), built on the pinned CC0 MPFB2 base: v2.0.17, commit `80919fa4…`, `base.obj` SHA-256 `8e761e66…`.
   - Macro targets (gender, age, muscle, weight, proportions, height, race) are applied with MPFB's own piecewise factors.
   - Re-posed from A to T by linear blend skinning. The pelvis is centred and the feet sit at z = 0.
   - MPFB joints and weights are mapped to the native joints, including the Bento fingers; 60 joints are used.
2. **Costume** (`scripts/build_genesis.py`). Original geometry only:
   - garments cut from MPFB's fitting helpers (tights, skirt, hair, eyes, lashes);
   - rigid props on native joints (spheres, tori, boxes, cones, flame sheets);
   - recolour and displacement from procedural noise.
3. **LODs.** Blender (authoring only) decimates per material to 30 % / 10 % / 3.5 %. Weights are re-transferred from LOD0 by nearest same-material vertex.
4. **Compile and verify.** The steps are the same as for Lumi: `MutSea.AvatarCompile` (alpha wearable generated through `AssetClothing`), then the independent decoder `verify_avatar_mesh.py`.
5. **Provision** (MutSea `GenesisWardrobe.cs`). This is an operator action while the wearer is offline, journaled with an appearance snapshot. It writes content-derived assets, a `Genesis · <title> <version>` folder, Current Outfit links, the alpha slot and the attachment. Rollback restores the appearance.

## Packages (v0.1.0, factor PBR, no texture bytes)

| Package | LOD0/1/2/3 triangles | Materials | Pelvis height | Native mesh bytes | `compiled.json` pin | Max rest error |
|---|---|---|---|---|---|---|
| elara-female | 25,139 / 7,546 / 2,531 / 926 | 8 | 0.885 m | 510,147 | `ebcae40c…` | 1.2 × 10⁻⁵ m |
| elara-male | 26,169 / 7,856 / 2,632 / 961 | 8 | 1.030 m | 532,331 | `e26608ea…` | 1.5 × 10⁻⁵ m |
| kai-female | 28,060 / 8,421 / 2,820 / 1,042 | 6 | 0.834 m | 652,681 | `26e3ffa5…` | 1.2 × 10⁻⁵ m |
| kai-male | 28,060 / 8,421 / 2,835 / 1,061 | 6 | 0.928 m | 576,960 | `5011c07c…` | 1.4 × 10⁻⁵ m |
| terra-female | 32,463 / 9,734 / 3,264 / 1,317 | 6 | 0.835 m | 763,546 | `1630938e…` | 1.3 × 10⁻⁵ m |
| terra-male | 34,022 / 10,204 / 3,407 / 1,397 | 6 | 1.195 m | 783,759 | `49788909…` | 1.7 × 10⁻⁵ m |
| ignis-female | 28,442 / 8,528 / 2,840 / 1,057 | 6 | 0.980 m | 693,982 | `c3457ed5…` | 1.4 × 10⁻⁵ m |
| ignis-male | 27,922 / 8,381 / 2,798 / 1,028 | 6 | 1.120 m | 705,428 | `92532aec…` | 1.6 × 10⁻⁵ m |

Every package has 60 joints and the alpha regions head, upper, lower, eyes and hair. LOD triangle counts match exactly after decode. Elara Female rebuilds byte-identically from the current script, including the Blender LODs.

## Native evidence (private, under `~/Finalverse/dev/phase4b/`)

**Elara Female worn on FirstLook** (module r6, evidence in `genesis-wardrobe-20261011/`):

- The native viewer showed the Genesis body with the system body hidden: `sheet-elara-worn-v010-alpha.png`.
- Performance: 126 fps, p95 10.5 ms (feature check).
- A second account saw the wearer's attachment carrying the Genesis mesh `d99719d0…` at point 10 (`observer-wearer.json`).
- Rollback restored the prior appearance. Re-provisioning then succeeded.

**Family gallery** (module r7):

- All eight packages were shown as standing animesh mannequins on the Harbor plaza: `sheet-genesis-gallery-v010.png` (captures `ff50de50…`, `f553905a…`, `01258fc3…`).
- Performance: 74–81 fps, p95 17–27 ms with nine skinned Genesis bodies in view (feature check).
- The gallery is never attached to backup. The region database showed 0 gallery rows after backup cycles.

## Gates

| Gate | Status | Evidence / limit |
|---|---|---|
| H1 native skin and rest pose | PASS, all eight | Independent decode < 2 × 10⁻⁵ m; weights within 2 × 10⁻⁵ |
| H2 native render | PASS, all eight | Native viewer captures (gallery); Elara F also worn on an account |
| H3 worn on an account, alpha hides body | PASS Elara F; NOT TESTED for the other seven | Same compiler and wardrobe path; each needs its own provisioning check |
| H4 second observer | PASS Elara F | Attachment and mesh seen from a second account |
| H5 rollback | PASS Elara F | Appearance restored; items kept |
| H6 idle (native stand) | PASS, all eight as animesh | Same skin; the worn idle is the wearer's own |
| H7 walk / turn / Sit while worn | NOT TESTED | Driven by the wearer's native animations; capture pending |
| H8 LOD transitions | NOT TESTED | Four LODs present; popping not reviewed |
| H9 textures, faces | FAIL — open | Factor PBR only; faces are the untextured base |
| H10 selectable chooser | FAIL — open | Intentionally non-selectable until H3/H7/H8 pass per character |
| H11 restart persistence | PASS Elara F | Appearance and attachment survive simulator restarts (module r6 → r7) |

## Findings and known limits

- Four packages first failed to compile with "Normals must be unit length". Opposed faces sharing vertices (flame sheets, pinched tips) had cancelled normals. Normals now fall back to the largest incident face.
- An unquoted `;` in INI list values starts a comment. Only the first gallery entry loaded until the value was quoted, and the World README now says so.
- Art is a first pass:
  - Kai Male reads as a golden mannequin.
  - Ignis F and M differ mostly by accessories.
  - Terra Female is closest to the reference.
  - Hair, crowns and props are rigid, with no secondary motion.
- After a simulator restart, the logged-in test viewer reconnected and was refused once ("already logged in"). The first relaunch was then refused as "authentication failed"; a second relaunch succeeded.
