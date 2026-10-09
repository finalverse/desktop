# Avatar and animation capability audit

2026-10-09; viewer source `de1a355f44`, MutSea `a20dfa7c62`. Complements the tested [appearance compatibility repair](avatar-appearance-audit.md).

| Capability | Source evidence | Development acceptance |
|---|---|---|
| Classic body/shape/skin/eyes/hair | `llavatarappearance`, `llvoavatar`, wearable parameters and baked layers | FirstLook visibly renders; six initial wearables/COF links resolved. Shape/height/hair editing persistence still needs dedicated checks. |
| Clothing and color | `llagentwearables`, inventory asset acknowledgement and legacy bake seam | Pants edit/save/editor close/restart and changed lower bake passed previously; preserve regression. |
| Baked textures | `llfinalverselegacyappearance`, local tex-layer compositor and RGBHM JPEG2000 | Legacy non-system-grid uploads and second protocol-client delivery passed; temporary bake UUID durability is not outfit durability. |
| Alpha layers | Wearable/bake dictionaries and inherited compositing | Code exists; masking a rigged body with this grid and multi-layer clothing are unverified. Legacy first-item-per-type message limits layering. |
| Mesh/rigged attachments | `llmeshrepository`, `llvovolume`, `llvoavatar` rigged bounds/skin info/joint overrides | Code exists; production starter rig and visual joint/attachment acceptance pending. |
| Attachment points | Avatar XML definitions and `llvoavatar` attachment maps | Reuse inherited names/IDs. Template compatibility and persistence pending. |
| Skeleton/facial controls | `character/avatar_skeleton.xml`, joint/visual parameter infrastructure, eye/look-at motions | Existing bones/motions do not prove a cleared hero face rig or lip sync. |
| Idle/walk/turn/sit | `llvoavatar.cpp` registers inherited stand, walk, turn and sit motion classes; `llkeyframemotion` | Human locomotion exists. Lumi’s current linked figure does not execute these skeletal motions. |
| Overrides/blending | Inherited motion controller, attachment joint overrides and simulator animation messages | Audit caller behavior before promising an animation-override product. No new motion system justified. |

## Production path

Retain inherited avatar protocol, skinning, attachment and baked clothing systems. Build a small set of original or explicitly cleared templates with declared body/shape, skin, eyes, hair, outfits, attachment/skeleton dependencies, animation profile, default parameters and provenance. FirstLook is the compatibility fixture; Lumi is the hero acceptance target. Do not market the current seven-part proof embodiment as a production character.

Lumi must preserve citizen ID, home, memory and navigation when her representation changes. Validate silhouette/face and outfit in day/golden-hour/night; idle/walk/turn/sit/look-at/speaking gesture; feet grounding, collision and attachment bounds; appearance persistence and remote observer delivery. Only then add deterministic gesture selection for greeting/listening/speaking. The supplied portrait is art direction with provenance still to be established, not a redistributable character asset.

There is no acceptance of production avatar quality yet. Purchasing assets or redistributing third-party characters is outside this audit; licensing and source files must be resolved before inclusion.
