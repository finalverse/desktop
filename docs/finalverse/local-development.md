# Local development and account management

The active Phase 4B candidate preserves the Phase 4A facade and Phase 3 persistent-citizen services. MutSea remains the spatial platform/runtime brand. The original source checkouts retain unrelated work; build the candidate worktrees for the current feature. Earlier Phase 2/3 instructions below describe preserved fixtures, not the current Phase 4B instance.

| Component | Local source | Branch |
|---|---|---|
| Finalverse viewer | `~/Finalverse/dev/worktrees/viewer-ai-world` | `codex/phase4b-destinations-import` |
| MutSea with world operations | `~/Finalverse/dev/worktrees/mutsea-worldops` | `codex/phase4b-destinations-import` |
| AI planner gateway | `~/Finalverse/services/ai-gateway` | `codex/persistent-ai-citizens` |
| Agent Runtime | `~/Finalverse/services/agent-runtime` | `main` (independent new service) |
| Finalverse Studio authoring seed | `~/Finalverse/studio` | `main` (no remote) |

## Current isolated Phase 4B test

The running loopback grid is `http://127.0.0.1:18094/`, alias `MutSeaCitizens`, container `mutsea-finalverse-phase4b-pg-20261009`. Its native persistent services now use a dedicated PostgreSQL 16.14 container, `mutsea-phase4b-postgres-20261009`, on a private network with no published database port. The SQLite predecessor `mutsea-finalverse-phase4b-20261008` remains stopped and intact for rollback. Use the ordinary account **MutSea FirstLook**; its private credential file is `~/Finalverse/dev/phase4b/firstlook-account.json`. Its isolated test password was rotated on October 9; use this current file rather than an older receipt/config. The Phase 3 Developer account remains the citizen controller; simultaneous logins must use different accounts. PostgreSQL account **MutSea PgProvision** is a separate appearance observer, with its private file under `~/Finalverse/dev/phase4b/postgresql/`.

The earlier published starter-library branches were fast-forwarded to `main` and pushed: viewer `de1a355f44`, MutSea `f28e7bff67`. Current Phase 4B work proceeds on the candidate branches above. Native PostgreSQL configuration, migration/backup limits and rollback are documented in the server's `docs/mutsea/postgresql.md`. The separate agent-runtime memory database and Finalverse journals are independent stores; this cutover does not make all state one SQL transaction.

The packaged candidate is `~/Finalverse/dev/phase4b/Finalverse FirstLook.app`, profile `~/Finalverse/dev/phase4b/profile-fresh-retest`. Launch after closing an existing candidate:

```sh
open --env CFFIXED_USER_HOME="$HOME/Finalverse/dev/phase4b/profile-fresh-retest" \
  "$HOME/Finalverse/dev/phase4b/Finalverse FirstLook.app" --args \
  --grid MutSeaCitizens --set AutoLogin false \
  --set RememberPassword false --set RememberUser false
```

Enter the password from the private account file. Development acceptance also uses a private one-session settings file with `--sessionsettings`; do not copy it into Git or distribute it, and do not substitute the persistent `--settings` flag. Avoid `secondlife://` start links when automatically logging into MutSea: that scheme can select Linden's built-in grid despite `--grid`. Select the known loopback grid and enter its world first, or use a verified grid-specific location scheme. The historical Phase 4A app remains intact. Appearance/save findings and limits are in [avatar-appearance-audit.md](art/avatar-appearance-audit.md).

Current original procedural Starter Library packages are under `~/Finalverse/studio/exports/starter-v0.1`: pavilion, bench, coastal tree, beacon and lantern. They have Blender sources, glTF/Collada payloads, semantic/hash/budget manifests and pass the offline validator. The lantern now passed an approved native mesh import, semantics, live viewer rendering, server restart and guarded Undo, including a new import/restart/Undo on PostgreSQL. This is a bounded first importer; broader Harbor import, useful LOD/collision refinement, rigged avatars and public provisioning remain pending.

Recommended gateway repository name: **`ai-gateway`**, under the Finalverse organization (`finalverse/ai-gateway`). It currently has no remote; no GitHub repository or push is claimed. The existing viewer repository remains `finalverse/final_viewer`.

Original checkouts: `~/Finalverse/viewer` and `~/Finalverse/classic/mutsea-o`. Do not reset or build over the original viewer's unrelated modifications. Deferred Linux work is in `~/Finalverse/dev/worktrees/viewer-branding`.

## Build the macOS viewer

The original Phase 2 app was Intel x86-64 through Rosetta. The synchronized source now configures a universal ARM64/Intel build; see [upstream-sync-20261008.md](upstream-sync-20261008.md) for fresh verification rather than assuming the earlier app proves this revision. Use Xcode's Clang, the pinned Python dependency lock and the preserved build-variable file. These commands create a durable tooling environment outside Git:

```sh
cd "$HOME/Finalverse/dev/worktrees/viewer-ai-world"
python3.13 -m venv "$HOME/Finalverse/dev/tooling/viewer-venv"
"$HOME/Finalverse/dev/tooling/viewer-venv/bin/pip" install -r docs/finalverse/toolchain-python.lock
export PATH="$HOME/Finalverse/dev/tooling/viewer-venv/bin:$PATH"
export AUTOBUILD_VARIABLES_FILE="$HOME/Finalverse/dev/baseline-macos/tooling/variables"
export AUTOBUILD_CPU_COUNT=2 AUTOBUILD_ADDRSIZE=64 AUTOBUILD_BUILD_ID=262810010
export PYTHON="$HOME/Finalverse/dev/tooling/viewer-venv/bin/python"
export CC=/usr/bin/clang CXX=/usr/bin/clang++
autobuild configure -c RelWithDebInfoOS -- \
  -DLL_TESTS=ON -DUSE_OPENAL=ON -DUSE_VELOPACK=OFF \
  -DPython3_EXECUTABLE="$PYTHON" \
  -DCMAKE_OSX_SYSROOT="$(xcrun --show-sdk-path)" \
  -DFINALVERSE_BUNDLE_ID=com.finalverse.viewer.experience \
  -DVIEWER_CHANNEL="Finalverse Test"
autobuild build -c RelWithDebInfoOS --no-configure
autobuild build -c RelWithDebInfoOS --no-configure -- --target BUILD_TESTS
ctest --test-dir build-darwin-universal -C RelWithDebInfo -j4 --timeout 120 --output-on-failure
```

For an already configured tree, use the environment above and only the final build command. Upstream now chooses universal architectures and determines the SDK's deployment floor. See [upstream-baseline.md](upstream-baseline.md) for the historical Intel baseline and [upstream-sync-20261008.md](upstream-sync-20261008.md) for the current configuration.

Current build output: `build-darwin-universal/newview/RelWithDebInfo/Finalverse Test.app`. The previously tested copy remains `~/Finalverse/dev/phase2/Finalverse AI.app`, with its own identifier and profile. Close an existing test viewer before relaunching that prior app; its isolated profile already contains the local grid entry:

The Phase 2 bundle uses `com.finalverse.viewer.phase2` to distinguish it from the regular client in macOS app selection. `FINALVERSE_BUNDLE_ID` defaults to the regular client's `com.finalverse.viewer`; the profile still requires the explicit isolated path below. Keep one registered copy of each development identifier when using app automation.

```sh
CFFIXED_USER_HOME="$HOME/Finalverse/dev/phase2/profile" \
  "$HOME/Finalverse/dev/phase2/Finalverse AI.app/Contents/MacOS/Finalverse Test" \
  --grid MutSeaHarborAI --set AutoLogin false
```

## Validate the Mac package

From the viewer source directory, use the same Python environment:

```sh
"$PYTHON" scripts/finalverse/verify_branding.py \
  --bundle "build-darwin-universal/newview/RelWithDebInfo/Finalverse Test.app" \
  --bundle-id com.finalverse.viewer.experience
PYTHONPATH=indra/lib/python "$PYTHON" -m unittest discover \
  -s indra/test -p test_llmanifest.py -v
```

The branding check compares Info.plist with build metadata, supplied artwork hashes, notices, updater isolation and platform packaging rules. A successful compile alone does not verify the UI.

## Build MutSea

Docker Desktop is the verified native ARM64 server path on this Mac. Build the existing runtime, then the separate module against the same runtime assemblies:

```sh
cd "$HOME/Finalverse/dev/worktrees/mutsea-worldops"
docker build -f deploy/standalone/Dockerfile -t mutsea:verify .
docker build -f deploy/worldops/Dockerfile \
  --build-arg MUTSEA_IMAGE=mutsea:verify -t mutsea:worldops-verify .
```

The current add-on source runs 51 world/navigation tests, including the original Phase 2 kernel suite. Building these images does not replace the running fixture. Do not start two simulators against the same database/volume. Configuration and opt-in module instructions are in the server's `Finalverse/World/README.md`; standalone deployment instructions are in `deploy/standalone/`.

On this development machine the existing isolated fixture is `mutsea-finalverse-phase2`. Its read-only configuration is `~/Finalverse/dev/phase2/harbor-config`, persistent data is Docker volume `finalverse-phase2-data`, and its module is `~/Finalverse/dev/phase2/server-module/Finalverse.World.dll`. The original Harbor starter-scene volume is mounted read-only. These are development resources outside Git. To start a stopped fixture and check the grid:

```sh
docker start mutsea-finalverse-phase2
curl --noproxy '*' --fail --silent http://127.0.0.1:18092/get_grid_info
```

Wait for MutSea's console to report startup completion before logging in. To stop normally, attach as described below, enter `shutdown`, and wait for the container to exit. Avoid forced termination during world writes. Restart using `docker start`; never remove its volume to simulate a restart.

For a **fresh separate** installation, first provision PostgreSQL and a private database JSON. `deploy/standalone/configure.py --output /private/new-config --host 127.0.0.1 --port <unused-port> --database-config /private/database.json` creates private standalone files and a generated account. It refuses nonempty output and defaults to PostgreSQL without silent SQLite fallback. The explicit `--database sqlite` option remains for independent legacy fixtures. Enable `[FinalverseWorld]` as the module README describes and use independent data storage. The checked-in `deploy/standalone/compose.yaml` plus PostgreSQL network overlay is the TLS/LAN deployment template, not the loopback fixture; do not run it unchanged for this local test.

## Run the gateway

Ollama with `llama3.2:3b` is already installed on the development Mac. The gateway only proposes plans; MutSea performs authentication, policy validation and approved execution.

```sh
cd "$HOME/Finalverse/services/ai-gateway"
python3 -m venv "$HOME/Finalverse/dev/tooling/gateway-venv"
"$HOME/Finalverse/dev/tooling/gateway-venv/bin/pip" install -r requirements.txt
"$HOME/Finalverse/dev/tooling/gateway-venv/bin/python" -m unittest -v
# Run only if another gateway is not already listening on 18765:
FINALVERSE_MODEL=llama3.2:3b \
  "$HOME/Finalverse/dev/tooling/gateway-venv/bin/python" gateway.py
```

See the gateway README for the sample `examples/inspect-request.json` planning request and its expected null plan. The sample uses an empty synthetic scene; actual world context comes from the viewer capability. Python 3.13 is verified. `curl --noproxy '*' -fsS http://127.0.0.1:18765/health` reports the running provider/model. Ollama listens on loopback port 11434. No cloud API key is needed for this slice.

## Login to the local world

The AI fixture's grid is **MutSeaHarborAI**, login URI **`http://127.0.0.1:18092/`**, region **MutSea Harbor**. Existing generated development account: first name **MutSea**, last name **Developer**. Obtain its password locally from `~/Finalverse/dev/phase2/harbor-config/account.json`; the credential file is outside Git and its values are intentionally absent from this guide. Enter `MutSea Developer` in the viewer username field.

The earlier Harbor fixture uses loopback port 18091 and is separate. Localhost refers to this development machine; neither URL is a public registration service. A second login using the same account can displace the first session. Use separate accounts for simultaneous testing. The gateway is not a user-registration or identity service.

To reproduce the isolated grid entry, merge this record into the LLSD map in the profile's `Library/Application Support/Finalverse/user_settings/grids.xml` while the viewer is closed. Preserve other entries. The XML key and `keyname` must be the endpoint authority, and `grid_login_id` is the command-line alias:

```xml
<key>127.0.0.1:18092</key><map>
  <key>keyname</key><string>127.0.0.1:18092</string>
  <key>grid_login_id</key><string>MutSeaHarborAI</string>
  <key>label</key><string>MutSea Harbor AI</string>
  <key>login_uri</key><array><string>http://127.0.0.1:18092/</string></array>
  <key>helper_uri</key><string>http://127.0.0.1:18092/</string>
  <key>login_identifier_types</key><array><string>agent</string></array>
</map>
```

Never use an unknown grid alias for automatic login. The viewer's regression guard refuses it rather than sending credentials to a fallback grid. A plain double-click does not select the isolated profile; use the launch command above.

## Register and manage users

The standalone server exposes inherited account commands on its operator console. Attach to the owned AI fixture:

```sh
docker attach --sig-proxy=false mutsea-finalverse-phase2
```

Press **Ctrl-P, then Ctrl-Q** to detach and leave MutSea running. In the MutSea console, use interactive commands; do not put passwords in command arguments, shell history or transcripts:

```text
create user
show account Firstname Lastname
reset user password Firstname Lastname
login status
```

`create user` prompts for first name, last name, a hidden password, email, optional UUID and optional avatar model. Leave UUID blank for a generated one; model may be blank. `reset user password` also prompts without echo. `show account` displays account details and user level, not the password.

For maintenance, `login disable`/`login enable` control region admission; `login level <number>` changes the service's minimum account level. `login reset` restores configured `MinLoginLevel` (default zero), rather than unconditionally admitting every account. `set user level Firstname Lastname <number>` changes an account's level; levels at least 200 may confer grid-god authority when enabled. Ordinary testing needs no elevation. Console access is operator authority; no public self-service registration UI is deployed.

Source evidence: `MutSea/Services/UserAccountService/UserAccountService.cs` registers account commands and hidden prompts; `MutSea/Services/LLLoginService/LLLoginService.cs` enforces minimum level; `MutSea/Region/CoreModules/World/Access/AccessModule.cs` controls region admission. These are wrappers around the existing identity/login system, not a replacement implementation.

## Try the AI slice

Open **Finalverse → Ask / Create** (Command-Shift-K). Ask “What objects are around me?” Select an owned practice object using the inherited Edit tool, then request a supported change. **Plan** reads the authoritative world and prepares a bounded proposal. Review it before **Build / Apply**. **History** shows recorded operations; **Undo last** performs guarded restoration when the objects still match the recorded result. After reopening the floater, select **Nearby** to refresh the authoritative undoable plan; **History** alone does not refresh it.

Supported creation is currently box/sphere primitives and a deterministic 17-component Lumi home. A general request such as “a table please” is not a validated furniture-generation workflow. This remains a development slice; the synchronized universal viewer now runs natively on Apple Silicon. Public deployment, avatar appearance, Intel runtime and other platform clients have separate gates.

For the Harbor home test, use the World Map to teleport to `(172,156,27)`, then wait for the avatar to settle. The earlier site near `(172,153.45)` is correctly refused by conservative lake-path clearance. Inspect the actual preview location and 17 components before applying. After building, view from south of the home, for example `(172,146,27)`. Do not change placement policy to force an invalid site to pass.


## Capture and verify a viewer-created home

Log out of the viewer before using the same account in the protocol probe. `capture` stores the existing UI-created operation and does not prepare/apply an edit. Receipts remain private. The current machine has a flattened protocol SDK at `~/Finalverse/dev/protocol-smoke-net8`:

```sh
cd "$HOME/Finalverse/dev/worktrees/mutsea-worldops"
dotnet build tools/Finalverse.WorldSmoke -c Release \
  -p:MutSeaBin="$HOME/Finalverse/dev/protocol-smoke-net8" \
  -o "$HOME/Finalverse/dev/phase2/world-smoke"
DOTNET_ROLL_FORWARD=Major dotnet \
  "$HOME/Finalverse/dev/phase2/world-smoke/Finalverse.WorldSmoke.dll" \
  "$HOME/Finalverse/dev/phase2/harbor-config/account.json" \
  "$HOME/Finalverse/dev/phase2/my-ui-receipt.json" capture
```

Use `verify` with that same receipt after normal simulator shutdown/restart. After viewer composite Undo, use `verify_undo` after another restart. They check the original component IDs, full transforms, semantic relationships and retained history. `capture` refuses to overwrite a receipt. `DOTNET_ROLL_FORWARD=Major` is only needed when this Mac's newer host SDK runs the .NET 8-targeted probe; the deployed server remains native .NET 8. For a fresh protocol SDK build, follow `tools/MutSea.ProtocolSmoke/build.py` and use its actual output assembly directory.


## Current Phase 3 citizen sandbox

The new service source is `~/Finalverse/services/agent-runtime`; intended repository name **finalverse/agent-runtime**. It has no remote. Its source-matched README and `docs/finalverse/agents/` describe build, private configuration, restart and bounded tool behavior. Gateway now also supplies read-only citizen conversation; its model has no simulator authority.

Current independent sandbox: grid alias **MutSeaCitizens**, login URI **http://127.0.0.1:18094/**, region **MutSea Harbor**, account **MutSea Developer**. Read its password locally from `~/Finalverse/dev/phase3/config/account.json`; do not reuse Phase 2 credentials for this separate world. Account creation/inspection/password management still uses the inherited simulator console described above. There is no new public signup endpoint.

The verified container is `mutsea-finalverse-citizens-phase3-v4-20261008`; its independent persistent data volume is `finalverse-phase3-data-20261008`. Keep Phase 2 containers and data unchanged. Start a stopped sandbox with `docker start` and stop normally with console `shutdown`. Both TCP and UDP are loopback 18094. The runtime/gateway listen on loopback 18766/18765, respectively.

Current native universal package: `~/Finalverse/dev/phase3/Finalverse Citizens.app`, identifier `com.finalverse.viewer.citizens`, separate profile `~/Finalverse/dev/phase3/profile`. Do not register backup `.app` copies with the same identifier. Start the viewer with an explicit profile and grid:

```sh
CFFIXED_USER_HOME="$HOME/Finalverse/dev/phase3/profile" \
  "$HOME/Finalverse/dev/phase3/Finalverse Citizens.app/Contents/MacOS/Finalverse Test" \
  --grid MutSeaCitizens --set AutoLogin false
```

A fresh profile needs the same LLSD grid record format shown above with authority/login URI `127.0.0.1:18094` and `grid_login_id` `MutSeaCitizens`. Existing private profile already has it. Open AI, enable **Talk to Lumi**, ask identity/home/memory questions and **Lumi, go home**. Inspector, Pause/Resume/Cancel and separately approved garden controls are in the same floater. See [phase3-persistent-citizens.md](phase3-persistent-citizens.md).

The Python lock requires modern Python; on this Mac use Python 3.13.6 (`~/.pyenv/versions/3.13.6/bin/python`) explicitly if `python3.13` is unavailable. Xcode's bundled Python 3.9 cannot install this lock. The durable environment at `~/Finalverse/dev/tooling/viewer-venv` was created with that interpreter and used for the Phase 3 build/tests, replacing the temporary Phase 0 environment.


## Current Phase 4A macOS experience

Viewer source: `/Users/wenyan/Finalverse/dev/worktrees/viewer-ai-world`, branch `codex/experience-facelift`. MutSea source: `/Users/wenyan/Finalverse/dev/worktrees/mutsea-worldops`, branch `codex/persistent-ai-citizens`. Gateway source: `/Users/wenyan/Finalverse/services/ai-gateway`. Runtime source: `/Users/wenyan/Finalverse/services/agent-runtime`. The services, account and MutSeaCitizens grid above are unchanged. MutSea retains its server brand.

Current tested bundle: `/Users/wenyan/Finalverse/dev/phase4a/Finalverse Experience.app`, identifier `com.finalverse.viewer.experience`. Its separate profile is `/Users/wenyan/Finalverse/dev/phase4a/profile`. The preserved Phase 3 Citizens app remains available. Close an existing viewer before launching another copy with the same test account. A plain app double-click does not select this isolated profile.

```sh
open --env CFFIXED_USER_HOME="$HOME/Finalverse/dev/phase4a/profile" \
  "$HOME/Finalverse/dev/phase4a/Finalverse Experience.app" --args \
  --grid MutSeaCitizens \
  --sessionsettings "$HOME/Finalverse/dev/phase4a/session-settings.xml" \
  --set AutoLogin false --set UIScaleFactor 1.0
```

A private local convenience launcher is `~/Finalverse/dev/phase4a/run-experience.sh`. Login is **MutSea Developer** on **http://127.0.0.1:18094/**; read the password locally from `~/Finalverse/dev/phase3/config/account.json`. Manage users through `docker attach --sig-proxy=false mutsea-finalverse-citizens-phase3-v4-20261008` using the inherited interactive `create user`, `show account` and `reset user password` commands described above. There is no public signup page.

Start here teaches movement and directs the user to Lumi, Create and Friends. My Stuff is under Me. Select retains an object and exposes Ask AI/Edit/Clear. Plan → review → Apply → History/Undo is the approved world-edit path. Tools → Full interface restores advanced controls; Back to Finalverse returns. The supported recipes are home/garden and bounded primitive operations. Avatar/scene asset quality, unrestricted generation and public multi-session release gates remain open. See [Phase 4A validation](ux/phase4a-validation.md) for the actual build, test, screenshots and performance evidence.
