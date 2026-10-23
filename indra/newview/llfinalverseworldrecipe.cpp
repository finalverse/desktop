// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalverseworldrecipe.h"
#include "llsdjson.h"
#include "llfile.h"
#include <filesystem>
#include <fstream>
#include <set>

namespace
{
bool keys(const LLSD& value, const std::set<std::string>& expected)
{
    if (!value.isMap() || value.size() != expected.size()) return false;
    for (const auto& entry : llsd::inMap(value)) if (!expected.count(entry.first)) return false;
    return true;
}
bool choice(const LLSD& value, std::initializer_list<const char*> allowed)
{
    if (!value.isString()) return false;
    for (auto item : allowed) if (value.asString() == item) return true;
    return false;
}
}
bool llfinalverseValidateRecipe(const LLSD& recipe, std::string& error)
{
    error = "This file is not a supported place recipe. No world changes were made.";
    const bool v2 = recipe["schema_version"].isInteger() && recipe["schema_version"].asInteger() == 2;
    const std::set<std::string> expected = v2 ? std::set<std::string>{"schema_version", "kind", "solver_version", "library_sha256", "place", "site"} : std::set<std::string>{"schema_version", "kind", "solver_version", "library_sha256", "place"};
    if (!keys(recipe, expected) ||
        !recipe["schema_version"].isInteger() || (recipe["schema_version"].asInteger() != 1 && !v2) ||
        (v2 && !choice(recipe["site"], {"in_front_of_actor", "near_actor", "near_water"})) ||
        !choice(recipe["kind"], {"world_spec"}) || !recipe["solver_version"].isString() ||
        recipe["solver_version"].asString().empty() || recipe["solver_version"].asString().size() > 80 ||
        !recipe["library_sha256"].isString()) return false;
    const auto hash = recipe["library_sha256"].asString();
    if (hash.size() != 64 || hash.find_first_not_of("0123456789abcdef") != std::string::npos) return false;
    const auto& p = recipe["place"];
    if (!keys(p, {"title", "archetype", "size", "ground", "seating", "lighting", "planting", "shade", "focal", "edge"}) ||
        !p["title"].isString() || p["title"].asString().size() < 3 || p["title"].asString().size() > 240 ||
        !choice(p["archetype"], {"nook", "picnic", "circle", "garden", "lookout", "plaza", "grove", "pavilion"}) ||
        !choice(p["size"], {"small", "medium", "large"}) || !choice(p["ground"], {"grass", "paved", "deck"}) ||
        !choice(p["lighting"], {"none", "low", "medium", "high"}) || !choice(p["planting"], {"none", "beds", "trees", "both"}) ||
        !choice(p["shade"], {"none", "trees", "canopy"}) || !choice(p["focal"], {"none", "table", "stele", "compass", "tree", "beacon"}) ||
        !choice(p["edge"], {"none", "rail", "beds"}) || !keys(p["seating"], {"count", "arrangement"}) ||
        !p["seating"]["count"].isInteger() || p["seating"]["count"].asInteger() < 0 || p["seating"]["count"].asInteger() > 8 ||
        !choice(p["seating"]["arrangement"], {"facing_focus", "ring", "pairs", "row_to_view"})) return false;
    if (boost::json::serialize(LlsdToJson(recipe)).size() > 4096) return false;
    error.clear(); return true;
}
LLSD llfinalverseReadRecipe(const std::string& filename, std::string& error)
{
    try
    {
        const std::filesystem::path path = fsyspath(filename);
        if (!std::filesystem::is_regular_file(std::filesystem::symlink_status(path)))
            throw std::runtime_error("Not a regular file");
        std::ifstream input(path, std::ios::binary);
        char data[4097]; input.read(data, sizeof(data));
        if (input.gcount() < 1 || input.gcount() > 4096 || input.bad()) throw std::runtime_error("Invalid size");
        LLSD recipe = LlsdFromJson(boost::json::parse(std::string_view(data, input.gcount())));
        if (llfinalverseValidateRecipe(recipe, error)) return recipe;
    }
    catch (...) { error = "Could not read this place recipe. Choose a regular JSON recipe of up to 4 KB."; }
    return LLSD();
}
bool llfinalverseSaveRecipe(const LLSD& recipe, const std::string& filename, std::string& error)
{
    if (!llfinalverseValidateRecipe(recipe, error)) return false;
    // Exclusive creation protects existing recipes, including concurrent saves.
    std::error_code ec;
    LLFile file(filename, LLFile::out | LLFile::binary | LLFile::noreplace, ec, 0600);
    if (!file) { error = "Could not save a new recipe here. Choose a new filename."; return false; }
    const auto json = boost::json::serialize(LlsdToJson(recipe)) + "\n";
    const bool wrote = file.write(json.data(), json.size(), ec) == json.size() && !ec;
    const bool closed = file.close(ec) == 0 && !ec;
    if (!wrote || !closed) { error = "The recipe could not be fully saved. Choose another filename."; return false; }
    error.clear(); return true;
}
