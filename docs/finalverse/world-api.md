# World API mapping and first action contract

Current status: authenticated MutSea inspection, plans, native execution and WorldLine are implemented development slices. See [Phase 2](phase2-ai-world-creation.md), [experience discovery](phase4b/experience-discovery.md) and the world documentation for implemented contracts and acceptance. The operation map and proposed envelope below preserve the historical Phase 0 audit; they are not a claim that every conceptual operation is implemented.

| Conceptual operation | Existing implementation evidence | Adapter work / limit |
|---|---|---|
| `World.inspect/query`, `Scene.describe` | `LLAgentListener::getPosition/getNearbyObjectsList/getNearbyAvatarsList`; `LLWorld::getRegionList`; `gObjectList` | Copy a bounded structured snapshot; partial world, not full simulator database |
| `Entity.get` | `LLViewerObjectList::findObject(LLUUID)`; `LLViewerObject` transforms, `permMove/permModify` | Require namespace + UUID, check live region/dead state; property fetch may be asynchronous |
| `Entity.create` | `LLToolPlacer::addObject` constructs `ObjectAdd` | Existing entry point takes screen x/y and raycasts; extract/reuse lower protocol construction with explicit position later; do not generate mouse clicks |
| `Entity.delete` | `LLSelectMgr::selectDelete/confirmDelete`, `DeRezObject` to trash | Destructive, ownership and no-copy complications; exclude from MVP |
| `Entity.clone` | `LLSelectMgr::selectDuplicate/selectDuplicateOnRay` | Copy permissions, linksets, asynchronous new UUID; later action |
| `Entity.move/rotate/scale` | `LLPanelObject`, `LLSelectMgr::selectionMove/sendMultipleUpdate`, `MultipleObjectUpdate` | Reuse validation/edit path; trusted executor controls explicit root target and preserves UI selection |
| `Entity.set_material` | `selectionSetGLTFMaterial`, `selectionSetMaterialParams`, `LLGLTFMaterialList` | Distinguish physics material from per-face render/PBR material; permission/fetch validation |
| `Entity.set_property` | `selectionSetObjectName/Description/PhysicsType/...` | Per-property allowlist; ownership/security properties never generic model writes |
| `Avatar.navigate` | `LLAgent::startAutoPilotGlobal/stopAutoPilot`; `LLAgentListener::startAutoPilot` | Own avatar first, completion on `LLAutopilot`; not arbitrary NPC control |
| `Avatar.look_at` | `LLAgentListener::resetAxes`, HUD look-at effects | Distinguish facing from camera focus |
| `Avatar.speak` | `LLFloaterIMNearbyChat::sendChatFromViewer` | Existing text chat; speech synthesis/voice injection not implemented |
| `Avatar.animate` | `LLAgent::sendAnimationRequest`; listener `playAnimation/stopAnimation` | Animation inventory permissions; local preview versus broadcast explicit |
| `Avatar.interact` | listener `requestTouch/requestSit/requestStand`; `LLToolGrab` | Script-triggering interaction requires specific capability, not unrestricted tool execution |
| `Scene.query_spatial` | Region/object positions, volume octrees, `LLViewerWindow` picking | Bounded spatial query over loaded entities; semantic relations are new sidecar data |
| `Scene.set_environment` | `LLEnvironment` local settings and parcel/region permission methods | Local preview is distinct from persistent shared environment update |
| `Inventory.search` | `LLInventoryModel::collectDescendentsIf`, inventory observers, background fetch | Cache may be incomplete; avoid full inventory disclosure to a model |
| `Inventory.instantiate` | `LLToolDragAndDrop` rez path and inventory asset operations | Extract explicit world intent; permissions, no-copy items and asynchronous rez remain |
| `Camera.move/focus` | `LLAgentCamera::setFocusGlobal/setCameraPosAndFocusGlobal` | Local reversible action; no persistent world write |
| `Simulation.run` | Physics flags/cost retrieval and mesh decomposition interfaces | No authoritative world simulation API in this client; server-side feature required |
| `WorldTransaction.*` | No general atomic world transaction in audited viewer | Add logical action lifecycle and compensations; do not claim ACID or universal rollback |

Sources: [agent listener](../../indra/newview/llagentlistener.cpp), [selection](../../indra/newview/llselectmgr.h), [selection implementation](../../indra/newview/llselectmgr.cpp), [placer](../../indra/newview/lltoolplacer.cpp), [object editor](../../indra/newview/llpanelobject.cpp), [camera](../../indra/newview/llagentcamera.h), [inventory model](../../indra/newview/llinventorymodel.h), [rez](../../indra/newview/lltooldraganddrop.cpp), [environment](../../indra/newview/llenvironment.h).

## Existing structured inspection

`getNearbyObjectsList` already replies with `id`, `global_pos`, `region_pos`, `region_id`, using a distance clamped to 1–512 m and squared distance comparisons. It iterates loaded volume objects and excludes attachments. It does not provide semantic names, ownership, permissions, confidence, pagination or full-world coverage. Use its behavior as the source seam; add bounds, lifecycle checks and an explicit snapshot projection rather than building a second scene runtime.

Names, descriptions and permission data are on `LLSelectNode`, with `mValid` and asynchronous property requests. Do not invent missing names or request every object's properties every frame. `LLViewerObject` also has render, asset and region dependencies, so never serialize its raw memory/pointers.

## Proposed first action envelope

```json
{
  "schema_version": 1,
  "action_id": "client-generated-uuid",
  "operation": "entity.move",
  "target": {"world_id": "configured-grid-identity", "region_id": "uuid", "entity_id": "uuid"},
  "parameters": {"position_region_m": [128.0, 128.0, 30.0]},
  "precondition": {"snapshot_revision": 17, "position_region_m": [127.0, 128.0, 30.0]},
  "capability_id": "trusted-session-grant-id"
}
```

The authenticated principal, issuance/expiry and capability scope are bound by trusted session state; the model cannot establish them by sending JSON. Validate schema, finite numeric bounds, region, linkset root, ownership/movability, no attachment/avatar/physical/permanent object, session continuity and current observed state. The first capability is limited to a user-selected owned object and a small movement bound. Revalidate on the main thread immediately before sending.

## Honest transaction semantics

Lifecycle: `proposed -> validated -> authorized -> dispatched -> observed_applied | observed_rejected | timed_out_unknown`; locally reject invalid actions before dispatch and record the reason. Reserve/deduplicate action IDs locally and across the bridge. Legacy packets lack a Finalverse server idempotency key and a transactional compare-and-swap version. A client revision is a stale-state check, not distributed concurrency control.

Correlate server observations to target, expected transform and bounded time window. Record that matching state is observation evidence, not proof of causal exclusivity. Lost confirmation is an unknown outcome; do not retry destructive operations automatically. Region change or disconnect cancels pending execution and expires grants. One-object MVP avoids claims of multi-object atomicity.

Capture the before-transform and provenance before dispatch. Undo validates the same permissions and that the current state still matches the applied transform, then sends a compensating move. If another actor changed the object, refuse automatic undo and surface the conflict. Local preview rollback before dispatch is distinct from server compensation after dispatch.
