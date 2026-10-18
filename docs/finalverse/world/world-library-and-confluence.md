# World Library and Confluence

Confluence is the working name for Finalverse's spatial AI workspace. MutSea remains the world authority; the open-source viewer is the interaction surface. This is a developer-preview foundation, not a finished marketplace or a replacement for every AI authoring tool.

## Visit a world

Create a world account at [MutSea Account](https://mutsea.com/account/#register), then use the same **First Last** avatar name and password in Finalverse. A UUID or email is not the viewer login name. Downloading the viewer does not create an account.

New viewer builds provide **World Library** from **Worlds** in the bottom dock (also available through the existing Destinations/Discover control). Select an online destination and choose **Teleport**. Availability comes from the connected grid and an operator-reviewed catalog. Offline, incomplete and recovery-fenced imports are not advertised as online.

The published preview.3 predates this interface. In that version use **World Map**, search for the exact region name, and **Teleport**. The admitted regions are **Confluence**, **Lake House**, **Conference Center** and **Autumn Castle**, alongside Harbor. [Server acceptance and current boundaries](https://github.com/finalverse/mutsea-o/blob/main/docs/mutsea/world-library-acceptance-20261018.md) distinguish live worlds from staged archives.

The published preview.3 can disconnect during repeated cross-region teleports because the source circuit may close before the destination handshake completes. Relaunch and sign in if affected. New source bounds deferred source removal to that handoff. A later native session also disconnected after arrival when the server stopped receiving client UDP packets for sixty seconds; the lasting-connection cause remains under investigation. A subsequent combined native run stayed in Confluence for over eight minutes and returned to Harbor successfully. Public protocol probes also passed, but the earlier disconnect and intermittent discovery timeouts remain open for release review; a new installer is held pending native stability and fresh-package checks.

## Confluence

The original CC0 world template has three distinct places:

- **Conversation Circle**: shared context table, four seats, context and artifact-review tables.
- **Review Pavilion**: a place to inspect proposed changes together.
- **Quiet Shore**: reflection by the water. Public-world proximity is not access to a person's private conversation.

Use **Open AI workspace**, or open AI Create and ask about the workspace. Authenticated inspection provides the region's workspace and zone facts to the configured planner. Nearby chat, voice, private transcripts, credentials and inventory are not automatically forwarded. Model output remains a proposal: review the preview and choose **Apply**; use **Undo last** where supported.

This first version supplies spatial context to the existing planner. Persistent private conversation threads, artifact boards, participant consent, agent-to-agent sessions and integrations with image/workflow tools are subsequent work. There is no arbitrary Python, node-graph or shell execution capability.

## Customize and release

Shared imported samples are preserved as originals. Where their advertised license allows it, **Take Copy** into your own inventory, rez the copy on land where you have building rights, edit it, and save it using the existing inventory tools. Native permissions and source-license obligations are separate requirements. Do not edit a shared original in place.

AI-created places from the approved Starter Library can also use:

1. Review and Apply the place in AI Create.
2. **Save release** creates a private immutable recipe in Creator Library.
3. Open **World Library**, select your release, and **Publish free · CC0** to make that recipe reusable. This is an explicit CC0 publication choice.
4. **Preview remix** checks the current catalog, terrain and your parcel permissions. Review and Apply the new copy. A saved remix retains its parent release ID.
5. **Unpublish** removes the active listing; it does not revoke copies already shared under CC0 or delete your immutable release.

Recipe export refuses to discard changes made outside the recipe. Save those edited objects through native inventory instead. General mesh/OAR uploading, mixed-license creator publication and complete-region release are not exposed by this initial release API.

The backend can record priced offers, but **checkout is disabled**. There is no payment, payout, entitlement or resale-enforcement service yet. A price does not remove the rights granted by an underlying CC0 license.

## Source and reuse

Imported regions retain their own provenance. Zadaroo's publisher declares CC0; attribution is retained in the directory and import records. OpenVCE includes contributor-specific terms and is held for separate review. Do not infer redistribution or sale rights from an editable object alone.

See [the server architecture and current boundaries](https://github.com/finalverse/mutsea-o/blob/main/docs/mutsea/world-library.md) and [the authoring source](https://github.com/finalverse/studio).
