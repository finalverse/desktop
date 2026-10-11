# Harbor hero places — arrival, beacon, waterfront, Lumi's cottage — validation, 2026-10-11

These places follow the [Harbor Pavilion](harbor-pavilion-validation.md) pattern:

- original small assets, pinned into a PlaceTemplate;
- provisioned in one journaled operation;
- retiring sandbox objects with snapshots;
- accepting, explicitly and without modification, the neighbours that stay.

Every place was checked offline against the real catalog with `MutSea.ContentNativeTests --places` before it was deployed. A `pg_dumpall` backup was taken before each change.

The runtime is module r11b (`082ab6f3…`) and viewer QuickCheck r3.

## Places on Harbor

| Place | Placements | Triangles | Lights | Plan | Retired | Accepted (unchanged) |
|---|---|---|---|---|---|---|
| `harbor-pavilion` 0.2.0 | 59 | 27,808 | 4 | `39ec5d4b…` | 12 sandbox pavilion groups | North Promenade |
| `harbor-arrival` 0.2.0 | 70 | 30,848 | 9 | `268fa3f8…` | Welcome Plaza slab, Arrival Marker, "Welcome to MutSea" board | Lake Walk, Workshop Walk |
| `harbor-waterfront` 0.2.0 | 29 | 12,396 | 4 | `eff05a93…` | Lake Overlook, Lake Sign | Lake Walk; FirstLook's bench and lantern by the walk; the estate bench |
| `lumi-cottage-garden` 0.2.0 | 9 | 7,308 | 2 | `b5048321…` | — | All 17 Lumi Home prims; Lumi |

Restarts re-run provisioning idempotently. Each place reports `place_already_provisioned`.

### Harbor Arrival and the Harbor Beacon (B15, B16)

Six new modules make up the arrival:

- **Paving:** 49 plaza pavers with a pale inset in darker waterfront stone.
- **Compass:** a flush bronze compass at the spawn point.
- **Beacon:** three stacked modules on the axis toward the pavilion, keeping each within the asset contract:
  - a stone and plaster base;
  - a bronze shaft;
  - a crown of bronze orbits around a native wayfinding light.
- **Welcome stone:** a stele whose bronze emblem and plate face the compass.

Four benches face the compass, eight lanterns light the path entries, and four blossom trees stand at the corners.

Native evidence is in `arrival-v2-20261011/sheet-arrival-v2-native-r1.png`: the spawn view looking north, an aerial, the beacon and the stele. Feature check: 64–78 fps, p95 18–27 ms.

### Lake Overlook (B17)

The overlook is a 12 × 16 m timber deck on the lake shore, built from approved modules only:

- a balustrade along the water and the north and south edges;
- three benches facing the lake;
- corner lanterns, west steps from the Lake Walk, and a lake stone.

It deliberately has no trees of its own, because the Lake Walk's existing bench, lantern and trees stand beside the steps.

Native evidence is in `waterfront-v2-20261011/sheet-waterfront-v2-native-r1.png`. Feature check: 76–85 fps.

### Lumi's Cottage Garden and the home restyle (B18, B19)

The house, its door, and the space in front where Lumi tends her own garden are not touched. The place puts new raised flower beds along the sides and back, lanterns at the front corners, blossom trees at the back corners, and a bench toward the lake.

The home's 17 prims keep their IDs and geometry. They were restyled with three operator `set_color` WorldPlans (`f5ae0e54…`, `a505dcc9…`, `13cceb40…`), each journaled and undoable:

- warm plaster walls;
- a terracotta roof;
- a timber lintel, floor and table;
- a sage cushion.

Native evidence: `garden-v2-20261011/sheet-cottage-garden-native-r2.png`, 83–88 fps.

## Citizen regressions (B13, B19, B21, B22)

| Check | Result |
|---|---|
| Lumi walks home: front-west | Arrived, 7 waypoints |
| Lumi walks home: directly behind the house | Arrived, 20 waypoints around the beds and trees (after the navigation fix below) |
| Lumi walks home: back-west | Arrived, 13 waypoints |
| Lumi's own garden | Proposed three shrubs; approved by the controller and planted at (170.6 / 172.0 / 173.4, 148.8); undone and removed |
| Identity / restart verifier | PASS: identity, autonomy, embodiment, semantic home relationship, memories, goal history |
| Agent semantics of placed parts | Place context (title, role, entrances) through the World relay; shown for the pavilion |

**Navigation fix found by this check.** The citizen navigator treated each object as a square sized by its whole bounding box. Tree canopies, which have no collision and sit overhead, closed the 2 m grid corridors around the house, and a first walk from behind the house failed.

Snapshots now report a `body_radius` built only from parts that collide at body height. An upright part uses its XY extent; a tilted part uses its 3D diagonal. Placement safety is unchanged.

The first cut measured every part by its 3D diagonal. That inflated the reach of tall walls and briefly blocked Lumi's own doorway; it was corrected before the walks above. Covered by world test 56.

## Open items

- **Frame time** dips with more parts and lights in view: the arrival is at 64–78 fps, the pavilion interior at p95 about 28 ms. To be addressed in the final profile pass.
- **Presets:** Golden Hour and Night are not yet reviewed for any hero place.
- **Garden beds** read small from a distance. The house remains a simple primitive cottage; a geometric upgrade would need identity-preserving replacement.
- **North Promenade** still runs about 1.3 m past the pavilion's north steps.
- **Gallery:** the QA gallery is disabled in the hero configuration (`MutSea.ini.gallery` keeps it).
