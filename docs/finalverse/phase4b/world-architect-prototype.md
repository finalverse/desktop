# World Architect: reusable WorldSpec slice — 2026-10-13

**Implemented locally:** natural-language composition can return an editable WorldSpec; a saved recipe can be replayed without inference into the existing WorldPlan/MutSea path. This is a working single-place foundation for Create→Share→Remix, not a whole-city generator or a public social product.

```mermaid
flowchart LR
    A[Natural language] --> B[Existing intent model]
    B --> C[WorldSpec recipe]
    C --> D[Save / edit a copy / replay]
    D --> E[Existing solver and critic]
    E --> F[WorldPlan preview]
    F --> G[MutSea policy / approval]
    G --> H[PlaceKernel / WorldLine / Undo]
```

## Contract

`composer/world_spec.py` reuses the existing PlaceIntent schema. WorldSpec1 carries `kind=world_spec`, `solver_version`, `library_sha256` and `place` choices. Unknown fields, versions and catalogs are rejected. It contains no actor credentials, entity IDs, runtime coordinates, asset URLs or executable code. Canonical JSON plus SHA-256 makes choices diffable; changed seating/planting/lighting produces a distinct recipe. JSON key order does not change its identity. Current solver/catalog versions are pinned; historical compiler resolution and automatic recipe upgrading are not implemented.

Successful composition responses add `world_spec` and `world_spec_sha256`. The existing `plan` field remains the same schema. `POST /v1/world/spec/plan` accepts only a wrapped recipe and does not call a model. The offline CLI `python -m composer.world_spec recipe.json NEW-preview.json` also compiles a proposal and refuses existing outputs. The checked-in `examples/caelwyn-reading-garden.world.json` is original bounded authoring data.

MutSea remains authority: it replaces placeholder identity with the authenticated actor, selects a site in front of that actor, checks terrain, parcel permissions, occupied space, asset pins, budgets and pending approval expiry. The existing deterministic PlaceKernel performs the commit and fingerprint-guarded Undo. Sharing the recipe grants no access to the author's account or existing world. The recipe hash is returned to the authoring client; it is not yet a separately indexed WorldLine field or marketplace record.

## Actual validation

Gateway suite: **66 tests** pass, including8 WorldSpec tests for canonical identity/remix, deterministic replay, repair isolation, authority/code/URL rejection, invalid budgets/versions, inference-free replay, real loopback HTTP and sanitized refusal. The existing58 tests still pass. Standalone CLI produced the expected preview and recipe hash `4939793b08a8fa26f55b9faa5f8c194963f2b84bb315bd5317f7f3ed418ee01e`.

An isolated loopback gateway on18767 ran the new source without replacing other services. A real **Harbor Newcomer** account submitted the saved recipe through the native viewer capability/protocol harness:

| Step | Result |
|---|---|
| Gateway replay | 91 ms, no model call |
| MutSea prepare / policy preview | 497 ms |
| MutSea deterministic commit | 981 ms |
| Created result | 9 approved placements /81 native parts |
| Native object stream | 848→929 |
| Semantic query | `Caelwyn Reading Garden`, `user.36210212.caelwyn-reading-garden`, authenticated newcomer owner |
| Guarded Undo | 929→848; fixture restored |

Receipts are private in `dev/phase4b/closeout-20261013/worldspec-native-{build,undo}.json`. The probe supports a bounded regular recipe and only the designated loopback gateway ports; no remote endpoint, redirects or new cloud credentials. All creation still uses the existing prepare/commit/undo operations. No simulator module was changed for WorldSpec.

The first database dump failed because its command referenced an unset POSTGRES_USER. The successful retry followed the build; it is not a pre-operation backup. A clean post-Undo full dump was saved. Earlier full dumps and kernel snapshots remain preserved, and no existing object was replaced. Record this failure rather than invent backup ordering.

## Limits and next slice

This verifies **gateway→native MutSea→semantic result→Undo**. It does not establish a consumer Share/Remix button, two-user native-screen acceptance of the recipe result, new-recipe restart acceptance, a five-minute human onboarding session or public deployment. The reused compose_place kernel's earlier second-observer/restart tests remain retained; they are not relabelled as new recipe-specific visual tests. No repetitive avatar captures were needed for this backend addition.

Next: expose saved recipes through the existing Create panel, preview/remix them as choices, then add a grounded terrain-aware site selector and Lumi co-builder. Preserve the existing approval path. Aether Sprite remains a later interaction concept rather than a new agent/model requirement. Multi-place/world composition requires an explicit transactional contract; do not silently widen this one-place version.


A real local Ollama request also exported a WorldSpec: “Create a tiny reading nook with two benches under a tree and flower beds.” Inference took13.69s during the native build. Its saved recipe replay took34.27ms (31 placements), and a three-bench remix took17.08ms (35 placements), without further inference. Distinct canonical hashes and actual timing receipts are retained in `worldspec-replay-latencies.json`. These are local feature measurements, not an end-to-end10s release guarantee. The temporary loopback18767 smoke gateway was stopped after these checks; the unchanged simulator remains running.
