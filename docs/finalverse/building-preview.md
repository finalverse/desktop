# Build and package a Mac preview

The public preview must come from a clean, pinned source commit. Do not package
a private acceptance app, developer profile or session-settings file.

## Toolchain

Use Xcode command-line tools, CMake and Python 3.13. Accept the Xcode license
locally if Xcode asks for it. The pinned Python packages are in
`docs/finalverse/toolchain-python.lock`; upstream dependency versions, hashes and
public archive URLs are in `autobuild.xml`.

```sh
git clone https://github.com/finalverse/final_viewer.git
cd final_viewer
git checkout v0.4.0-preview.3
python3.13 -m venv .venv
.venv/bin/pip install -r docs/finalverse/toolchain-python.lock
export PATH="$PWD/.venv/bin:$PATH"
export PYTHON="$PWD/.venv/bin/python"
export CC=/usr/bin/clang CXX=/usr/bin/clang++
export AUTOBUILD_CPU_COUNT=2 AUTOBUILD_ADDRSIZE=64 AUTOBUILD_BUILD_ID=54622
```

Use the release's `Finalverse-0.4.0-preview.3-build-variables.txt` asset as
`AUTOBUILD_VARIABLES_FILE`; its SHA-256 is recorded in provenance. This retains
the exact inherited compiler flag input instead of assuming an unrecorded
upstream Git revision. The same file is bundled under
`Contents/Resources/FinalverseDistribution/build-variables`. Do not substitute
an operator's private config. Dependency versions and hashes remain pinned in
`autobuild.xml`.

```sh
export AUTOBUILD_VARIABLES_FILE=/absolute/path/to/build-variables/variables
autobuild configure -c RelWithDebInfoOS -- \
  -DLL_TESTS=ON -DUSE_OPENAL=ON -DUSE_VELOPACK=OFF \
  -DINSTALL_PROPRIETARY=OFF -DUSE_BUGSPLAT=OFF \
  -DPython3_EXECUTABLE="$PYTHON" \
  -DCMAKE_OSX_SYSROOT="$(xcrun --show-sdk-path)" \
  -DFINALVERSE_BUNDLE_ID=com.finalverse.viewer.preview \
  -DVIEWER_CHANNEL="Finalverse Preview"
autobuild build -c RelWithDebInfoOS --no-configure
cmake --build build-darwin-universal --config RelWithDebInfo \
  --target BUILD_TESTS finalverse_resource_probe --parallel 2
ctest --test-dir build-darwin-universal -C RelWithDebInfo \
  -j2 --timeout 120 --output-on-failure
```

The configuration retains the inherited universal architecture/deployment-floor
logic. Apple Silicon runtime verification does not certify Intel hardware. Linux
and Windows have retained upstream build paths but no accepted binary in this
preview.

## Distribution contract

The release packager must copy the built app into a fresh staging directory,
install source/license/third-party notices and exact build provenance, scan for
private profiles, test settings and credentials, then sign and verify the
**staged** app. An ad hoc signature is an integrity check and is not Developer ID
signing or notarization. The release must state this clearly.

Package the app with installation instructions and an Applications link in a
read-only DMG. Mount and verify that exact DMG, run a fresh-profile launch check,
and generate SHA-256 checksums. Export corresponding source with `git archive`
from the pinned commit, including required submodule/vendor material where used;
never archive a working directory with private files. Include the dependency
inventory and license texts from the actual build. Retain original copyrights
and the LGPL source/build instructions.

Publish assets to a GitHub **prerelease**, not a GA release. Upload the installer,
source/provenance, checksums and real demo media. Verify the anonymous download
URL before updating either website. Main/master source integration is separate
from binary acceptance. A successful compile alone does not authorize claiming
that an artifact was installed, notarized or exercised on another platform.

## Reproduce the release transformation

Run `scripts/finalverse/package_preview.py --help` with the locked Python
environment. It requires a clean source tree, the rebuilt preview bundle,
installed dependency directory, exact build-variable file, pinned dependency
source archive and a fresh output directory. It records the source commit,
checks universal architectures and nonportable links, refuses private/debug
files and automatic login, carries notices, signs ad hoc, exports source and
creates/verifies the DMG. It never reads a private account or session file.

The staged first preview excludes optional libVLC libraries, codec plugins and
its media plugin because that redistribution source inventory is not accepted.
Voice defaults off. These distribution changes are recorded in provenance and
release notes; the full source/development implementation remains intact. The
LGPL/MPL source archive contains exact OpenAL/ALUT, Hunspell and Dullahan
revisions, including recursive submodules, plus its source manifest. The native
build dependency inventory names all installed packages; it is explicitly a
build inventory, not a claim that every package is shipped.

Generate the short demo recap with macOS AVFoundation:

```sh
swift scripts/finalverse/demo_recap.swift \
  docs/finalverse/media/hosted-lumi-built.png \
  docs/finalverse/media/hosted-lumi-undone.png /absolute/new/recap.mp4
```

It labels two authentic captures as a still-image recap and adds no synthetic
world behavior. The WebVTT source is alongside the captures.


## Protected resource reads (preview.2 and later)

The native `finalverse_resource_probe` acceptance tool uses the viewer's actual
`LLFile` reader to read the required settings, strings, colors and CA bundle. Run
it against a staged bundle and a mounted read-only image; the packager now
requires `--resource-probe /absolute/build/llcommon/RelWithDebInfo/finalverse_resource_probe`
and performs those checks itself, including XML parsing and identical resource
hashes after mounting. A successful Python read or a writable-copy launch alone
is insufficient to accept a package.

On October 16 the old reader was also reproduced against a real macOS
AppTranslocation nullfs mount: all eight reads failed with EROFS before the fix
and all eight passed afterward. This protected mount differs from an ordinary
read-only APFS disk image; both cases belong in release acceptance. The native
probe is a test tool and is not bundled into the client. Never remove quarantine
or weaken Gatekeeper to make a test pass.

With a clean committed source tree and a rebuilt bundle, package with absolute
paths (the output directory must not already exist):

```sh
python scripts/finalverse/package_preview.py \
  --version 0.4.0-preview.3 \
  --bundle "$PWD/build-darwin-universal/newview/RelWithDebInfo/Finalverse Preview.app" \
  --packages "$PWD/build-darwin-universal/packages" \
  --build-variables "$AUTOBUILD_VARIABLES_FILE" \
  --third-party-source /absolute/Finalverse-0.4.0-preview.3-third-party-source.tar.gz \
  --resource-probe "$PWD/build-darwin-universal/llcommon/RelWithDebInfo/finalverse_resource_probe" \
  --output /absolute/fresh-preview-output
```

The dependency source archive may be reused only when its pinned dependency
versions are unchanged. The packager records its hash. Keep the unit-test log,
protected-mount probe results and actual installed-app startup evidence with the
release verification receipt; packaging success does not substitute for launch.
