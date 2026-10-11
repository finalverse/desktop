# Genesis humanoids — worn native avatars, validation, 2026-10-11

The Genesis roster is closed at **nine templates**: Lumi (the nonhuman companion; see [Lumi validation](genesis-lumi-validation.md)) and eight original humanoids:

- **Elara** (`elara-female`): the canonical female human. Elara is exclusively female.
- **Auren** (`auren`): a distinct male human, the Finalverse architect and worldbuilder.
- Kai Female and Male
- Terra Female and Male
- Ignis Female (v0.2.0, her own garnet palette) and Male

Each humanoid is a **worn native avatar**: one rigged mesh attachment skinned to the pinned viewer skeleton, plus an alpha wearable that hides the system body. The wearer's own agent animates it and collides with the world, using the inherited animations, Sit and walking.

All eight were provisioned in turn to a real account. Each walked, sat and stood natively and was seen by a second account. None is selectable yet.

**Template identity.** Studio `manifests/genesis-templates-v0.2.json` lists `package_id@version` with compiled pins. The provisional `finalverse.genesis.elara-male@0.1.0` is **retired** and superseded by `finalverse.genesis.auren@0.1.0`:

- Runtime configurations naming it are refused, with the successor named (`AvatarCatalog.Retired`).
- Nothing is silently aliased.
- Wearers move by an explicit assignment; the wardrobe replaces the outfit and keeps the items.
- Inventory it already created is untouched.

`ignis-female@0.1.0` is a superseded version. It remains a valid pinned package.

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

## Packages (factor PBR, no texture bytes)

| Package | LOD0/1/2/3 triangles | Materials | Pelvis height | Native mesh bytes | `compiled.json` pin | Max rest error |
|---|---|---|---|---|---|---|
| elara-female | 25,139 / 7,546 / 2,531 / 926 | 8 | 0.885 m | 510,147 | `ebcae40c…` | 1.2 × 10⁻⁵ m |
| auren 0.1.0 | 24,233 / 7,275 / 2,441 / 979 | 8 | 1.042 m | 485,317 | `efdf1967…` | 1.5 × 10⁻⁵ m |
| kai-female | 28,060 / 8,421 / 2,820 / 1,042 | 6 | 0.834 m | 652,681 | `26e3ffa5…` | 1.2 × 10⁻⁵ m |
| kai-male | 28,060 / 8,421 / 2,835 / 1,061 | 6 | 0.928 m | 576,960 | `5011c07c…` | 1.4 × 10⁻⁵ m |
| terra-female | 32,463 / 9,734 / 3,264 / 1,317 | 6 | 0.835 m | 763,546 | `1630938e…` | 1.3 × 10⁻⁵ m |
| terra-male | 34,022 / 10,204 / 3,407 / 1,397 | 6 | 1.195 m | 783,759 | `49788909…` | 1.7 × 10⁻⁵ m |
| ignis-female 0.2.0 | 28,442 / 8,528 / 2,840 / 1,057 | 6 | 0.980 m | 693,982 | `57d0b44e…` | 1.4 × 10⁻⁵ m |
| ignis-male | 27,922 / 8,381 / 2,798 / 1,028 | 6 | 1.120 m | 705,428 | `92532aec…` | 1.6 × 10⁻⁵ m |

Retired and superseded entries: elara-male 0.1.0 `e26608ea…` and ignis-female 0.1.0 `c3457ed5…`.

Auren is built from a text brief, recorded as design provenance. His short hair is grown from the body's scalp: an offset with a lifted front, watertight and fitted. MPFB's coarse hair proxy is not used for him. His trim uses straight-edged bands.

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

**Final QA** (2026-10-11; viewer QuickCheck r2 `9ec246dc…` with the invisible-bake fix; module r9 `9ca56004…`):

- **Worn checks, all eight humanoids.** Each was provisioned on FirstLook in turn (one restart per swap, using the replace rule), then:
  - native walk → Sit on the owned bench → Stand, sheets `sheet-<package>-walk-sit-stand.png`, combined in `sheet-genesis-worn-final-r2.png`;
  - seated 80–88 fps, standing p95 15–25 ms (feature checks);
  - a second account received the attachment at point 10 and `IMG_INVISIBLE` on head, upper, lower, eyes and hair: `genesis-gallery-20261011/observer-*-final.json`.
- **All nine together:** `sheet-genesis-nine-final.png`, with the gallery at 87 fps / p95 14.5 ms.
- **LOD distances** 6, 14, 28 and 56 m (`sheet-genesis-lod-distances.png`):
  - silhouettes stay intact, with no holes or lost props;
  - all eight remain identifiable at 56 m;
  - 84–87 fps.

## Elara Male hair defect — diagnosis and fix

Symptom: a grey long-hair shape drew over the worn Genesis head, though the Genesis hair itself was visible at the temples. It appeared in some sessions only, for example Elara Male twice, but not a third time.

| Candidate | Finding |
|---|---|
| Costume geometry | Ruled out. The same mesh rendered short auburn hair as an animesh and in other sessions, and the grey shape lies outside the Genesis mesh. |
| Attachment rendering | Ruled out. In the same frames, face, beard and outfit rendered correctly from the attachment. |
| Baked appearance timing | Contributing only. The grey persisted from login +40 s to +130 s, after the bakes reached the simulator at +12 s. |
| System hair layer | **The visible element.** The viewer kept drawing the system hair mesh. |

**Root cause** is a shared-pipeline defect in the viewer's client-baked appearance path. `llfinalverseUploadLegacyBake` had dropped the inherited `LLViewerTexLayerSetBuffer::doUpload` rule: a layer set fully hidden by an alpha wearable must not be uploaded, and its baked slot becomes `IMG_INVISIBLE`. Instead the empty composite was uploaded as a real bake (observers received an asset ID for the hair slot). The system hair mesh then drew with it. Whether that happened depended on which bake ran and published in a session.

**Fix:** viewer `6adb096935` restores the rule. Verified from a second account for all eight humanoids: every alpha-hidden region now arrives as `IMG_INVISIBLE`. Publishing then settles: eight publishes in the first 5 s and none in the following minutes, with no rebake requests.

The simulator does not recognise `IMG_INVISIBLE` as a cached bake. A core `AvatarFactoryModule` change for this was drafted, then deferred because it needs a core rebuild, and the observed behaviour does not require it.

Related wardrobe fix (MutSea `0a003d70e2`): Current Outfit links now carry the viewer's order code (`@1300` for the alpha), so the viewer no longer rewrites them at every login.

## Gates

| Gate | Status | Evidence / limit |
|---|---|---|
| H1 native skin and rest pose | PASS, all eight | Independent decode < 2 × 10⁻⁵ m; weights within 2 × 10⁻⁵ |
| H2 native render | PASS, all nine | Gallery and Lumi at home (`sheet-genesis-nine-final.png`) |
| H3 worn on an account, alpha hides body | PASS, all eight | Provisioned on FirstLook in turn; no system body or hair after the viewer fix |
| H4 second observer | PASS, all eight | Attachment at point 10; five alpha regions arrive as `IMG_INVISIBLE` |
| H5 rollback / replace | PASS | Explicit rollback (Elara F) and replace-on-assign across all eight; items kept |
| H6 idle | PASS, all eight | Native stand, worn and as animesh |
| H7 walk / turn / Sit / Stand while worn | PASS, all eight | Native autopilot approach, owned-bench Sit and Stand. Skirts, kilts and coat tails part over the thighs when seated, and cloaks pass through the bench back (rigid garments) |
| H8 LOD | PASS for static distances | 6–56 m silhouettes intact; popping during live camera motion not reviewed |
| H9 textures, faces | FAIL — open | Factor PBR only; faces are the untextured base |
| H10 selectable chooser | FAIL — open | Non-selectable by design until onboarding gates. The roster is closed at nine |
| H11 restart persistence | PASS | Appearance and attachment survive simulator restarts (many cycles, modules r6–r9) |
| H12 template identity and retirement | PASS | Versioned registry; elara-male refused with the successor named; existing inventory untouched |

## Findings and known limits

- Four packages first failed to compile with "Normals must be unit length". Opposed faces sharing vertices (flame sheets, pinched tips) had cancelled normals. Normals now fall back to the largest incident face.
- An unquoted `;` in INI list values starts a comment. Only the first gallery entry loaded until the value was quoted, and the World README now says so.
- Art is a first pass:
  - Kai Male reads as a golden mannequin.
  - Ignis F (v0.2.0) now has her own base colour.
  - Terra Female is closest to the reference.
  - Hair, crowns and props are rigid, with no secondary motion.
- After a simulator restart, the test viewer's next login is usually refused once ("already logged in"). Relaunching succeeds; the QA cycle retries automatically.
