# Harbor environment authoring and publication boundary

2026-10-09. The original numerical Golden Hour, Day and Night library lives in Studio `manifests/environment-library-v0.1.json`. It supplies restrained sunlight, cool fill, haze, clouds, water fog/waves and explicit sun/moon angles. Gamma remains 1.0. Presets are candidate art direction until native viewer images, skin/clothing, water and foreground frame times pass.

The viewer implements sky, water and day-cycle settings through `LLEnvironment`, `LLSettingsVO` and shared `llsettingssky/water/daycycle`. Native sky/water validation runs before local benchmark previews. The preview selects `ENV_LOCAL`, so it cannot claim persistent region publication. Generic default textures and inherited native parameter representation keep their separate provenance. Original numerical overrides do not grant rights to inherited or external assets.

MutSea `MutSea/Region/CoreModules/World/LightShare/EnvironmentModule.cs` registers `EnvironmentSettings` and `ExtEnvironment` when the native capability configuration enables it. It loads region environment from `SimulationDataService`, stores authorized region changes and supports parcel environments. Estate-command and parcel-edit checks guard native updates. An NPC or child presence is rejected. Track-specific updates are unsupported. Use this established service for later operator-authorized publication; no direct LLM writes to environment storage.

## Acceptance sequence

1. Keep the unchanged, naturally advancing region baseline.
2. Pin its recorded sky/water locally; repeat all eight cameras as the controlled baseline.
3. Apply only the candidate session graphics controls and compare the same sky/water/cameras/resolution/population.
4. Preview original Golden Hour, Day and Night. Inspect sun direction, long shadows, overly dark skin, interior/path readability and water/reflection artifacts. Accept or revise based on actual images.
5. Add original path/interior lights through the bounded content transaction seam. Emissive appearance alone is not a world light source.
6. Publish to the region only after policy/audit/rollback evidence exists; verify the same environment through another client and simulator restart.

Camera `CAM_Sunset` is a named viewpoint, not proof that an inherited day-cycle snapshot was taken at sunset. The initial region baseline is daytime. Golden Hour deliberately places the low sun along the +X waterfront view. Native region persistence, a coherent lit night scene and commercial Harbor art quality remain separate gates.
