# macOS GUI acceptance and automation

Use unit tests, protocol/API probes, authoritative persistence checks and filtered viewer logs before spending time on screenshots. GUI checks prove rendered appearance and actual interaction; a successful API call does not prove a visible scene change.

The inherited LLUI runs inside one native macOS window. In the October 2026 development fixture, the native window, close/minimize/full-screen controls and application menus expose accessibility elements. Most in-world menus, toolbar buttons, floaters, color controls and inventory rows do not expose individual native accessibility elements. Their XUI `name`, `label` and `tool_tip` identify controls to the viewer's own widget registry, but do **not** automatically create macOS AX identifiers. Finalverse's `destination_fallback`, `explore_current_world` and `discover_*` controls have stable widget names and explicit labels/tooltips. A complete Cocoa/LLUI accessibility bridge remains separate work.

## Reliable paths

- Keyboard: Command-Q followed by the visible Quit confirmation performs normal logout. Command-W closes the focused floater. Control-Shift-1 opens native viewer Statistics. Command-Shift-S opens Snapshot. Confirm the current screenshot before acting on dialogs.
- Native menus/window controls: use current accessibility indices, never cached indices from a previous observation.
- Source/control harness: unit-test grid URL policy independently of CEF; protocol-test World capabilities and UDP scene data. The existing command registry can invoke facade commands in a dedicated harness. No arbitrary public command-execution endpoint is added.
- Visual only: avatar bake appearance, outfit editor/color swatch, scene mesh/material appearance and camera composition. One screenshot can establish a state; do not repeat unchanged clicks indefinitely.

The automation service sometimes delivered hover without activating a legacy control. A fresh screenshot and keyboard navigation were more reliable. Screen lock can suppress all windows. During compilation on the 16 GB Mac, memory pressure also slowed shell and GUI responses. These are distinct from application render performance. Record background builds when measuring FPS.

## Isolated launch

Use a dedicated profile; never share the normal client profile or log in two clients as the same avatar:

```sh
open --env CFFIXED_USER_HOME="$HOME/Finalverse/dev/phase4b/profile-fresh-retest" \
  "$HOME/Finalverse/dev/phase4b/Finalverse FirstLook.app" --args \
  --grid MutSeaCitizens --set AutoLogin false \
  --set RememberPassword false --set RememberUser false
```

A private one-session settings file may supply the already authorized test account. Its path, rather than a password, goes in arguments. Keep accounts, session settings, capability-bearing logs and receipts outside Git with mode 0600. Filter logs by event name and redact capability URLs. Use a different development account for second-client protocol checks.

Quit through the normal client path and verify LogoutReply before relaunching. A signal-driven exit can leave the simulator's online session stale; do not interpret a subsequent login refusal as an appearance regression. Restart only the owned fixture through its console and retain scene databases, kernel/content/citizen WorldLine journals and agent memory together.

## Discovery acceptance

Verify the connected MutSea grid's native cards, a locally approved remote URL, and failure/timeout fallback. Unit cases cover absent/unknown grids, false `system_grid`, malformed URLs, userinfo, unsupported schemes, origin/port confusion, HTTP outside same-origin loopback and inherited Second Life destination domains. Remote content stays untrusted and receives no destination OpenID injection. Visual acceptance must confirm that the tested packaged executable matches the build.
