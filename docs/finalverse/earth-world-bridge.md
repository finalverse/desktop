# Earth → World · local v0.6 preview

The **Create → Earth → World** panel connects a reviewed Earth Intelligence
proposal to MutSea's existing native world transaction system. It is available
only when the current region advertises the bridge. The public Mac preview does
not include this local development panel yet.

1. Log into the intended MutSea grid and region. The panel displays your actual
   avatar UUID for the Earth CLI handoff; use that UUID, not your display name.
2. Create a proposal using Earth Intelligence's `experience plan` command. A
   geographic proposal uses an owned, approved spatial workspace; a virtual-only
   proposal needs no geographic dataset. Issue its scoped one-use handoff using
   `experience handoff … --agent … --output /private/handoff.json`.
3. Paste the token into the masked handoff field and choose **Preview**. The field
   is immediately cleared. Review provenance, schematic/virtual coordinates,
   generated interpretation, source exhibits, permissions and siting.
4. Choose **Approve & create** or **Reject**. Approval belongs to this native login
   and expires in two minutes. A changed account, region or capability invalidates
   the panel's pending state. A new login requires a new preview.
5. Save the native plan UUID. **Inspect** shows its authoritative outcome and
   WorldLine receipt; **Visit** goes to the native place entrance. **Undo** uses
   existing ownership, parcel and fingerprint checks and refuses later edits.

The panel sends a mutation once, with retries disabled. If no result is
confirmed, it disables approval and asks you to inspect the saved plan ID. Never
repeat creation to resolve an uncertain response. Reopening a retained plan
refreshes its authoritative review rather than showing stale approval text.

**Ask Lumi to review** is enabled only in the existing citizen's interactive home
region. This is a read-only conversation over the same Lumi identity/controller;
it grants no edit permission. There is no embedded globe, new renderer or model
credential in the viewer.

Cross-service configuration and the acceptance ledger live in the
`finalverse/finalverse-earth` repository under `docs/earth/bridge-v06/`. Native
runtime adaptation is documented in MutSea's
`docs/finalverse/earth-world-bridge.md`. Public deployment and release remain
separate from the local build and visual checks.

## Native verification fixes

HTTP completion dispatches to the main viewer coroutine before touching controls
or rendering text. The compact panel fits the tested Mac viewport.

The shared asset fetcher resolves ViewerAsset from the current region for each
request after waiting for capabilities. It no longer retains a departed region’s
URL across teleport. This applies to native material delivery generally, without
changing material assets, the renderer, or world persistence.

Startup grid changes clear credentials supplied for a different explicit grid
and disable automatic login. Same-grid startup keeps its supplied login. A
regression test covers both outcomes. For an operator’s private grid test, use
an explicit `x-grid-location-info://<grid>/region/<region>/x/y/z` location; a
generic `secondlife://<region>/…` location selects the inherited grid. Never put
a password or token in process arguments.
