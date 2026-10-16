# Getting started with the Mac developer preview

Finalverse is an experimental desktop client for persistent worlds powered by
MutSea. This preview is for developers and invited testers. Downloading the
client does **not** create an account or grant entry to the private Harbor pilot.
Public self-registration, shared public Lumi ownership and GA support are not
available.

## 1. Download from GitHub

Open the [v0.4.0-preview.1 release](https://github.com/finalverse/final_viewer/releases/tag/v0.4.0-preview.1).
Choose `Finalverse-0.4.0-preview.1-mac-universal.dmg` and `SHA256SUMS.txt` from
**Assets**. GitHub serves the installer; the world server does not host downloads.
Source and dependency/build provenance are provided alongside the installer.

The executable contains Apple Silicon and Intel architectures. Runtime
acceptance was performed on Apple Silicon; Intel hardware is not verified for
this preview. The build deployment floor is macOS 12, but that alone does not
establish compatibility with every Mac or OS release. Windows/Linux viewer
binaries and mobile apps are not included.

Verify the downloaded disk image before opening it:

```sh
cd "$HOME/Downloads"
shasum -a 256 Finalverse-0.4.0-preview.1-mac-universal.dmg
```

Compare the complete hash with the matching line in the release's
`SHA256SUMS.txt`. If it differs, stop and download again from this repository.

## 2. Install and open

1. Open the disk image and drag **Finalverse Preview.app** into Applications.
2. Eject the disk image and open the copy in Applications.
3. Read the bundled welcome page before signing in.

This developer preview has an **ad hoc signature**. It is **not Developer ID
signed or notarized**, so macOS may block it. Decide whether to trust this
specific download after checking its origin and hash. Apple's
[per-app opening instructions](https://support.apple.com/en-us/102445) explain
the system's warning and the user-controlled Privacy & Security flow. Do not
disable Gatekeeper globally or remove quarantine recursively. A managed Mac may
require its administrator's approval. A signed/notarized installer is a separate
beta gate.

The preview uses its own application identity and normal Finalverse data
directories. It contains no test account, saved password, developer profile or
cloud API key. Keep existing viewer profiles backed up before testing.

Optional libVLC/codec playback is excluded from this public package pending its
corresponding-source inventory. Voice defaults off and is not preview-certified.
The full source/development build retains both implementations; this preview
does not certify parcel audio/video or voice calls.

## 3. Obtain world access

Request an invitation at [info@finalverse.com](mailto:info@finalverse.com?subject=Finalverse%20preview%20world%20access).
An operator supplies your **own** account and an access window. Do not use the
Pilot Builder account shown in demo screenshots. Its password is not public.

Manage an existing Harbor account at [MutSea account management](https://mutsea.com/account/)
using the same avatar first/last name and password as the viewer. View your home
region, save a web profile, change your world password and generate private
recovery codes. New passwords must be 12–16 printable ASCII characters for this
preview's login field. Save codes before you need them; a reset invalidates the
entire previous set. Email reset and public signup are not yet available.
[Full account/recovery guide](https://github.com/finalverse/mutsea-o/blob/main/docs/mutsea/account-portal.md).

The intended pilot login endpoint is `https://grid.mutsea.com/`. Select
**Finalverse Harbor (Preview)** in the grid selector. A fresh profile already
selects this grid; the picker is normally hidden. The inherited Mac shortcut
**⌘⇧G** shows the grid picker. Existing profiles retain their saved choice. If your installation does
not show that grid, use the operator-supplied grid configuration; never enter
Harbor credentials into a different grid. Other compatible grids use their own
accounts and may not provide Finalverse AI capabilities.

An HTTP 503 or closed-admission message from the pilot usually means access is
gated or outside your test window. Contact the operator; do not change firewall,
TLS or router settings to bypass it. The website's email link is an invitation
enquiry, not registration.

## 4. Explore and create

After entering a world, use the arrow keys to move, **Explore** for map/camera
tools, **Friends** for social controls and **Me** for outfits and inventory.
**Tools → Full interface** retains the mature editing tools.

In a world advertising authenticated AI planning:

1. Ask **“What objects are around me?”** The client sends structured world
   context through the active authenticated world session.
2. Ask **“Create a small blue box four meters to my right.”** MutSea resolves
   your heading, terrain and permissions. This bounded primitive grammar supports
   forward/backward/left/right and 0.5–12 meters.
3. Read the preview and location. Choose **Apply** only when you want that change.
4. Use **History** to inspect the record and **Undo last** to reverse it when its
   objects still match the recorded result.

Where your account is explicitly authorized to control Lumi, use **Lumi** or
**Talk to Lumi**, then **“Lumi, create a small quiet reading nook nearby.”** Review
the proposal, choose **Build together**, and use **Undo creation** to remove that
creation. Lumi's status exposes activity and memory. Hosted model/runtime
services are server-side; users do not install a gateway or enter cloud API keys.
The current hosted pilot has one configured controller; a normal invited account
must not assume it owns that citizen. Unavailable controls reflect capabilities
and permissions rather than a requirement to run local developer services.

Save a recipe before closing a preview if you want to load or remix it later.
Recipes do not confer build rights. WorldLine and supported Undo are guarded
compensation, not unlimited restoration of arbitrary changes by other users.

## 5. Troubleshooting and feedback

| Symptom | First check |
|---|---|
| macOS blocks opening | Verify the GitHub asset/hash and read the signing limitation above. |
| Login refused or 503 | Confirm your invitation, endpoint and access window with the operator. |
| Already logged in | Close the other client and wait for its normal logout; never share one account between viewer and probes. |
| Lumi or Create unavailable | Check that this world/account advertises and authorizes the service. |
| Placement refused | Choose clear, level, dry ground within your parcel rights; do not force a blocked site. |
| Undo refused | Inspect History. Objects may have changed since the operation; do not delete unrelated objects. |

Report reproducible issues through the
[viewer issue tracker](https://github.com/finalverse/final_viewer/issues). Include
the preview tag, Mac model/architecture, macOS version, steps and a relevant
screenshot. Redact passwords, account email, capability URLs, session tokens,
private chat and unrelated desktop content. Do not attach an entire profile.

See [actual demo evidence](demos.md), [source/build instructions](building-preview.md)
and the [release boundary](releases/0.4.0-preview.1.md).
