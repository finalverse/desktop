# Getting started with the Mac developer preview

Finalverse is an experimental desktop client for persistent worlds powered by
MutSea. This preview supports limited MutSea registration. Downloading the client does **not** create an account. Create your own starter avatar and home through the account portal. Shared public Lumi ownership and GA support are not available.

## 1. Download from GitHub

Open the [v0.4.0-preview.3 release](https://github.com/finalverse/final_viewer/releases/tag/v0.4.0-preview.3).
Choose `Finalverse-0.4.0-preview.3-mac-universal.dmg` and `SHA256SUMS.txt` from
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
shasum -a 256 Finalverse-0.4.0-preview.3-mac-universal.dmg
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

## 3. Create an account and enter the world

Open [MutSea registration](https://mutsea.com/account/#register). Choose your
first and last avatar name, a starter character and a 12–16 character printable
ASCII password. Email is optional and unverified. Wait for **Account ready**:
the native runtime prepares your appearance, outfit, inventory and persistent
home before activating your account. Registration pauses if preview capacity is
full or the world worker is unavailable; existing users can still sign in.

Use **First Last**, separated by a space, and the password you chose. An avatar
UUID or email is not your viewer login. Do not use the private account shown in
demos. Select **Finalverse Harbor (Preview)**, `https://grid.mutsea.com/`, before
entering credentials. The grid picker is visible in preview.3. Existing profiles
retain their selected grid and saved credentials; check that Second Life is not
selected when signing into MutSea. Different grids have separate accounts.

Sign up and Need help logging in on the MutSea grid open its real registration
and recovery pages. Preview.2 users can reveal the picker with **⌘⇧G** and should
register through the link above instead of its inherited Sign up button.

Manage your account at [MutSea account management](https://mutsea.com/account/):
view your home/identity, edit a web profile, change your native world password,
and generate recovery codes. Save these in a password manager before you need
them. A reset invalidates the whole previous set and all web sessions. Email
reset and MFA are not available. [Account and recovery guide](https://github.com/finalverse/mutsea-o/blob/main/docs/mutsea/account-portal.md).

If login is unavailable, check the service and account state; do not alter TLS,
firewall or router protections to bypass an error. Contact support without
sending passwords, recovery codes or capability URLs.

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
The current hosted pilot has one configured controller; a normal registered account
must not assume it owns that citizen. Unavailable controls reflect capabilities
and permissions rather than a requirement to run local developer services.

Save a recipe before closing a preview if you want to load or remix it later.
Recipes do not confer build rights. WorldLine and supported Undo are guarded
compensation, not unlimited restoration of arbitrary changes by other users.

## 5. Troubleshooting and feedback

| Symptom | First check |
|---|---|
| macOS blocks opening | Verify the GitHub asset/hash and read the signing limitation above. |
| Missing Files / cannot load `settings_files.xml` in preview.1 | Install preview.2 or later. It fixes input-only reads on macOS's protected AppTranslocation mount. Replace the app; keep your profile. |
| Login refused or 503 | Confirm Account ready, First Last name, selected grid and service availability. |
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
and the [release boundary](releases/0.4.0-preview.3.md).
