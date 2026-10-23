# Licensing, provenance, assets and service audit

Status: initial engineering audit, not distribution clearance. Audit date: 2026-10-07.

The [2026-10-23 Firestorm evaluation](firestorm-evaluation.md) records the
owner-requested alternative review and the approved CLA workflow ownership fix.
Both foundations retain LGPL 2.1 obligations. The current open configuration
continues while actual binary/dependency distribution checks remain required.

## Source provenance

The root [LICENSE](../../LICENSE) and [doc/LGPL-license.txt](../../doc/LGPL-license.txt) contain LGPL 2.1. [doc/LICENSE-source.txt](../../doc/LICENSE-source.txt) describes inherited Linden code, while source headers such as [llviewerobject.h](../../indra/newview/llviewerobject.h) specify LGPL version 2.1 only. Preserve these headers, contributor notices and file-specific licenses. Do not relabel inherited C++ as newly authored Finalverse code or apply a blanket permissive license to the fork.

The starting commit merges the Second Life development history; local `upstream` points at `qwy16/secondlife_viewer`, not the official upstream URL. The cached `upstream/contribute` and `origin/contribute` refs match HEAD. An anonymous live query of origin HEAD and `refs/heads/contribute` independently returned the starting commit; no fetch, merge or push was performed. Compared with local upstream commit `e15a892821`, the merge adds only `.github/workflows/sync_upstream.yml`. That is evidence of a bootstrap fork with no Finalverse runtime at this baseline, not proof that all historical contributions were individually reviewed.

Alternative Firestorm-derived checkouts contain additional provenance and contributors (`FIRESTORM-SOURCE_LICENSE_HEADER.txt`). If selected or merged later, audit them independently; the direct viewer's licensing conclusion must not be blindly reused.

## Distribution risk register

| Surface | Evidence | Required release work |
|---|---|---|
| Inherited source | LGPL 2.1 and file headers | Preserve notices, supply corresponding modified source/build provenance; review linking/relink obligations for actual packaging |
| Viewer artwork | `doc/LICENSE-logos.txt`: CC BY-SA 3.0 notice, separate trademarks | Inventory used/modified assets and carry notices; commission product-owned icons where needed |
| Trademarks | SL/Linden names and logo references in README, assets, UI | Product identity inventory separate from source copyright |
| Third-party libraries | `autobuild.xml` licenses, platform archives | File-level notices, actual binary SBOM, transitive license review |
| Optional proprietary code | KDU, Havok, BugSplat, appearance utility manifest entries | Baseline excludes proprietary path; verify rights before enabling/distributing |
| Mixed bundled tools | `slvoice` Mixed, `vlc-bin` GPL2, `dullahan` MPL, fonts/dictionaries varied | Audit actual bundled files, not only manifest summary labels |
| Unclear labels | `jpegencoderbasic` NONE, physics extensions internal | Resolve actual notices/source provenance before release |
| Service identity/endpoints | `LLGridManager`, viewer manifest, About/settings | Classify login/grid, updates, crash, analytics, profiles, help and assets separately |
| Login/profile credentials | Authentication code and SL service policy | No credential forwarding to AI services; user-session and local credential isolation |

Manifest license strings are metadata, not a definitive license opinion. `dependency-manifest.json` records the source manifest; the packaging audit must inspect fetched archives and bundled runtime assets too.

## Service policy distinct from source licensing

The official [Third Party Viewer policy](https://secondlife.com/corporate/third-party-viewers) separates access to Second Life from use of open-source code. Its viewer identity, privacy and permission constraints apply when connecting to that service. Do not infer that a Finalverse-owned simulator has the same restrictions or that LGPL grants rights to the Second Life service/trademarks. Review target-grid policy before deploying persistent agents there.

Existing grid definitions deliberately protect system SL login endpoints from user-file overrides ([llviewernetwork.cpp](../../indra/newview/llviewernetwork.cpp)). Preserve that protection; generic search/replace of domains could redirect credentials. The AI metadata sidecar must not falsify creator/owner records or hide data inside inherited assets.

## Before distribution

Produce exact source/build/dependency revisions, artifact hashes, bundled notices, a source/relink compliance package appropriate to the actual link model, new/modified artwork attribution and a complete service endpoint inventory. Review About/login branding and privacy data flows. Native mobile packaging and store terms need a separate compatibility review. No distribution, public release or signing was performed in Phase 0.

## Phase 1 provenance additions

The user supplied existing Finalverse logo/icon and Lumi materials. [branding/README.md](../../indra/newview/branding/README.md) and the asset manifest record their provenance and hashes; pixels remain unchanged and no new artwork authorship/license is asserted. Confirm applicable asset rights before redistribution. The format-packaging script is LGPL-2.1-only. Existing source licenses, contributor lists and third-party notices remain. The inherited viewer is explicitly credited on the local welcome page and About screen. Product support and update identity are separated from grid/service identity, and the server continues to use MutSea branding. This documents the changes; it does not close the binary SBOM, redistribution, trademark or signing gates above.


## October 16 developer-preview distribution scope

The preview packaging path is [package_preview.py](../../scripts/finalverse/package_preview.py).
It stages a separate app from the full development build, copies the root
license, inherited source/logo notices and actual dependency license directory,
and emits a build inventory and source/build provenance. Proprietary paths,
BugSplat and automatic updates are disabled. The optional libVLC library, codec
plugins and media plugin are removed from the staged app until their complete
corresponding-source inventory is accepted. Voice defaults off and is not
preview-certified. These are explicit preview exclusions, not subsystem rewrites.

The release carries the exact viewer source commit and build-variable input,
plus pinned OpenAL/ALUT, Hunspell and Dullahan source with recursive submodules
and a source manifest. The actual license text for the build dependency labelled
`jpegencoderbasic: NONE` contains Adobe's BSD-style redistribution terms; retain
that text rather than interpreting the manifest label as an absence of terms.
All fetched license notices are retained even when a package is not shipped.
The dependency JSON identifies itself as a build inventory, not a file-level
claim that every installed package is distributed. CEF/Chromium notices remain
in the actual package notices.

The preview packager checks architectures, bundle symlinks, dynamic library
paths and private/debug files before creating a disk image. Corresponding source
and rebuild instructions accompany the binary; dependencies should remain
replaceable under their applicable terms. Preserve file-specific licenses when
modifying or redistributing any part. This engineering inventory does not
resolve future proprietary builds, mobile-store terms or a final production
legal/trademark review. Signing, notarization and runtime acceptance are separate
from licensing. User-provided branding remains attributed in its existing
manifest; no new ownership claim is made for inherited artwork.
