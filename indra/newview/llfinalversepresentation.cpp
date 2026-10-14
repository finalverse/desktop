// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalversepresentation.h"
#include <iomanip>
#include <sstream>

std::string llfinalversePlanSummary(const LLSD& plan)
{
    std::ostringstream text;
    const LLSD steps = plan["steps"];
    text << "Ready for your review · " << steps.size() << (steps.size() == 1 ? " change" : " changes") << "\n";
    if (plan["siting"].isString()) text << plan["siting"].asString() << "\n";

    S32 index = 0;
    for (const auto& step : llsd::inArray(steps))
    {
        const std::string type = step["type"].asString();
        const LLSD before = step["before"], after = step["after"];
        const LLSD value = after.isMap() ? after : before;
        // Always surface destructive steps, even in a long recipe.
        if (index++ >= 6 && type != "delete") continue;
        const std::string name = value["name"].asString().empty() ? "object" : value["name"].asString();
        const std::string action = type == "create" ? "Create" : type == "update" ? "Change" : type == "delete" ? "Remove" : type;
        text << "\n• " << action << " · " << name;
        if (type == "delete") text << " (remove from the world)";
        auto changed = [&](const char* field, S32 count)
        {
            if (before[field].size() != count || after[field].size() != count) return false;
            for (S32 i = 0; i < count; ++i) if (before[field][i].asReal() != after[field][i].asReal()) return true;
            return false;
        };
        if (type == "update" && before["name"].asString() != after["name"].asString()) text << "\n  Rename from “" << before["name"].asString() << "”";
        if (type == "update" && changed("position", 3))
        {
            text << "\n  Move by ";
            for (S32 axis = 0; axis < 3; ++axis)
            { if (axis) text << ", "; text << std::fixed << std::setprecision(1) << after["position"][axis].asReal() - before["position"][axis].asReal(); }
            text << " m (x, y, z)";
        }
        if ((type == "create" || (type == "update" && changed("scale", 3))) && after["scale"].size() == 3)
        {
            text << "\n  Size ";
            for (S32 axis = 0; axis < 3; ++axis)
            { if (axis) text << " × "; text << std::fixed << std::setprecision(1) << after["scale"][axis].asReal(); }
            text << " m";
        }
        if (type == "update" && changed("color", 4))
        {
            text << "\n  Color RGB ";
            for (S32 channel = 0; channel < 3; ++channel)
            { if (channel) text << ", "; text << (S32)(after["color"][channel].asReal()*255 + .5); }
        }
        if (type == "update" && changed("rotation", 4)) text << "\n  Orientation will change; see Technical details.";
        if (type == "update" && before["description"].asString() != after["description"].asString()) text << "\n  Description: " << after["description"].asString();
    }
    if (steps.size() > 6) text << "\n\nAll " << steps.size() << " steps are available in Technical details.";
    text << "\n\nPlacement and permissions checked. Approve within two minutes. You can cancel without changing the world.";
    return text.str();
}

bool llfinalverseBenefitsExpected(bool system_grid, const LLSD& response)
{
    return system_grid || response.has("account_level_benefits");
}

LLFinalverseServices llfinalverseParseServices(const LLSD& result, const std::string& region, const std::string& actor)
{
    LLFinalverseServices services;
    const LLSD citizen = result["citizen"];
    if (region.empty() || actor.empty() || !result.isMap() ||
        !result["schema_version"].isInteger() || result["schema_version"].asInteger() != 1 ||
        !result["region_id"].isString() || result["region_id"].asString() != region ||
        !result["actor_id"].isString() || result["actor_id"].asString() != actor ||
        !result["recovery_required"].isBoolean() || !result["compose_places"].isBoolean() ||
        !citizen.isMap() || !citizen["present"].isBoolean() || !citizen["can_interact"].isBoolean() ||
        !citizen["state"].isString()) return services;
    const std::string state = citizen["state"].asString();
    if (state != "enabled" && state != "not_enabled" && state != "not_in_world" &&
        state != "not_controller" && state != "starting" && state != "representation_recovery") return services;
    // Do not turn malformed discovery into an affirmative citizen control.
    if (citizen["can_interact"].asBoolean() && (!citizen["present"].asBoolean() || state != "enabled")) return services;
    services.world = services.known = true;
    services.recovery = result["recovery_required"].asBoolean();
    services.compose = result["compose_places"].asBoolean();
    services.citizen_present = citizen["present"].asBoolean();
    services.citizen_interactive = citizen["can_interact"].asBoolean();
    services.citizen_state = state;
    return services;
}

std::string llfinalverseCitizenUnavailable(const LLFinalverseServices& services)
{
    if (services.checking) return "Checking which AI tools are available in this world…";
    if (!services.known) return "Lumi's tools are unavailable here. You can still inspect the world.";
    if (services.citizen_state == "not_controller") return "Lumi is here, but this account cannot use her controller tools. You can still explore and create.";
    if (services.citizen_state == "starting") return "Lumi is still connecting. You can explore while her tools become available.";
    if (services.citizen_state == "representation_recovery") return "Lumi's embodiment needs administrator recovery. You can still explore the world.";
    return "Lumi is not connected to this world. You can create your own place and explore.";
}
