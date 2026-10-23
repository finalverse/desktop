# FinalStorm native viewport (source preview)

The existing Desktop can host FinalStorm in a floater opened from Explore →
FinalStorm preview, the action finder, `/storm`, or Command–Option–F on macOS. Native world login, UDP/capability networking, object cache,
camera controls, interaction, conversation UI and existing world renderer remain
in place. The bridge renders a copy of current-session decoded native geometry,
not another independently authenticated user's snapshot.

Build FinalStorm's `runtime` package `finalstorm-desktop`, then launch this
Desktop source build with `FINALSTORM_DESKTOP_LIBRARY` set to the absolute path
of `libfinalstorm_desktop.dylib` (macOS) or `libfinalstorm_desktop.so` (Linux).
Only an explicit regular file with ABI version 2 is loaded. This is a developer
preview, not a signed plugin distribution mechanism. Do not load untrusted code.
Windows has a clear unavailable state; no Windows embedding acceptance is claimed.

The exact header is vendored in `indra/newview/finalstorm/finalstorm_desktop.h`.
Producer implementation: `finalverse/finalstorm` at
`5d42b10628d5102436c75731f507bfa0d811d61b`. The ABI 2 header SHA-256 is
`a57028de5ab73da3f8fdbb7421b5e6f1b8fee8440a9a132a5f15c0429dae03bb`.
Use the package's pinned release profile: stripping this cdylib with the pinned
Rust 1.92 toolchain can produce an unaligned Mach-O string table on macOS 27.
FinalStorm's macOS CI loads the actual library to catch that failure.
The C ABI handle and GPU live entirely on one worker
thread. The UI thread exports bounded decoded `LLVolumeFace` positions, normals,
UVs, indices, region-space linkset transforms and per-face color, accepts only
the most recent returned frame, and uploads pixels to a power-of-two OpenGL
texture with an exact cropped UV rectangle. It never waits for network/asset
fetches inside the renderer. The request queue has one replaceable slot.

The input limits are 8 MiB, 256 objects, 1,024 faces, 65,536 vertices and 196,608
indices. Excess objects/faces are omitted from this preview; it is not a complete
world renderer. Invalid frames clear the preview. Closing it, logging out or
changing the native session/region destroys the renderer and clears the texture
before another session's pixels can be shown. Native geometry cache keys include
the complete mesh content, so transform/color updates reuse buffers and changed
geometry cannot reuse stale buffers. Each object retains its MutSea UUID.

The viewport follows the native camera. Use the existing movement/camera tools;
Return to world closes it. Conversation opens the existing UI. No world write,
policy decision, capability token or login credential is sent through the ABI.
The bridge also copies up to 16 decoded native base-color images, at most
128×128 RGBA8 each. The existing async decoder supplies small retained raw
levels; capture never waits for network or reads back the native GL texture.
Unavailable images keep the per-face color. Native default texture mapping
supplies offsets, repeats, rotation and animation. Media, planar mapping and
PBR surfaces retain native fallback. Requests are bounded to 1 MiB of decoded
images and 8 MiB including geometry and hex serialization. Changed decoded
content changes the renderer's texture cache identity.

Full PBR, skinned avatars, HUDs/attachments, picking and SpatialDB
static composition are not implemented here. Native decoding and existing
renderer remain the fallback for those features. Earth handoff and complete
Living World acceptance remain separate gates.

## Validation

Build the actual `secondlife-bin` target and exercise a real native login. A
standalone library test cannot pass application embedding. Record the source,
binary/library hashes, native object/triangle/pixel counts and hardware backend,
plus actual UI capture. Verify refresh, close/reopen and logout clearing. Keep
private accounts, session files and capability addresses outside source control.

On October 23 ABI 1's actual arm64 Desktop at `8183487f2176c2c0821388182471121867b80f03`
rendered 78 native mesh objects, including 68 linked children, on Apple M1 Metal
with software rendering disabled. GPU-visible object IDs matched mesh IDs read
by an independently authenticated native SDK observer in the same region. Live
native camera refresh, close/reopen, normal logout with the viewport open, and
a fresh login/reopened renderer were observed. All 119 native CTest targets
passed; the action catalog regression also passed after the shortcut change.
The canonical platform repository retains the bounded acceptance and UI captures
in `integration/evidence/desktop-acceptance-oct23.json`. This does not accept
session expiry, revocation, two Desktop views, procedural primitives or the full
Living World journey.

The remote build was rejected before starting because of the GitHub account's
billing/spending limit. The inherited CLA workflow also requires upstream
Second Life signature-store access. Neither is a successful build or legal
agreement; keep the PR draft until the repository owner resolves those gates.

This change does not make a new public package or change production services.
