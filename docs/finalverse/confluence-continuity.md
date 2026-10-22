# Confluence continuity · v0.7 integration

This branch adds an **Earth session** recipient and **Artifacts** tab to the
existing movable Conversation window. It is an opt-in local integration, not a
new public viewer release. World AI, Nearby and the existing Lumi controller
path retain their existing authorities.

The Earth session owner creates a short-lived grant for a specific native
avatar, grid and region. Paste `/connect <handoff>` into Conversation; input is
consumed without being added to chat history, settings or logs. `/disconnect`
clears the native link; server revocation remains an explicit owner operation.
Never paste an owner API credential or login password as a handoff.

Shared messages and source previews use the native `FinalverseWorld` capability,
MutSea's existing private Earth transport, and the canonical Rust session store.
There is no second conversation database or model router in the viewer. Native
actor/login/region/capability checks prevent applying stale asynchronous results.
Replay is bounded and reads back off on interruption; sent messages and uncertain
world operations are never automatically retried. Only one newly requested user
action may wait behind a read.

**Size** toggles compact/expanded height. Minimize, restore, close, move and the
existing `Ctrl+Alt+C` shortcut remain available. Closed or minimized windows do
not poll the shared session. Unchanged replay does not rebuild text or the
artifact chooser; reading older text retains scroll position. Login changes
clear shared data and queued input.

**Inspect source** opens a bounded original geographic preview. Questions about
the selected artifact include its original source receipt and content hash;
full source data stays in the authorized Earth Web/CLI view. `/earth` opens the
fixed local development console without putting a credential in its URL.

The local acceptance used a real local model, the same CLI/Web/native session,
two independent native accounts, source inspection and approved exhibition
creation/Undo. The macOS arm64 build and 119 CTest targets passed; presentation
coverage includes 20 tests. Shared-session Lumi, broader performance and
cross-platform accessibility gates remain open. The existing Lumi recipient is
not a claim that Lumi has joined this new shared session.

The canonical [architecture, operator guide and release ledger](https://github.com/finalverse/finalverse-earth/tree/codex/confluence-v07/docs/finalverse/v07)
distinguish verified local behavior from public release readiness.
