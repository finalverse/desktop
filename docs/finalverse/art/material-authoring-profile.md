# Finalverse material authoring profile v0.1

2026-10-09. Viewer implementation and importer delivery are distinct gates.

## Supported representations

`indra/llprimitive/llgltfmaterial.cpp` reads/writes base color, normal, metallic/roughness and emissive textures, factors, alpha mode/cutoff and double-sided state. It explicitly implements `KHR_texture_transform` scale/offset/rotation. The ORM representation is shared with the occlusion channel on export. This audit does not assert arbitrary glTF extensions such as transmission or clearcoat support.

Legacy diffuse and normal/specular material paths remain in the viewer. MutSea `Region/OptionalModules/Materials/MaterialsModule.cs` registers `RenderMaterials`, implements glTF JSON material updates and stores per-face material entries/overrides. Ordinary native capability permissions must still authorize updates. The current bounded content importer (`tools/MutSea.ContentCompile/Program.cs`, `Finalverse/World/ContentModule.cs`) rejects texture payloads and emits only legacy RGBA/fullbright. Thus a material displayed in Blender or stored in a manifest is **not yet delivered as PBR by this importer**.

## Canonical contract

- glTF 2.0 metallic/roughness subset; Blender shader graphs are baked into supported inputs.
- Base color and emissive images use sRGB; normal and ORM images are linear data. ORM: R occlusion, G roughness, B metallic. Factors use the glTF linear convention.
- Apply transforms; meters, Z-up authoring with explicit export conversion. Meshes need valid UVs and tangents for normal mapping. Fail nonfinite values and invalid dependencies.
- Opaque by default. Mask foliage where necessary; avoid blended layers for ordinary leaves. Clear glass has an explicit blended fallback; transmission/refraction is not assumed.
- Hero textures selectively ≤2048 px, standard ≤1024, background ≤512; record dimensions, bytes, hashes and attribution. Higher resolution needs a measured justification.
- Materials and texture dependencies get stable IDs/version and hashes. Record PBR intent separately from the importer’s actually supported fallback.

The original Studio library defines nine restrained material seeds: warm wood, light stone, dark metal, clear glass, warm plaster, neutral fabric, green leaf, pink flower and waterfront stone. No third-party image textures are bundled. Texture-free factors are authoring seeds, not a finished production texture library. PBR import, live surfaces, lighting/alpha behavior, restart and guarded Undo remain release gates.
