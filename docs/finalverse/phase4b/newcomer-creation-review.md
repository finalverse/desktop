# Native newcomer creation and local integration — 2026-10-14

The original Starter Library island now has a verified native viewer journey: a provisioned Auren account enters through a fresh private profile, renders its appearance, describes a place, reviews the grounded proposal, approves it, inspects the new objects and WorldLine, and undoes that exact operation. This is a local development acceptance slice. Public registration, avatar/experience selection and the first-time user study remain open.

## Preserve established worlds

The source-matched WorldTemplate guard was deliberately installed in the primary Harbor/OpenVCE/Harbor Template grid and the separate curated Tropical grid after confirming no active user sessions. Both received native backup/shutdown, private database/configuration/module/journal backups and a clean restart. Tropical's namespace egress guard was reapplied while paused before client access; its scripts remain disabled. No public deployment, new PostgreSQL instance or Docker image was made.

Full persisted comparisons after startup confirmed:

| Local grid | Scene part rows | Inventory item rows | Stored appearance rows | Existing journals |
|---|---:|---:|---:|---|
| Primary | 5,635, identical | 91, identical | 54, identical | byte-identical |
| Tropical | 1,472, identical | 12, identical | 10, identical | byte-identical |

The primary human Nook remains untouched. Historical world journals remain readable and idempotent; an old operation without compatible rollback postconditions still refuses automatic rollback. Installing the guard does not synthesize missing history.

Tropical's development account had a zero HomeRegionID. The existing account service filled only that missing home on nine sampled dry, level terrain points, read it back and emitted `newcomer_ready` with `created=false`, `home_set=true`. Its password, outfit and inventory were preserved. A strengthened native OAR probe asserted `start_location=home`, the authenticated account and region, all 1,472 part transforms, five exact source texture payloads and normal logout. No appearance was sent by the probe.

## One bounded creation journey

The original island retains its four pinned places and 134 groups / 498 parts. The existing, provisioned Island Newcomer wore Auren through a fresh viewer profile; appearance loading resolved to a visible native avatar. This was not a public registration or another nine-avatar screenshot loop.

From the busy arrival plaza, “a small coastal reading nook near the shoreline” produced a clear refusal: no suitable shoreline site nearby. After a native map teleport to clear dry coastal terrain, the same request produced a validated preview at `[194,150,22.4]`, yaw −90°. The runtime selected the site from actual terrain, water, collision and permission checks. The model supplied intent and recipe choices, not arbitrary coordinates or executable code.

Approval created one bench, four pavers and two warm path lanterns: seven linked groups / 29 native parts. They visibly appeared beside the coast. Nearby listed the new seating, floor and lights; History showed the intent and commit. Named operation `70cb3ce6-582c-4ec3-8c17-3fc0d716c81f` was then undone through the viewer. The objects visibly disappeared and all original 498 persisted prim rows matched the pre-review baseline exactly. No template rollback or other person's Undo was used.

## Login warning repair

The first review showed “Your requested location is not currently available” despite a successful native `last` arrival. A one-session login bypasses the login panel and can leave its startup SLURL unspecified. The inherited request defaults to `last`, while startup checked only an explicit last/home/URL type and incorrectly reported a fallback.

`llLoginStartDestination()` now supplies the same expected destination kind to request construction and reply comparison. URL coordinates and explicit home/last semantics are preserved. A real `safe` fallback or mismatched explicit destination still triggers the warning. Camera restoration stays subject to its existing explicit-last checks.

The macOS ARM64 Release viewer and login-test target built successfully. The native login suite passed three cases, including actual request/default-reply agreement, explicit destination distinctions and fallback mismatch checks. The rebuilt, deep-verified ad hoc package entered using another fresh private profile, displayed Auren and showed no false warning. Executable SHA-256: `0ca313820d6a07e5a6664fc199ac9be21de161256032613fd224b2ca8bd254cf`. The local package includes preserved unrelated viewer work excluded from this source commit. Existing Velopack library deployment-target linker warnings remain; this run does not validate older macOS support.

Private evidence lives under `~/Finalverse/dev/onboarding-20261014`: integration comparisons and backups, create/undo journals, exact prim-row check, home-login/texture receipt, build/test logs, isolated launch inputs and rebuilt app. Credentials and full database/configuration files remain outside Git. GUI observations were combined in the active conversation; no repeated screenshot export or benchmark loop was run.

## Remaining gates

Phase 4B is open for final art/landscape and arrival treatment, public onboarding and character/experience choices, capability-aware availability of Lumi on each world, continuous collision/LOD interaction, controlled performance and distribution approval. The original island is a reusable functional seed. This acceptance does not establish premium hero art or a performance budget. Next focus on a clear first creation experience and reusable authored content; retain the existing policy → prepare → commit → PlaceKernel → WorldLine path.
