// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalversepresentation.h"
#include <iomanip>
#include <sstream>
#include <set>
#include <cmath>
#include <map>

void LLFinalverseMessageLog::session(const std::string& id)
{
    if (id != mSession) { clear(); mSession = id; }
}
void LLFinalverseMessageLog::clear()
{
    mEntries.clear(); mBytes = 0; ++mRevision;
}
void LLFinalverseMessageLog::append(const std::string& id, const std::string& channel,
    const std::string& speaker, const std::string& text, const std::string& time)
{
    session(id);
    if (id.empty() || (channel != "conversation" && channel != "activity") || text.empty()) return;
    std::string safe = utf8str_truncate(text, 4096);
    safe.erase(std::remove_if(safe.begin(), safe.end(), [](unsigned char c) {
        return (c < 32 && c != '\n' && c != '\t') || c == 127;
    }), safe.end());
    if (safe.empty()) return;
    LLSD entry;
    entry["channel"] = channel;
    entry["speaker"] = utf8str_truncate(speaker, 64);
    entry["text"] = safe;
    entry["time"] = utf8str_truncate(time, 16);
    mEntries.push_back(entry); mBytes += safe.size(); ++mRevision;
    while (mEntries.size() > 128 || mBytes > 65536)
    { mBytes -= mEntries.front()["text"].asString().size(); mEntries.pop_front(); }
}

std::string llfinalverseQuickAction(const std::string& text)
{
    std::string command(text); LLStringUtil::trim(command); LLStringUtil::toLower(command);
    static const std::map<std::string,std::string> actions{
        {"/world","world"}, {"/fps","first_person"}, {"/overview","overview"}, {"/bird","overview"},
        {"/create","create"}, {"/worlds","destinations"}, {"/map","map"}, {"/inventory","inventory"},
        {"/friends","friends"}, {"/me","profile"}, {"/help","help"}};
    const auto found = actions.find(command);
    return found == actions.end() ? std::string() : found->second;
}

LLSD llfinalverseFindActions(const std::string& query)
{
    struct Action { const char* id; const char* title; const char* keywords; };
    static const Action actions[] = {
        {"destinations", "World Library", "explore regions teleport samples confluence"},
        {"map", "World map", "travel regions teleport geography"},
        {"inventory", "Inventory", "assets objects items folders"},
        {"friends", "Friends", "people social contacts"},
        {"nearby", "Nearby people", "social avatars local"},
        {"profile", "My profile", "me identity account"},
        {"appearance", "Outfits and appearance", "me avatar clothing wardrobe body"},
        {"chat", "Conversations", "chat messages text social"},
        {"build", "Object editing", "create build tools move rotate scale"},
        {"myenvironments", "Environment", "lighting sky water day night"},
        {"snapshot", "Take a photo", "camera screenshot capture share"},
        {"places", "Saved places", "landmarks favorites travel"},
        {"view", "Camera controls", "orbit zoom focus view"},
        {"move", "Movement controls", "walk fly navigation"},
        {"preferences", "Settings", "preferences options display sound"},
        {"performance", "Performance", "graphics fps rendering quality"}};
    LLSD result = LLSD::emptyArray();
    if (query.size() > 128) return result;
    std::string filter(query); LLStringUtil::toLower(filter);
    for (const auto& action : actions)
    {
        std::string haystack = std::string(action.title) + " " + action.keywords;
        LLStringUtil::toLower(haystack);
        std::istringstream tokens(filter); std::string token; bool matches = true;
        while (tokens >> token) if (haystack.find(token) == std::string::npos) { matches = false; break; }
        if (!matches) continue;
        LLSD row; row["id"] = action.id; row["title"] = action.title; result.append(row);
    }
    return result;
}

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
    services.server_planning = result["server_planning"].isBoolean() && result["server_planning"].asBoolean();
    services.earth_bridge = result["earth_bridge"].isBoolean() && result["earth_bridge"].asBoolean();
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

LLSD llfinalverseParseDestinations(const LLSD& result, const std::string& region, const std::string& actor)
{
    LLSD out = LLSD::emptyArray();
    if (!result.isMap() || !result["schema_version"].isInteger() || result["schema_version"].asInteger() != 1 ||
        result["region_id"].asString() != region || result["actor_id"].asString() != actor ||
        !result["destinations"].isArray() || result["destinations"].size() > 64) return out;
    std::set<std::string> ids;
    for (const auto& d : llsd::inArray(result["destinations"]))
    {
        const auto id = d["region_id"].asString();
        if (!LLUUID::validate(id) || LLUUID(id).isNull() || !ids.insert(id).second ||
            !d["title"].isString() || d["title"].asString().empty() || d["title"].asString().size() > 320 ||
            !d["online"].isBoolean() || !d["spawn"].isArray() || d["spawn"].size() != 3) continue;
        bool valid = true;
        for (S32 axis = 0; axis < 3; ++axis)
        {
            const auto& n = d["spawn"][axis]; const F64 value = n.asReal();
            valid = valid && (n.isReal() || n.isInteger()) && std::isfinite(value) && value >= 1. && value <= (axis == 2 ? 4096. : 255.);
        }
        if (d["online"].asBoolean())
        {
            valid = valid && d["global_origin"].isArray() && d["global_origin"].size() == 2;
            for (S32 axis = 0; axis < 2; ++axis)
                valid = valid && d["global_origin"][axis].isInteger() && d["global_origin"][axis].asInteger() >= 0;
        }
        if (valid) out.append(d);
    }
    return out;
}

LLSD llfinalverseSessionPage(const LLSD& state, const LLSD& page, std::string& error)
{
    error.clear();
    const std::string id = page["session"]["id"].asString();
    if (page["protocol_version"].asString() != "1.0" || !LLUUID::validate(id) ||
        page["session"]["mode"].asString() == "code" || !page["events"].isArray() ||
        page["events"].size() > 32 || (!state["session_id"].asString().empty() && state["session_id"].asString() != id))
    { error = "Incompatible shared session. Reconnect explicitly."; return LLSD(); }
    LLSD next = state;
    next["session_id"] = id; next["title"] = utf8str_truncate(page["session"]["title"].asString(), 120);
    next["status"] = page["session"]["status"]; next["permissions"] = page["permissions"];
    if (!next["messages"].isArray()) next["messages"] = LLSD::emptyArray();
    if (!next["activity"].isArray()) next["activity"] = LLSD::emptyArray();
    S32 cursor = state["cursor"].asInteger();
    for (const auto& event : llsd::inArray(page["events"]))
    {
        if (event["protocol_version"].asString() != "1.0" || event["session_id"].asString() != id ||
            !event["sequence"].isInteger() || event["sequence"].asInteger() != cursor + 1 || cursor >= 4096)
        { error = "Session event gap or replay. Reconnect and replay from the source."; return LLSD(); }
        ++cursor;
        const std::string type = event["type"].asString(), request = event["request_id"].asString();
        if (type == "message.started")
        {
            LLSD human, ai;
            human["request"] = ai["request"] = request;
            human["role"] = "human"; human["speaker"] = "Human";
            human["text"] = utf8str_truncate(event["data"]["user_text"].asString(), 4096);
            ai["role"] = "ai"; ai["speaker"] = utf8str_truncate(event["data"]["model"].asString(), 128); ai["text"] = "";
            next["messages"].append(human); next["messages"].append(ai);
        }
        else if (type == "message.delta")
        {
            for (S32 i = next["messages"].size() - 1; i >= 0; --i)
                if (next["messages"][i]["request"].asString() == request && next["messages"][i]["role"].asString() == "ai")
                { next["messages"][i]["text"] = utf8str_truncate(next["messages"][i]["text"].asString() + event["data"]["text"].asString(), 32000); break; }
        }
        else if (type == "confluence.participant_message")
        {
            for (S32 i = 0; i < next["messages"].size(); ++i)
                if (next["messages"][i]["request"].asString() == request && next["messages"][i]["role"].asString() == "human")
                    next["messages"][i]["speaker"] = "Avatar " + event["data"]["agent_id"].asString();
        }
        else
        {
            LLSD activity; activity["type"] = type; activity["sequence"] = cursor;
            next["activity"].append(activity);
        }
    }
    if (!page["cursor"].isInteger() || page["cursor"].asInteger() != cursor)
    { error = "Incorrect session replay cursor."; return LLSD(); }
    next["cursor"] = cursor;
    S32 bytes = 0;
    for (const auto& message : llsd::inArray(next["messages"])) bytes += message["text"].asString().size();
    while (next["messages"].size() > 128 || bytes > 65536)
    { bytes -= next["messages"][0]["text"].asString().size(); next["messages"].erase(0); }
    while (next["activity"].size() > 64) next["activity"].erase(0);
    if (!page["artifacts"].isArray() || page["artifacts"].size() > 8)
    { error = "Artifact reference budget exceeded."; return LLSD(); }
    next["artifacts"] = page["artifacts"];
    return next;
}
