# Lumi — character direction and production gate

2026-10-09. Art direction is proposed; no production embodiment has passed viewer acceptance. The seven-part linked figure remains the running compatibility fixture.

Lumi is Harbor's welcoming resident and home creator. Her appearance should communicate warmth, intelligence and curiosity before decoration. Preserve her existing citizen identity, controller, home relationships, memories, goals and autonomy history. An embodiment update is a version of that resident, never a new logical citizen.

| Element | Direction |
|---|---|
| Silhouette/proportions | Adult human, approximately 1.70 m; grounded proportions, relaxed upright posture. Recognizable at arrival distance without oversized accessories. |
| Face | Readable eyes, natural expression and soft facial planes. Warm expression; avoid permanently smiling or exaggerated doll proportions. |
| Hair | Pale ash-blonde with warm highlights; shoulder-to-mid-back length, swept away from the eyes. Prefer opaque modeled strands/clumps for the first mobile-conscious version. |
| Palette | Warm ivory, muted gray-blue, dark metal, restrained pale gold. Skin remains readable in Day, Golden Hour and Night. |
| Clothing | Contemporary coastal creator outfit: comfortable tailored jacket/top, gray-blue trousers, practical shoes. Preserve customization and persistent wearables. |
| Accessories | One small original geometric pendant or hair detail; no constant emissive skin or bright ornament that replaces lighting. |
| Idle | Weight shifts, breathing, occasional head/eye attention; no continuous extravagant movement. |
| Greeting/listening/speaking | Brief open-hand greeting, attentive head orientation, restrained hand gestures. Stop gestures when speech stops; do not imply verified phoneme lip sync. |
| Walk/turn/sit | Reuse native avatar motion and simulator navigation/sitting. Feet must ground correctly; hands and clothing must remain free of obvious intersections. |

The supplied Finalverse image is a direction reference. Its creation and redistribution provenance has not been established; it is not a cleared texture, face scan or character model.

## Selected authoring path

Use an explicitly pinned **MakeHuman/MPFB core base** for the first mesh authoring compatibility spike, with original Finalverse hair and clothing where needed. Its core asset license is CC0; the authoring tools have separate GPL/AGPL source licenses. Third-party community assets require their own review. See the [project's license statement](https://static.makehumancommunity.org/about/license.html) and [export guidance](https://static.makehumancommunity.org/mpfb/docs/exporting/index.html), checked 2026-10-09. No external character has been imported or cleared in the repository yet.

| Option | Decision |
|---|---|
| Native classic avatar with original appearance | Compatibility fallback and motion fixture; visual quality still needs work. |
| MPFB core base, original styling, native rig adaptation | Selected authoring spike: editable source and explicit core rights. Native skeleton/skin weights and clothing fit must be proved. |
| Entirely new original human mesh | Retain as fallback if adaptation fails; slower face/topology/rig authoring. |
| Unreviewed downloaded or commercial character | Excluded until exact files, source, license and skeleton compatibility are established. |

MPFB's default/Rigify rig is not evidence of Second Life skeleton compatibility. Do not silently rename bones or ship an untested retarget. Record the pinned source revision, exact asset digests, license, modifications and rig map before importing.

## Runtime boundary

MutSea already exposes `INPCModule.CreateNPC(... agentID ... AvatarAppearance)` and native motion/sit infrastructure. Evaluate this existing path before adding character runtime code. `Finalverse/World/CitizenBridge.cs` currently navigates and speaks through a linked `SceneObjectGroup`; replacing it with a presence requires a reviewed adapter and persistence/rollback checks. The current small-asset compiler deliberately rejects rigged geometry and is not an avatar importer.

An `AvatarTemplate` for this spike needs body/shape, skin, hair, eyes, outfit, attachments, skeleton/animation profile, dependencies, provenance and performance metadata. Keep asset IDs separate from Lumi's canonical citizen ID. Preserve the v1 embodiment snapshot until v2 passes acceptance.

## Acceptance before replacement

1. Cleared source and editable Blender file; native skeleton/rest pose/weights validated with a single rigged attachment first.
2. Day/Golden Hour/Night screenshots of face, silhouette, skin, clothing and hair in Finalverse.
3. Idle, walk, turn, sit, greeting, listening and speaking gesture, observed by a second client.
4. Native collision/feet grounding, attachment bounds, clothing fit and customization/persistence.
5. Simulator and agent-runtime restart retain the same logical identity, memory and home; rollback restores v1 without erasing history.
6. Canonical camera measurements and texture/triangle budgets recorded. Initial authoring targets: body/outfit/hair combined at most 35k high triangles and approximately 20k/8k/3k lower levels; these are budgets to validate, not measured results.

Lumi remains a P0 Phase 4B gate. A spec, a Blender portrait or an NPC API alone does not complete it.
