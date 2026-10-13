# Local Docker cleanup — 2026-10-13

The owner requested removal of obsolete development images, instances and PostgreSQL stacks to recover disk space. Inventory was captured before changing anything. Cleanup targeted retired Finalverse/MutSea fixtures and regenerable unused builder cache. Unrelated project containers and databases were preserved.

## Retained current runtime

| Container | Purpose |
|---|---|
| `mutsea-finalverse-phase4b-pg-20261009` | Current Harbor, OpenVCE Plaza and Harbor Template simulator |
| `mutsea-phase4b-postgres-20261009` | Current primary PostgreSQL database |
| `mutsea-tropical-stage-20261013` | Current isolated Tropical compatibility/content fixture |
| `mutsea-tropical-postgres-20261013` | Tropical's separate PostgreSQL database |

Their seven related config/runtime/scene/database volumes were retained. Needed simulator, PostgreSQL, firewall and build images remain. Current simulator containers contain module overlays; rebuilding from an image tag alone does not reproduce those overlays. Preserve the documented source-matched native build and private fixture configuration.

## Removed and recoverable

Removed 22 retired MutSea containers and 19 obsolete related volumes, including the earlier isolated OpenVCE PostgreSQL/simulator/loader and pre-PostgreSQL, Phase 2/3, TLS and old ARM64 fixtures. Before volume removal, read-only archives were verified by gzip integrity, member count, bytes and SHA-256. Old compiled ARM64 tooling was archived separately. Recovery archives and container inventory/diffs/logs are private local files under `dev/docker-cleanup-20261013`; they are not in Git. The former `…-pre-openvce` and `…-r4-retired` containers are no longer available as one-command rollback targets; their relevant persistent data is in the private recovery set.

Obsolete image tags and unreferenced image IDs were removed without forcing deletion of an image used by a retained container. Builder cache was reclaimed by age and LRU with a bounded reserve; shared layers needed by current images remain. No blanket volume or image prune was used.

| Measure | Before | After |
|---|---:|---:|
| Containers, all projects | 62 | 40 |
| Volumes, all projects | 360 | 341 |
| Images, all projects | 38 | 29 |
| Builder cache | 18.84 GB | 4.329 GB |
| Approximate Mac free space | 47 GiB | 61 GiB |

About **14 GiB** of physical Mac space was recovered. Verified private recovery archives occupy approximately 245 MB. Docker's remaining reported reclaimable volumes mostly belong to other projects and were not treated as disposable. Current primary simulator/PostgreSQL and separate Tropical simulator/PostgreSQL remain running; gateway and citizen-runtime readiness checks returned HTTP 200. This was local cleanup, not a remote fleet cleanup or public deployment.
