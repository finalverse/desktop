# Harbor starter world, lighting review and a disk incident — 2026-10-11

## Walks: the last Harbor connectors

Two places built from plaza pavers and lanterns replace the white v1 path slabs. Each retired its v1 slab with a snapshot:

- `harbor-north-walk` 0.2.0: 13 placements, from the plaza to the pavilion. Plan `ecf15fcc…`; retired the North Promenade.
- `harbor-lake-walk` 0.2.0: 9 placements, from the plaza toward the overlook. Plan `b2ff2cfc…`; retired the Lake Walk.

The first attempt was refused. The north walk's bounds reached 5 cm into the pavilion's south-step lanterns, and it was moved 0.1 m south.

## WorldTemplate: `mutsea.starter.harbor-world@0.2.0`

A reusable starter world, made of:

- Harbor Arrival (with the beacon), the pavilion, the Lake Overlook, and the North and Lake walks;
- Harbor's own terrain, exported to R32 (`b70f57ee…`), with water at 20 m;
- spawn at the arrival compass, and the Day environment preset reference.

Every place is pinned to the SHA-256 of its exact `place.json`. Studio builds it with `scripts/build_world.py`; MutSea provisions it with `WorldTemplateKernel` into an **empty** region (see the World README).

| Check (fresh "Harbor Template" region, loopback port 18097) | Result |
|---|---|
| Provision | One operation: terrain/water snapshot (`3a526547…`), template terrain, then five places in order (large first, walks last). 180 groups / 669 parts in the region database |
| Second account visits | Teleport Harbor → Harbor Template in 0.3 s; 669 primitives streamed; back in 0.3 s with Lumi visible |
| Rollback | World journal: intent → committed → rollback_intent → rolled_back. All 5 place plans rolled back; 0 objects left; terrain restored from the hash-checked snapshot |
| Re-provision | Operation `bb8eb564…`; 669 parts again |
| Native view | Not captured inside the template region. The capture harness is deliberately pinned to Harbor's region ID; the template uses Harbor's exact places, which are captured below |

## Lighting review: all eight canonical cameras, three presets

These are full native runs: fresh launch, 10 s warm-up and sample per camera, shadows on, QuickCheck r3. Each reuses the sky and water that native validation accepted in the 2026-10-09 preset runs.

| Camera | Day | Golden Hour | Night |
|---|---|---|---|
| Arrival | 78.9 fps / p95 19.1 ms | 80.9 / 16.9 | 83.5 / 15.6 |
| Harbor (overview) | 70.1 / 27.0 | 78.3 / 26.6 | 80.7 / 26.7 |
| Lumi | 79.9 / 25.1 | 87.7 / 14.4 | 84.7 / 16.1 |
| Lumi Home | 89.0 / 24.7 | 90.2 / 24.7 | 89.8 / 24.7 |
| Garden | 84.2 / 16.0 | 82.5 / 16.4 | 85.3 / 15.3 |
| Pavilion | 86.1 / 15.1 | 86.7 / 14.6 | 84.2 / 15.5 |
| Waterfront | 82.5 / 24.9 | 86.1 / 14.9 | 85.9 / 15.2 |
| Sunset | 116.8 / 10.5 | 116.0 / 10.4 | 116.5 / 10.3 |

Sheets are in `dev/phase4b/hero-env-20261011/sheet-hero-{day,golden-hour,night}-8cam.png`.

- **Night:** lantern pools mark the plaza, both walks, the overlook and the pavilion steps. The beacon marks the axis, and the lake is moonlit. The pavilion interior is dim, because its only lights are at the steps.
- **Golden Hour:** low warm light, and the sun's reflection across the lake in the Sunset view.

These are feature runs, not a controlled release benchmark. They are faster than the 10-09 pre-hero baseline, but the viewer build, shadow setting and sample length all differ, so no cost is inferred.

A first attempt derived the presets from the newer matched baseline. Its recorded sky and water carry an `asset_id` field and a blank-string texture, and native validation rejected them. Reusing the previously validated preset environments fixed it.

## Incident: disk full from a recreated simulator container

To publish a UDP port for the template region, the simulator container was recreated from a committed image (`mutsea-finalverse-phase4b:harbor-r5-20261011`). The original container is kept stopped as `…-r4-retired`.

- **What went wrong:**
  - The recreation omitted `-it`. The original ran with OpenStdin and TTY.
  - The simulator console then printed prompts in a tight loop, flooding the container log.
  - Within about 20 minutes the host data volume was full (2.2 GiB free), and the Docker daemon became unresponsive.
- **Mitigation:**
  - Deleted the superseded QuickCheck r2 package (1 GB; r3 contains its fix).
  - Quit Docker Desktop; its VM stopped gracefully. Force-killed the stale backend.
  - Restarted Docker, then removed the flooded container. Its log went with it; no volume was affected.
  - Host space recovered to 92 GiB as Docker trimmed.
- **Recovery:**
  - Recreated the container exactly like the original: `-it`, on both the database network and `bridge`, same volumes and user, plus port 18097.
  - The log is now quiet, and all places and the world were idempotent on start.
- **Side effects:**
  - Restarting Docker restarted every container. Unrelated stacks returned on their own restart policies.
  - Several local CreditChain and exchange containers are crash-looping on **missing bind-mounted config files or a read-only filesystem**. These look like pre-existing configuration problems, not disk damage, but their history was reset, so this is not proven.
  - They were not touched; they are outside this work's scope.
- **Rule:** recreate the simulator only with `-it` and both networks. The World README records this.

## Gates affected

| Gate | Status |
|---|---|
| B6 Golden Hour | PARTIAL: eight-camera native set captured; art-direction sign-off pending |
| B7 Night | PARTIAL: destination lighting hierarchy now present (beacon, lanterns, overlook); pavilion interior dim |
| B24 final screenshots | PASS, current fixture: eight-view hero sets in Day, Golden Hour and Night |
| B25 final FPS | PARTIAL: 70–117 fps, p95 ≤ 27 ms in feature runs; controlled release benchmark pending |
| World template (integrated plan K) | PASS, current fixture: provision, protocol visit, rollback and re-provision |
