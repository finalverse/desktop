# Fresh-login contact-plane defect — 2026-10-13

## Source diagnosis

MutSea `ScenePresence.CollisionPlane` initializes to `Vector4.UnitW` and returns that sentinel when no feet contact exists. `LLClientView` sends it in native full/terse avatar updates. The viewer stores it as `mFootPlane`. Before this fix, `LLWorld::resolveStepHeightGlobal` treated every nonzero four-vector as a surface, including `(0,0,0,1)`.

For that sentinel the normal is zero; its plane-distance calculation is `(0 - 1) + 0.05`, clamped to zero. It therefore chooses the top of the two-metre ankle ray instead of terrain: **one metre above the ankle**. Native stand IK tries to plant both feet there. This explains the fresh-login folded legs on Auren and Elara, and why a walk/contact refresh removes the symptom without a mesh change. It is a protocol interpretation defect, not evidence that a new avatar skeleton or forced first walk is required.

## Small shared fix

`llgroundplane.h` accepts only finite planes with a nonzero normal. `llworld.cpp` uses that check before projecting feet onto a contact plane. UnitW and cleared/non-finite planes fall back to the inherited terrain path; valid platform and slope contact behavior stays the same. No animation suppression, artificial walk, avatar-ID special case or world mutation is added.

The native quick harness reports bounded `foot_plane` and `foot_plane_has_surface` alongside existing avatar/animation state. The SDK observer records the actual collision plane in its receipt. The old viewer was absent during the initial observer attempt, so that failed receipt is retained rather than labelled native proof.

## Tests and build

The standalone C++ regression checks UnitW/cleared/non-finite planes, real floors/slopes, the old one-metre-lift scenario and preserved raised-platform tolerance. It passes independently and as the CMake/CTest `finalverse_ground_plane` target. The full ARM64 Release viewer build also passed with Xcode27.0.

Xcode27 initially required the license; the user accepted it in Terminal. This SDK rejects macOS11. The existing deployment-target clamp selects12, but old per-configuration LL_BUILD_RELEASE/RELWITHDEBINFO flags still embedded literal11. `00-Common.cmake` now removes duplicate minimum-version flags **after** importing all three C++ flag groups, leaving the existing SDK-derived CMake target as authority. This is a build compatibility fix, not a hardcoded new product-wide minimum. Explicit CMake regeneration is required because the clock-shifted cached Xcode project did not reliably regenerate before PCH compilation.

## Native result

The rebuilt executable SHA-256 is `c9cb3c99aa4464b31025b94d1b53f06127f344552a00c2aab0dadd9dd89fe95b`. It was copied into a new private `Finalverse QuickCheck r4.app`, leaving r3 intact. Its isolated profile began with only the loopback grid definition, without texture, shader or inventory caches.

Cold login reached Harbor without movement or a forced first walk. The representative Elara attachment appeared, and a separate native-protocol observer confirmed the expected mesh and all five invisible body bakes. Native capture state reports standing, no seat, no flight/autopilot and position `[128.05008,134.16649,26.14157]`. Its contact normal was approximately `[-0.0005293,0.00000794,0.9999999]`, plane W `25.15035`; the observer independently reports the same plane. An unobstructed three-quarter capture shows normally planted legs.

Private evidence: `dev/phase4b/closeout-20261013/{cold-login-native,observer-cold-login.json,viewer-package.json}` and `dev/phase4b/quick-visual-lumi-20261010/capture-1c95d6ce-539e-43f9-8e60-617a03e7a856/`. The first fixed camera missed the avatar, and a front correction was partly inside the beacon. Both are retained; the final correction is the visual standing evidence. The focused checks took170s and26s of wall time despite five-second sampling because macOS focus was lost; sending Escape to the current viewer restored focus. These are feature checks, not controlled performance acceptance.

The cold-login capture saw a valid physical contact; it did not record the transient UnitW packet before contact. The invalid sentinel projection is proven by source and regression tests, while this native check establishes successful startup/standing on the new build. No isolated old/new native reproduction of the transient packet is claimed. Retain previous all-nine gallery/worn sheets and walk/sit/stand, alpha, rig and restart proofs; no additional motion cycle or nonhuman reprovisioning was performed in this increment. Expand QA only for evidence of a template-specific regression.
