# Public download decisions

## October 16: experimental developer preview

The owner explicitly requested a public GitHub prerelease with current source,
real demo media and installation instructions. This supersedes the previous
blanket binary hold **only for an experimental developer preview**. It does not
accept the invited beta or GA gates below.

The release candidate is rebuilt with its own preview bundle identity and a
pinned, clean source tree. The staged distribution has no operator profile or
account, carries inherited notices and corresponding source/build inputs, and
excludes optional libVLC/codec playback pending its source inventory. Voice
defaults off. It uses an ad hoc signature; Developer ID, notarization and second
Mac acceptance remain open. GitHub download availability does not open the
private world or grant control of Lumi.

See [preview scope](releases/0.4.0-preview.1.md),
[installation](getting-started.md), [build/package contract](building-preview.md)
and [actual demo evidence](demos.md). Artifact verification is recorded in the
release assets and final delivery report; source preparation is not proof that
an installer has run.

## October 15: private pilot decision (historical)

Decision: **hold public binary publication**. The current application is a private
pilot, not a beta/GA release candidate. This is a packaging/onboarding/scope
boundary, not a requirement to keep expanding art before real users can test.

Verified Mac artifact: universal arm64 + x86_64 executable, run on this Apple
Silicon Mac, ad hoc signed for private review. At this historical decision point, the latest viewer source included
preserved destination/network changes awaiting a coherent release commit. No signed/notarized clean installer or public update channel is
accepted. Intel hardware, Windows and Linux viewer binaries have not been
verified for this release. A Linux server image is not a Linux viewer build.

| Gate before public Mac beta | Current position |
|---|---|
| Clean, pinned release source; source/license notices; dependency/asset provenance | Pilot code is locally committed; unrelated work remains preserved separately. Prepare a clean release branch and audit the actual packaged contents. |
| Developer ID signing, notarization, checksums and installation on a second Mac | Current ad hoc app is a private test bundle; no public installer is published. |
| Invited fresh account can provision appearance, enter the hosted world and use authorized Create/Undo without local developer services | Operator-provisioned pilot login and world operations are verified; self-service public acquisition remains gated. |
| Hosted AI isolation, controller ownership and recovery under multiple real users | One explicitly configured Lumi controller; bounded budgets and restart behavior are tested. General public co-builder access is not implemented. |

The combined hosted-Lumi Mac interaction gate has now passed: identity,
reviewable co-building, explicit approval, a rendered 147-part reading nook and
visible Undo with acknowledgement. Native counts returned from 570 to the
423-part baseline. See [acceptance evidence](hosted-lumi-and-placement.md).
This verifies the operator pilot and does not replace fresh-user onboarding or
clean release packaging.

Next release target is an **invited Mac beta** after these concrete gates pass.
Package a clean app without private account/session/profile/log files; publish
only that signed/notarized artifact and checksum/source links through an explicit
GitHub release. Do not upload the private review app or mark an unverified Linux/
Windows artifact as downloadable. GA follows actual invited-user reliability and
support evidence. No GitHub release, public tag or binary upload was performed
for this milestone.
