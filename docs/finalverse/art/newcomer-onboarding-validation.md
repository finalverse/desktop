# Five-minute new-user entry — validation, 2026-10-11

This is the integrated plan's gate L: a brand-new account reaching a meaningful first experience in Harbor within five minutes, with no operator action after the account is prepared.

It was run with one new local test account, **Harbor Newcomer**, created for this check. Its credentials are generated and kept only in private 0600 fixture files.

## What a newcomer gets (account → appearance)

MutSea `[FinalverseAccounts]` (commit `2a0320e206`) prepares the account before its first login:

1. **Account.** Created through the inherited account service, the same path as the console `create user`. That provides the password, inventory, a verified starter appearance and activation (state `active`, level 0).
2. **Home.** Set to the Harbor arrival compass. The account service had left it unset: at `PrimsLoaded` the default region is not registered yet, so its own home lookup failed. The fix fills only a missing home.
3. **Genesis outfit.** The chosen template is applied through the wardrobe. Here that was Kai Female 0.1.0, action `0cde409b…`.

Passwords are never logged or journaled. Restarts are idempotent: the account is not recreated and the outfit is not re-applied.

## The path, timed

| Step | How | Result |
|---|---|---|
| First login, fresh install | Native viewer, new empty profile, cold cache | Launch to first rendered capture in about 51 s, including a 10 s warm-up and 10 s sample (so about 30 s to in-world) |
| Where they land | Native and protocol | On the arrival compass, 0.14 m from its centre, facing the beacon, the north walk and the pavilion |
| What they look like | Native close view plus a second account | Kai Female worn. The other account sees the Genesis mesh at point 10 and all five body regions as invisible bakes |
| What they can learn | The newcomer's own World relay (semantics) | The beacon and stele resolve to the place **Harbor Arrival**, which lists the welcome text and four entrances |
| Lumi | Protocol | Lumi's embodiment is in the newcomer's object stream, 49 m away at her cottage |
| Starter world | Protocol teleport | Harbor Template: 670 primitives streamed; back at the arrival at 10.0 s |
| Walk to the pavilion | Protocol autopilot | Past the beacon, up the north walk, onto the deck: arrived at 28.3 s |
| Sit and stand | Protocol, then native | Sat on a shared pavilion bench at 28.9 s and stood at 31.1 s. The native second login also sat and stood on the same bench, with native sit and stand animations at 57–60 fps |
| Comes back | Native re-login | Resumed at the pavilion still wearing Kai Female; the second account confirmed it again |

The protocol journey finished all **9 milestones in 31.1 s** against a 300 s budget. It ends at the pavilion, so the next login resumes there.

Evidence is in `dev/phase4b/onboarding-20261011/`:
- `newcomer-journey-a3.json` (protocol receipt);
- `observer-first-login.json` and `observer-second-login.json`;
- `first-login-a2/` and `second-login-c1/`;
- `sheet-newcomer-native-walk-sit-stand.png`. Its top row is the arrival view, the portrait at the compass, and the approach to the bench; its bottom row is seated and standing.

## Defects found by this check

- **Missing home** for accounts created while regions load. Fixed in the provisioning step, as described above.
- **The protocol walk flew onto the pavilion roof** on one attempt. The autopilot target was the avatar's current height; on the raised deck that exceeds terrain plus avatar height, which switches the server's autopilot to flying. The probe now uses ground-level targets, as a walking user would. Native click-to-walk was not affected.
- **The World capability URL changes when the avatar re-enters a region.** Each entry registers a fresh capability, so the old URL returns nothing. This is correct security behaviour; the probe now requests the seed capability again after returning.

## Not covered (gate stays PARTIAL)

- **No human first-time user was observed.** The timings are a machine critical path, a lower bound on what a person needs. A real five-minute session with an unfamiliar user is still to do.
- **There is no self-serve sign-up or in-product template chooser.** The Genesis choice is operator-made in the private newcomer file. Public registration is out of scope and not authorized.
- **A newcomer cannot talk to Lumi.** The citizen relay is controller-only by design, so newcomers see Lumi and hear what she says aloud but cannot ask her anything. A safe visitor-greeting policy is a Phase 4C design question, not a change made here.
- **There is no in-viewer guidance** (hints, a first-run tour). The arrival's beacon, stele and paths are the only wayfinding.
- **Native-capture framing:** the bench camera is partly blocked by the pavilion's centre table.

## Gate

| Gate | Status |
|---|---|
| L five-minute new-user entry | PARTIAL: machine path PASS (provisioned account, first login, arrival, semantics, starter world, pavilion walk, sit/stand, re-login persistence, second observer, 31.1 s). Human session, self-serve chooser and visitor-to-Lumi interaction open |
