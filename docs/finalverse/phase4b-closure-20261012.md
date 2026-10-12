# Phase 4B closure — 2026-10-12

Phase 4B ("first impression") is **closed by product decision, with carry-forward**. Closing it is not the same as passing every gate. Each open gate below is moved, explicitly, to the track that owns it. The [B1–B28 ledger](art/phase4b-gates-20261010.md) and the [integrated ledger](phase4b-integrated-plan.md) remain the evidence of record.

## What Phase 4B delivered

- **Lumi as a native animated citizen.** The canonical identity was migrated in place, with walk, greeting, listen and talk motions, home and garden autonomy, and body-height navigation. It survives restarts.
- **Genesis humanoids.** Nine versioned templates are worn natively with alpha-hidden bodies:
  - Elara F, Auren (successor to the provisional Elara Male), Kai F/M, Terra F/M, Ignis F 0.2.0 and Ignis M;
  - a retirement and alias policy for templates;
  - a viewer bake fix (alpha-hidden regions publish as `IMG_INVISIBLE`).
- **PlaceTemplate kernel.** Places compose approved assets in one journaled operation, with snapshot retirement, explicit accept lists, compensation, fingerprint-guarded rollback and place semantics for agents.
- **Hero Harbor.** The Pavilion, the Arrival with its Beacon, the Lake Overlook, Lumi's Cottage Garden and the North and Lake walks, in Day, Golden Hour and Night eight-camera native sets.
- **WorldTemplate.** `harbor-world@0.2.0` can be provisioned into an empty region, visited, rolled back and re-provisioned.
- **Newcomer provisioning.** A new account gets a chosen Genesis outfit and the arrival as its home; the machine-path five-minute entry runs in 31.1 s.
- **Operations.** PostgreSQL runtime, OpenVCE as a second region, the native QuickCheck harness, and protocol probes for every flow above.

## Final gate state

Ledger: **15 PASS, 12 PARTIAL, 3 FAIL** (B1–B28, K and L). Genesis checks: **10 PASS, 2 FAIL** (H1–H12).

| Open item | State | Carried to |
|---|---|---|
| H9 Genesis textures and faces; B9 Lumi textures and gestures; B11 Lumi Sit | FAIL / PARTIAL | **Market-quality visuals** |
| B14/B16/B17/B18 hero polish: skylight soffit, presets, Lumi Home geometry (identity-preserving) | PARTIAL | **Market-quality visuals** |
| B6/B7 Golden Hour and Night art-direction sign-off; pavilion interior night light | PARTIAL | **Market-quality visuals** (needs the user's review) |
| B5 Balanced profile; B8 pavilion hotspot; B25 controlled release benchmark | FAIL / PARTIAL | **Performance profile**, on final content |
| B3 native continuous route capture | PARTIAL | Regression suite of the next track |
| B20 agent dialogue about hero assets | PARTIAL | **AI track** |
| L human first-time session; H10 self-serve chooser; visitor-to-Lumi interaction | PARTIAL / FAIL | **4B.5 first-time study** and **4C** |
| B27 historical cached-provenance incident | FAIL | Security housekeeping; it does not block local R&D |

## Baseline and rollback

**Commits** (local only, all tagged `phase4b-close-20261012`):

| Repository | Commit |
|---|---|
| MutSea `codex/phase4b-destinations-import` | `2a0320e206` |
| Studio `main` | `228e4eb` |
| Viewer `codex/phase4b-destinations-import` | this closure commit; the 15 destination/UI work-in-progress entries are preserved uncommitted |

Nothing was pushed or deployed remotely.

**Runtime as last verified on 2026-10-11:**
- simulator image `mutsea-finalverse-phase4b:harbor-r5-20261011`, plus module r13c `c858dc0e…`;
- configuration `MutSea.ini.stage3` (places, world, newcomers), on the PostgreSQL volume;
- viewer QuickCheck r3 `7a8fadb6…`.

Rollback modules r12, r11b, r10b and r9 are kept, as are the database dumps taken before each place, the world and the newcomer.

**Runtime snapshot pending.** The host rebooted on 2026-10-12 and Docker was not running at closure. Take a final `pg_dumpall` and commit the simulator image at the next Docker start. That start also restarts unrelated local containers that crash-loop on missing configuration; they are outside this work.

## Lessons kept for the next phase

- **Verify in the native viewer, and make claims from protocol receipts.** Several defects showed only in real use: a bake ignoring alpha, a navigation footprint inflated by canopies, a missing newcomer home, an autopilot that flew, and a capability URL that changes on region re-entry.
- **The model proposes data; deterministic kernels validate and journal it.** Every place, world and outfit change is reversible. Keep that boundary for AI-driven features.
- **Operational rules:**
  - recreate the simulator only with `-it` and both networks;
  - build with `--no-incremental` after the clock jump;
  - never put credentials in console scripts or argv.
