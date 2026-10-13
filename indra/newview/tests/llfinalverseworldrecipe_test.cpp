// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "../llviewerprecompiledheaders.h"
#include "../llfinalverseworldrecipe.h"
#include "llsdjson.h"
#include "llfile.h"
#include "../test/lltut.h"
#include <filesystem>
#include <fstream>
namespace tut
{
struct finalverse_recipe_data
{
    LLSD recipe = LlsdFromJson(boost::json::parse(R"({"schema_version":1,"kind":"world_spec","solver_version":"0.2.0","library_sha256":"9eff112ac530cb0d95008f9d4cb980fad358806573520afe5085b3511079003b","place":{"title":"Reading Garden","archetype":"nook","size":"small","ground":"grass","seating":{"count":2,"arrangement":"facing_focus"},"lighting":"low","planting":"beds","shade":"trees","focal":"tree","edge":"none"}})"));
    std::filesystem::path root = fsyspath(LLFile::tmpdir()) / ("finalverse-recipe-" + LLUUID::generateNewID().asString());
    finalverse_recipe_data() { std::filesystem::create_directory(root); }
    ~finalverse_recipe_data() { std::error_code ec; std::filesystem::remove_all(root, ec); }
};
typedef test_group<finalverse_recipe_data> group_t;
typedef group_t::object object_t;
group_t finalverse_recipe_group("finalverse_worldrecipe");
template<> template<> void object_t::test<1>()
{
    std::string error;
    ensure("recipe accepted", llfinalverseValidateRecipe(recipe, error));
    auto file = (root / "recipe.json").string();
    ensure("first save succeeds", llfinalverseSaveRecipe(recipe, file, error));
    ensure_equals("recipe roundtrip", llfinalverseReadRecipe(file, error), recipe);
    LLSD remix = recipe; remix["place"]["seating"]["count"] = 3;
    ensure("existing recipe protected", !llfinalverseSaveRecipe(remix, file, error));
    ensure_equals("existing content retained", llfinalverseReadRecipe(file, error), recipe);
    ensure_equals("original is immutable", recipe["place"]["seating"]["count"].asInteger(), 2);
}
template<> template<> void object_t::test<2>()
{
    std::string error;
    for (auto key : {"actor_id", "capability", "position", "code", "url"})
    { LLSD bad = recipe; bad[key] = "private-sentinel"; ensure("extra authority refused", !llfinalverseValidateRecipe(bad, error)); }
    LLSD bad = recipe; bad["place"]["seating"]["count"] = true;
    ensure("boolean cannot become a count", !llfinalverseValidateRecipe(bad, error));
    bad = recipe; bad["place"]["seating"]["count"] = 9;
    ensure("seat bound enforced", !llfinalverseValidateRecipe(bad, error));
    ensure("errors contain no supplied secrets", error.find("private-sentinel") == std::string::npos);
}
template<> template<> void object_t::test<3>()
{
    std::string error; auto file = (root / "oversized.json").string();
    { std::ofstream out(file); out << std::string(4097, 'x'); }
    ensure("oversized input refused", !llfinalverseReadRecipe(file, error).isMap());
    ensure("directory refused", !llfinalverseReadRecipe(root.string(), error).isMap());
}
template<> template<> void object_t::test<4>()
{
    std::string error; LLSD placed = recipe; placed["schema_version"] = 2; placed["site"] = "near_water";
    ensure("v2 bounded site rule accepted", llfinalverseValidateRecipe(placed, error));
    ensure("v1 recipe still accepted", llfinalverseValidateRecipe(recipe, error));
    placed["site"] = "http://private-sentinel";
    ensure("site is no endpoint or coordinate escape", !llfinalverseValidateRecipe(placed, error));
}
}
