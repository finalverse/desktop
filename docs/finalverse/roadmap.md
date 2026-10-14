# Roadmap and recommended next milestone

## Status — 2026-10-14

**Phase 4B remains open.** The user continued the final art, onboarding, world-template and controlled performance gates after the historical [2026-10-12 closure checkpoint](phase4b-closure-20261012.md). That checkpoint records earlier evidence; it does not constitute current product acceptance.

The [connected coastal court](art/coastal-court-validation.md) now supplies explicit world v0.3.0, native full-route/return/Sit/Stand proof, one combined Day/Golden Hour/Night review, a focused actual-shadow check and bounded six-account native concurrency evidence. MutSea completes the chosen wardrobe and dry home before account activation; unsupported choices fail preflight and failed identities stay inactive. Public signup/choices/admission, premium landscape and Night readability, continuous LOD transitions and sustained/device/cold-cache load remain open. No acquisition/public release gate is closed by these local checks.

Part of 4B.1 was delivered with it:
- PlaceTemplate and WorldTemplate provisioning;
- newcomer account → Genesis appearance provisioning.

Grounded shoreline creation, saved recipe replay and one combined island review are complete development slices. The reusable foundation now includes an original island WorldTemplate, safe native rollback and account/home provisioning, described in [island provisioning](world/original-island-provisioning.md). A [native newcomer review](phase4b/newcomer-creation-review.md) verifies visible Auren, grounded creation, inspection, WorldLine and exact Undo; both established local grids received the source guard with their scene/inventory/appearance/history preserved. Market-quality art, public onboarding/choices, a first-time user study, continuous collision/LOD, distribution and controlled performance remain open. Complete those gates before claiming 4B closure or advancing public acquisition.

The subsequent [guided creation and discovery slice](phase4b/experience-discovery.md) closes the per-world/account Lumi availability gap: authenticated metadata, honest citizen controls, first-creation presets and direct Nearby inspection. It passed three native role/world sessions and one combined macOS UI check. The product gates above remain open.

The [reusable coastal arrival](art/coastal-arrival-validation.md) adds two original assets, a compact versioned arrival place and explicit new island v0.2.0. Native walking/canopy collision/Sit/Stand and a combined near/far local rendering sample pass. The existing worlds remain intact; premium art/circulation, public onboarding/study and release performance still gate 4B acceptance.

## Current plan — 2026-10-08

Phase 2 world creation and Phase 3 persistent Lumi are implemented development slices; Phase 4A has passed packaged macOS engineering acceptance. Its UI is retained. The Phase 0 ordering below is historical, not the current execution plan.

1. **4B — First impression:** repair appearance compatibility/provisioning, then produce inviting Harbor/Lumi content as reusable semantic, provenance-checked Starter Library seeds. Verify in the actual viewer, preserve identity/history and measure performance.
2. **4B.1 — MutSea Starter Library:** versioned World/Avatar/Outfit/Asset/Experience packages, dependency validation and recoverable account/community provisioning; reusable Harbor and Lumi Home.
3. **4B.5 — First-time study:** 3–5 new people, five-minute observation, address major confusion.
4. **4C — Growth loop:** create/invite/share/join/remix/return over reliable starter provisioning.
5. **5A–5H — MutSea Platform:** communities; world/experience templates; WorldSync and Blender Live; creator hub; third-party SDK; spatial commerce; enterprise; creator economy. Gate expansion on observed growth, not architectural diagrams.

See [platform boundaries](mutsea-platform.md), [updated 4B prompt](prompts/phase4b.md) and [deferred 4C prompt](prompts/phase4c.md). MutSea is the spatial platform/runtime; Finalverse is the consumer experience; Studio is authoring. Renderer migration, public deployment and multi-platform work remain separately gated.

## Historical Phase 0 plan

Phase 2 follows the later user-supplied **AI world-creation vertical slice**, superseding the original split below. The implemented candidate combines inspection, trusted actions, WorldLine, local inference, one Create panel, a 17-component Lumi home and restart/undo. See [phase2-ai-world-creation.md](phase2-ai-world-creation.md) for measured results and the completed visual workflow and normal restart/cleanup evidence. All 15 practical gates pass. Next harden recovery and semantic reconciliation before persistent citizen memory/navigation; no Phase 3 implementation was started. The remaining text preserves the Phase 0 roadmap rationale.

The product direction is an AI-native persistent world client using the mature viewer as bootstrap runtime. The audited source supplies streaming, avatars, camera, chat/voice, editing, inventory, materials and networking; it does not supply a Finalverse brain, policy kernel, semantic sidecar or action journal.

## Critical path

1. Close the verified baseline build/test/run gate described in `upstream-baseline.md`.
2. Phase 1: controlled product identity, artwork, packaging, attribution and separate settings/cache identity; preserve world compatibility. See [the implemented branding milestone](rebranding.md).
3. Phase 2: C++ value/action contracts, permission and capability policy, immutable snapshot adapter; keep all renderer/UI pointers out.
4. Phase 3: one LLUI AI floater and neutral server/local provider bridge, read-only nearby objects first.
5. Phase 4: one bounded owned-object move with observed result, readable operation record and conflict-aware undo.

The recommended first implementation milestone is **baseline repair followed by controlled identity plus a read-only WorldSnapshot adapter**. The measured repair backlog is the Python packaging-test import failure, local avatar cloud appearance and later viewer unresponsiveness/failed SIGTERM shutdown. Reproduce the runtime issue with evidence gathered before termination; its original cause is not established. Do not implement mutations until world-entry behavior and query freshness/coverage are verified. The first mutation then proves the full model-proposal/policy/executor/WorldLine path.

The subsequent Phase 1 work repairs the eight-case packaging suite and implements product identity. The next architectural milestone remains a read-only snapshot adapter and neutral AI shell after the runtime reliability/appearance gaps are addressed. Branding does not satisfy the AI-native MVP.

## Work to defer

Renderer replacement, general world scripting, multi-object atomic claims, destructive deletion, NPC autonomy, Blender runtime embedding and desktop UI compilation for phones. Studio and native mobile UI remain separate surfaces connected by contracts.

## Evidence for choosing this path

`LLAgentListener::getNearbyObjectsList` already supplies structured UUID/region/position data; `LLSelectMgr::sendMultipleUpdate` supplies existing persistent object transform messaging. Wrapping those paths avoids duplicating the world runtime. They need scope/freshness/permission/result handling, not a new renderer. Mobile extraction begins with these new values/policies, since existing llmath, primitive, appearance and window dependencies are not already a mobile shared core.

No calendar estimates are assigned until dependency availability and runtime baseline are verified. [migration-plan.md](migration-plan.md) contains the phase gates; [test-matrix.md](test-matrix.md) defines evidence required for product claims.
