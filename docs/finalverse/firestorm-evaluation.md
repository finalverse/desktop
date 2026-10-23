# Viewer foundation: licensing and Firestorm evaluation

Engineering decision, 2026-10-23. This compares source terms and build evidence;
it does not certify every future binary or dependency for distribution.

## Decision

Continue the working Finalverse Desktop foundation and its open build. Firestorm
is a credible OpenSim compatibility alternative, but changing to it does not
remove the inherited LGPL obligations. No identified source-license difference
currently justifies replacing the accepted native Desktop integration.

The user's request to migrate if Second Life constraints require it remains a
decision criterion. A concrete incompatible dependency or a demonstrated native
grid compatibility gap should trigger a scoped port evaluation. A failed
upstream CLA service check is addressed by repository CI ownership, not a source
rewrite or a fabricated signature.

## What was checked

| Concern | Current Desktop | Firestorm alternative | Consequence |
|---|---|---|---|
| Inherited source | LGPL 2.1, with file-specific notices | Also derived from Linden's viewer; retains Linden LGPL 2.1 notice | Preserve source, copyright, build and applicable linking/relink terms in either case |
| Contribution agreement | The inherited workflow calls Linden's private signature repository | A different contribution workflow does not relicense inherited code | Finalverse's CLA job now runs only in the Second Life organization; source-license checks remain enabled |
| Service policy | Linden's Third Party Viewer policy governs connections to its service | Firestorm also supports Second Life | A client swap does not avoid service rules when connecting to Second Life; MutSea has its own authority and policies |
| Branding and artwork | Separate trademark and artwork notices | Adds Firestorm branding and additional contributors | A migration adds provenance work; it does not grant trademark rights |
| Optional libraries | Manifest includes proprietary options; Finalverse uses `INSTALL_PROPRIETARY=FALSE` | Has its own dependency manifest and configuration choices | Inspect actual configured and bundled files; neither repository name certifies a package |

Current local `build-darwin-universal/CMakeCache.txt` records proprietary
installation disabled and OpenAL enabled. The CI matrix selects `ReleaseOS`;
its manifest configuration also disables proprietary installation. Manifest
entries for Havok or Kakadu alone do not establish that they are linked or
distributed. Existing [packaging checks and distribution audit](licensing-audit.md)
continue to govern actual artifacts, including voice/media exclusions and
corresponding dependency source. This comparison grants no additional rights.
Firestorm's own build guide also requires appropriate licenses before enabling
Kakadu or Havok; choosing Firestorm does not supply those licenses.

## Alternative source and port boundary

The reviewed candidate is the named release branch `Firestorm_7.2.5`, pinned to
`4ac201fdc56127de27de34567286885f4e75d194`. This is an evaluation input, not a
claim that the branch is the newest or that a Finalverse build from it passed.
Firestorm requests that distributed derivatives use official release branches.
Its nightly `master` was not selected as a product foundation.

A future port must preserve native login, inventory, avatars, permissions,
capability renewal and simulator authority. Port the bounded FinalStorm scene
capture/ABI adapter and native UI entry points separately from the authoritative
MutSea, Confluence, Lumi and SpatialDB services. Keep the existing service
identities and stores. Never copy credentials or turn a rendering adapter into
world-write authority. An FFI or process boundary alone is not license clearance.

Before changing the default client, run the same native rendering, session,
world-operation and Undo acceptance gates against the exact candidate. Preserve
the current working app until the replacement passes. There is no Firestorm
migration or accepted Firestorm binary in this change.

## Primary sources

- [Pinned Firestorm README](https://github.com/FirestormViewer/phoenix-firestorm/blob/4ac201fdc56127de27de34567286885f4e75d194/README.md): derivation and release-branch guidance.
- [Pinned inherited source notice](https://github.com/FirestormViewer/phoenix-firestorm/blob/4ac201fdc56127de27de34567286885f4e75d194/doc/LICENSE-source.txt) and [license](https://github.com/FirestormViewer/phoenix-firestorm/blob/4ac201fdc56127de27de34567286885f4e75d194/LICENSE).
- [Pinned Firestorm dependency manifest](https://github.com/FirestormViewer/phoenix-firestorm/blob/4ac201fdc56127de27de34567286885f4e75d194/autobuild.xml).
- [Pinned Firestorm macOS build guide](https://github.com/FirestormViewer/phoenix-firestorm/blob/4ac201fdc56127de27de34567286885f4e75d194/doc/building_macos.md): optional library licenses and build configurations.
- [Linden Third Party Viewer policy](https://secondlife.com/corporate/third-party-viewers): service access and open-source use are explicitly distinguished.
- Local [source notice](../../doc/LICENSE-source.txt), [license](../../LICENSE), [build configuration](../../autobuild.xml) and [CLA workflow](../../.github/workflows/cla.yaml).
