# Guided first creation and citizen availability — 2026-10-14

Create now reflects the current authenticated world and account. A world without an available Lumi relay offers **A reading nook**, **A small garden**, a custom request and saved recipes. It shows the Finalverse mark, Nearby inspection and native appearance access. A newcomer who can see Lumi receives the same creation entry points without acquiring her controller tools. The authorized controller retains the existing citizen conversation, home, garden and co-building paths.

The guided buttons prefill a request; they do not submit it or approve a change. The existing model proposal → native prepare/policy → explicit approval → commit/PlaceKernel → WorldLine path remains authoritative. This adds no world executor, account grant, citizen embodiment or deployment.

## Authenticated discovery contract

The region's native `FinalverseWorld` capability accepts a bounded JSON POST:

```json
{"op":"experience"}
```

A successful response wraps this result in `{"ok":true,"result":...}`:

```json
{
  "schema_version": 1,
  "region_id": "authenticated-region-uuid",
  "actor_id": "authenticated-account-uuid",
  "recovery_required": false,
  "compose_places": true,
  "citizen": {
    "present": true,
    "can_interact": false,
    "state": "not_controller"
  }
}
```

`present` describes the canonical citizen's live scene representation. `can_interact` describes eligibility for the configured human relay, not provider health or an authorization grant. States are `enabled`, `not_enabled`, `not_in_world`, `not_controller`, `starting`, and `representation_recovery`. Discovery does not call a model/runtime, enumerate the scene, or return controller identity, private endpoints, tokens or memory. It uses the same active capability/session/root-presence checks as other operations. A separate 100 ms discovery throttle prevents its background request from consuming the human action throttle.

The viewer checks schema version, native Boolean types, matching region/account scope and affirmative-control consistency. Unknown/malformed responses leave composed-place and citizen controls unavailable. Its cache is keyed by region UUID, actor UUID and capability URL; a late response from a previous context is discarded. One request runs at a time with a five-second timeout and a 30-second poll interval. These are UI hints; all commands retain native authorization and execution revalidation. Revocation may take up to the next successful poll to hide a button, but cannot authorize execution. A relay can fail after discovery; existing request errors still surface.

An older server exposing `FinalverseWorld` without `experience` retains basic inspection and existing world planning. New composed-place and citizen shortcuts remain unavailable until compatible metadata arrives. During native transaction recovery, Nearby/History remain usable while planning, building and composed-place replay are paused. Human pause/cancel and readable citizen inspection retain their existing server policy.

## UI and cost behavior

- The HUD shows four normal navigation buttons when citizen interaction is unavailable, and restores Lumi for her controller.
- Create/welcome/social copy no longer assumes a citizen connection on every world. An observer sees Lumi as a local citizen, with a tooltip explaining controller eligibility.
- **What's nearby?** opens the existing native inspection result directly. It avoids a model request and still supplies structured objects, distances and semantics.
- “Lumi…” requests remain citizen requests. An unavailable relay produces an explanation; it does not silently reinterpret the command as a general world mutation.
- Recipes use the existing Save/Remix/replay contract. They are not account/session grants or transferable WorldLine authority.

## Verification and limits

| Check | Result |
|---|---|
| MutSea composer/discovery suite | 13 checks pass, including absence, observer/controller separation and representation/startup states |
| Native viewer presentation suite | Nine cases pass; wrong actor/region, coerced Boolean, contradictory presence, future/missing metadata and recovery behavior covered |
| Builds | MutSea module and source-matched native probe: zero warnings/errors; macOS ARM64 Release succeeds with inherited deployment-target linker warnings |
| Native controller login | Live canonical citizen, eligible relay, matching actor/region; normal logout |
| Native observer login | Citizen present, control denied; native citizen inspection refuses access |
| Native island login | Citizen absent, composer available; native citizen inspection refuses access |
| Native authority checks | Body-supplied actor ignored, authenticated identity retained; immediate normal inspection succeeds after discovery |
| Packaged viewer | Deep/strict ad hoc signature verified; provisioned Auren renders in the original island |
| One combined UI journey | Four-button HUD; first-creation card/nook/garden/guidance readable; Nearby shows real world objects; Talk to Lumi disabled; nook request prefills and Apply plan remains disabled |

Three local simulators received clean native backup/shutdown, verified private rollback material, the same new module and restart. Before native probes, all 5,635 primary, 1,472 Tropical and 498 island prim rows, inventory item rows, stored appearance rows and WorldLine sidecar bytes matched their backups. The OAR fixtures' namespace egress guards were installed while paused and verified before access. No new Docker image or PostgreSQL instance was introduced. The primary human Nook was preserved.

The old protocol SDK did not request custom capabilities automatically; its first probe attempt was excluded from acceptance. The corrected probe explicitly negotiates `FinalverseWorld` through the inherited LLSD seed contract, enforces loopback/region/identity, bounded requests/responses and confirmed native logout. Three successful receipts and private raw logs/configuration/backups remain outside Git under `~/Finalverse/dev/experience-discovery-20261014`.

No fresh model-generated build/Undo, repeated avatar views or performance loop was run for this UI slice. The previous [newcomer creation journey](newcomer-creation-review.md) supplies native grounding/build/WorldLine/exact Undo evidence. This review confirms the new entry points, read path and eligibility controls. The local binary was built with preserved unrelated destination/network WIP; it is a development package, not a clean distribution artifact.

The final post-review check again matched all persistent scene rows and WorldLine files across all three grids. The viewer's isolated island session remains open for the user; primary and Tropical have no active human sessions.

**Phase 4B remains open** for final art/landscape and arrival treatment, public registration/character/experience choices, a first-time user study, continuous collision/LOD, controlled performance and distribution approval. This closes the bounded capability-aware UI gap, not those product gates.
