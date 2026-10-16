# Grid-aware discovery

MutSea is the spatial platform; Finalverse is the consumer experience. Consumer discovery may eventually use finalverse.com. Platform/community administration belongs to mutsea.com. This change does not depend on an unverified live web route.

The inherited destination floater now resolves discovery through `LLGridManager`, rather than a global Second Life URL on every grid. Agni/Aditi retain their service. External grids default to bundled native Finalverse cards, including Harbor, Lumi's Home, Pavilion, Creator Workshop and Sunset Dock. Cards prepare a question about the connected world; they do not claim a location exists or teleport to invented coordinates. Explore opens the existing facade and its actual movement/map/nearby tools.

## Local grid metadata

An operator/user may configure `destination_guide_url` in the existing local LLSD grids.xml. HTTPS is accepted at the selected grid's login origin. A separate discovery service needs an explicit HTTPS origin in `destination_guide_origins`:

```xml
<key>destination_guide_url</key><string>https://discovery.example/discover</string>
<key>destination_guide_origins</key><array><string>https://discovery.example</string></array>
```

This is an example, not a deployed Finalverse endpoint. Remote grid-info responses are not imported into this allowlist. Grid labels and `system_grid` claims confer no trust; only immutable built-in Agni/Aditi definitions are system grids. HTTP is allowed only for exact same-origin loopback development. Credentials, ambiguous authority/ports, opaque/file/script URLs and secondlife.com/subdomains are rejected for external-grid discovery. Missing or rejected URLs use the native cards. An explicitly unknown grid also uses discovery fallback while the existing login credential guard remains intact.

External pages are untrusted. HTTP failures, plugin failure and 15-second load timeout return to native cards. Cross-origin top-level navigation is stopped when its location event is observed; this is not a pre-request network firewall and does not constrain ordinary remote-page subresources. Cards need no web plugin. No destination metadata can run local authoring code or bypass World policy. Dynamic destination APIs, community catalogs and semantic deep links are future 4B.1/4C work.

## Bounded inherited URL audit

| Surface / source | Classification | Current handling |
|---|---|---|
| DestinationGuideURL / llfloaterdestinations | B, product discovery | Grid URL policy + native fallback; system grids retain upstream URL |
| SearchURL / llfloatersearch | B, product search/events | External grids open discovery; system search retained |
| MarketplaceURL / llfloatermarketplace | C, commerce not implemented | External grids show neutral unavailable message and never navigate this floater to SL; upstream marketplace retained for system grids |
| HelpURL / llfloaterhelpbrowser | B/C, product support | External grids use bundled Finalverse help; upstream help retained for system grids |
| Login/helper/profile grid metadata / llviewernetwork | A, grid/protocol compatibility | Explicit per-grid values retained; no global domain substitution |
| maps.secondlife.com SLURLs / llslurl, URL parser | A, interoperable location syntax | Preserve protocol parsing; not a destination service fallback |
| LGPL headers, LICENSE, credits and upstream references | A, legal/provenance | Preserve attribution |
| Premium, L$ purchase, land commerce, web profile/feed and legacy specialized marketplace links in full interface | B/C, wider product surface | Remaining audit/hardening items; consumer facade excludes much of this surface. Do not claim all external web routes have been replaced |

Only identified discovery/search/marketplace-floater/help paths are changed. A global secondlife.com replacement would break legal references and grid interoperability. Future grid service metadata should reuse the origin policy and explicitly define permissions, availability and error behavior per service.
