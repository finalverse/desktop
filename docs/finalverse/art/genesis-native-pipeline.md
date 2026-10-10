# Genesis native compatibility audit — 2026-10-10

**Audit complete; production embodiment incomplete.** Six supplied PNG reference hashes and nine intended forms are recorded by Studio's `scripts/audit_genesis.py` in `manifests/genesis-reference-audit-v0.1.json`. The manifest explicitly marks every form non-selectable and records zero production avatars. It is not an AvatarPackage or a runtime registry.

Lumi is the mint-green nonhuman companion, turquoise eyes and rounded triangular head. Elara Female is the auburn-haired woodland explorer with green tunic and blue-green cloak. Other references establish Kai's synthetic and Terra/Ignis elemental families. Missing complementary forms need consistent original designs. The earlier MPFB humanoid is a separate incompatibility study, not this Lumi design or a source of cleared production characters.

## Evidence in existing source

| Concern | Existing implementation and implication |
|---|---|
| Skeleton | `indra/newview/character/avatar_skeleton.xml`: 133 bones, 26 base / 107 extended, 26 collision volumes; hierarchy includes extended spine parents. Exact SHA-256 `adf352f0f4a65f19cbfd4665fc71c0450bc0a23a489ac6eb52987e343f221e19` |
| Axes | Native eye/foot offsets face **+X**, left limbs extend **+Y**, Z is up. Account for the authoring coordinate conversion explicitly; do not assume a front-facing -Y Blender rig is already native |
| Skin blocks | `indra/llprimitive/llmodel.cpp`, `LLMeshSkinInfo`: joint names, matching inverse bind matrices, bind-shape matrix, optional alternate binds/pelvis offset/joint-scale lock. Matrix array index is `j*4+k`. Joint/bind count mismatch drops bindings |
| Weights | `LLModel::writeModel`: up to four influences, one-byte joint index (<255), 16-bit influence, `0xff` sentinel for fewer than four. Same position/normal/material and quantization domains across LODs must be verified |
| Authoring import | `lldaeloader.cpp` parses Collada controllers, bind shapes, joints and weights. This is the mature viewer upload path to wrap. No successful Genesis upload/import is claimed |
| Animation | `llvoavatar.cpp` retains native motion registration. `llfloaterbvhpreview.cpp` accepts BVH and serializes a native keyframe motion; `llkeyframemotion.cpp` owns native animation encoding. Preserve these systems and prove motions on the actual new body |
| Appearance | Existing native avatar appearance, wearable baking, attachment/inventory and persistence remain available. A rigged mesh requires validated attachment/appearance provisioning and base-body hiding where appropriate; an avatar-shaped world object does not automatically gain those behaviors |
| Current Lumi | MutSea `Finalverse/World/CitizenBridge.cs`: `Body()` returns a **SceneObjectGroup**; `EnsureCitizen()` constructs the seven linked primitives. Canonical root ID `b151468b-cbcc-4af2-ae2d-6967e8938bb4`; movement uses deterministic object transforms in `CitizenFrame()` |
| Native NPC | Inherited NPC/ScenePresence facilities are available but not used by this seven-part bridge. Moving to a native avatar requires an explicit representation adapter and collision/navigation/identity recovery decisions |
| Content compiler | `tools/MutSea.ContentCompile/Program.cs` supports untextured, unrigged geometry and bounded small-asset catalog envelopes; its serializer reconstruction does not carry a proven avatar skin block. Do not lift its 32-part/4,096-triangle limits to hide an incompatible path |

The audit pins names/hierarchy/source transforms. It does **not** prove final rest-pose matrices, skeleton shape overrides, normalized weights, native skin decoding, nonhuman collision height, rigged LOD transitions or inherited animations on Lumi. Simulator collision does not follow visible skin geometry automatically.

## Next production boundary

1. Prove one original nonhuman Lumi model through native skin/attachment and animation delivery, using the pinned reference. Budget actual mesh/texture/material/animation/LOD bytes. Do not substitute a static mesh for an animated avatar.
2. Separate logical citizen identity from representation, adapting the existing bridge while preserving memory, goals, home, garden, relationships and WorldLine. Keep a recovery snapshot and explicit old-component mapping; never create another logical Lumi to sidestep migration.
3. Validate idle/walk/turn/talk/listen/greeting and Sit where appropriate, appearance save, logout/login, PostgreSQL restart and a second observer. Then expose a complete AvatarPackage through the shared content registry.
4. Prove the humanoid pipeline with Elara Female, then Elara Male, Kai Male/Female, Terra Male/Female and Ignis Male/Female. Parallel production starts only after the two pipeline proofs.

No new registry, arbitrary model-generated Python execution, renderer rewrite or Blender runtime dependency belongs in this implementation.

## Authoring status

The Mac startup probe on 2026-10-10 blocked inside `NSFileManager.mountedVolumeURLsIncludingResourceValuesForKeys` → `__getattrlist`, before any Python. The same operation was observed in earlier failed authoring; unlocking does not resolve that kernel wait. Diagnostics and the task-specific process identity remain private. No volume was unmounted. A TERM request did not clear the blocked process when checked.

A Debian bookworm ARM64 headless fallback was attempted with official Blender `3.4.1+dfsg-2+b1`; dependency downloads failed connecting to the official HTTP mirror. No image or authoring success is claimed from that attempt. Next agent should resolve authoring availability without repeating GUI waits or altering unrelated mounted data. Native viewer acceptance remains on the Mac.
