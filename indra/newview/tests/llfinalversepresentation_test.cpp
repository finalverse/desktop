// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "../llviewerprecompiledheaders.h"
#include "../llfinalversepresentation.h"
#include "../llfinalverselegacyappearance.h"
#include "../llfinalverseteleport.h"
#include "../test/lltut.h"
#include <set>
namespace tut
{
struct finalverse_presentation_data {};
typedef test_group<finalverse_presentation_data> group_t;
typedef group_t::object object_t;
group_t finalverse_presentation_group("finalverse_presentation");
template<> template<> void object_t::test<6>()
{
    ensure("declared legacy region", llfinalverseClientBakingRequired(true, false, false));
    ensure("no region is not legacy", !llfinalverseClientBakingRequired(false, false, false));
    ensure("server baking cannot be downgraded", !llfinalverseClientBakingRequired(true, true, false));
    ensure("system grid cannot be downgraded", !llfinalverseClientBakingRequired(true, false, true));
}
template<> template<> void object_t::test<1>()
{
    LLSD plan, step;
    step["type"]="update"; step["after"]["name"]="My chair";
    step["after"]["id"]="private-identifier";
    for (S32 axis=0; axis<3; ++axis) { step["before"]["position"].append(10.); step["after"]["position"].append(axis==0?12.:10.); }
    plan["steps"].append(step);
    const auto result=llfinalversePlanSummary(plan);
    ensure("target is understandable",result.find("My chair")!=std::string::npos);
    ensure("actual change is shown",result.find("2.0, 0.0, 0.0 m")!=std::string::npos);
    ensure("identifier is hidden",result.find("private-identifier")==std::string::npos);
}
template<> template<> void object_t::test<2>()
{
    LLSD plan, step;
    step["type"]="create"; step["after"]["name"]="Garden shrub";
    for (S32 i=0;i<9;++i) plan["steps"].append(step);
    step["type"]="delete"; step.erase("after"); step["before"]["name"]="Old wall";
    plan["steps"].append(step);
    const auto result=llfinalversePlanSummary(plan);
    ensure("full count shown",result.find("10 changes")!=std::string::npos);
    ensure("destruction cannot be truncated",result.find("Remove · Old wall (remove from the world)")!=std::string::npos);
}
template<> template<> void object_t::test<3>()
{
    LLSD plan, step; step["type"]="create";
    for (double size : {1.2,1.4,1.6}) step["after"]["scale"].append(size);
    plan["steps"].append(step);
    const auto result=llfinalversePlanSummary(plan);
    ensure("actual dimensions",result.find("1.2 × 1.4 × 1.6 m")!=std::string::npos);
    ensure("review does not imply execution",result.find("Ready for your review")!=std::string::npos);
}
template<> template<> void object_t::test<4>()
{
    LLSD response;
    ensure("optional vendor service absence is not a broken login", !llfinalverseBenefitsExpected(false,response));
    ensure("system grid failures must remain visible", llfinalverseBenefitsExpected(true,response));
}
template<> template<> void object_t::test<5>()
{
    LLSD response; response["account_level_benefits"]=LLSD();
    ensure("advertised but invalid service must remain visible",llfinalverseBenefitsExpected(false,response));
}

LLSD servicesReply()
{
    LLSD reply;
    reply["schema_version"] = 1; reply["region_id"] = "region"; reply["actor_id"] = "actor";
    reply["recovery_required"] = false; reply["compose_places"] = true;
    reply["citizen"]["present"] = true; reply["citizen"]["can_interact"] = true; reply["citizen"]["state"] = "enabled";
    return reply;
}
template<> template<> void object_t::test<7>()
{
    const auto reply = servicesReply();
    auto result = llfinalverseParseServices(reply, "region", "actor");
    ensure("authenticated controller can use existing relay", result.citizen_interactive && result.canCompose());
    ensure("old region reply cannot grant new controls", !llfinalverseParseServices(reply, "another-region", "actor").known);
    ensure("other account reply cannot grant controls", !llfinalverseParseServices(reply, "region", "another-actor").known);
}
template<> template<> void object_t::test<8>()
{
    LLSD reply = servicesReply(); reply["citizen"]["can_interact"] = "true";
    ensure("coerced boolean cannot enable controls", !llfinalverseParseServices(reply, "region", "actor").known);
    reply = servicesReply(); reply["citizen"]["present"] = false;
    ensure("contradictory presence cannot grant control", !llfinalverseParseServices(reply, "region", "actor").known);
    reply = servicesReply(); reply["schema_version"] = 2;
    ensure("unsupported schema fails closed", !llfinalverseParseServices(reply, "region", "actor").known);
    ensure("missing metadata fails closed", !llfinalverseParseServices(LLSD(), "region", "actor").citizen_interactive);
}
template<> template<> void object_t::test<9>()
{
    LLSD reply = servicesReply(); reply["citizen"]["can_interact"] = false; reply["citizen"]["state"] = "not_controller";
    auto result = llfinalverseParseServices(reply, "region", "actor");
    ensure("observer can create without commanding citizen", result.citizen_present && !result.citizen_interactive && result.canCompose());
    ensure("observer limitation is explained", llfinalverseCitizenUnavailable(result).find("controller") != std::string::npos);
    reply["recovery_required"] = true; result = llfinalverseParseServices(reply, "region", "actor");
    ensure("recovery keeps inspection but pauses changes", result.world && !result.canCompose() && !result.canPlan());
}

template<> template<> void object_t::test<10>()
{
    LLSD reply = servicesReply();
    ensure("legacy fixture keeps local planner", !llfinalverseParseServices(reply, "region", "actor").server_planning);
    reply["server_planning"] = true;
    ensure("authenticated region advertises server planner", llfinalverseParseServices(reply, "region", "actor").server_planning);
    auto checking = llfinalverseParseServices(reply, "region", "actor"); checking.checking = true;
    ensure("first discovery cannot race into the local planner", !checking.canPlan());
    ensure("different actor cannot select planner route", !llfinalverseParseServices(reply, "region", "other").server_planning);
    reply["server_planning"] = "true";
    ensure("string cannot select server planner", !llfinalverseParseServices(reply, "region", "actor").server_planning);
}

template<> template<> void object_t::test<11>()
{
    LLSD reply, d; reply["schema_version"]=1; reply["region_id"]="region"; reply["actor_id"]="actor";
    d["region_id"]="11111111-1111-4111-8111-111111111111"; d["title"]="Confluence"; d["online"]=true;
    d["spawn"].append(128.); d["spawn"].append(116.); d["spawn"].append(24.); d["global_origin"].append(256000); d["global_origin"].append(256000);
    reply["destinations"].append(d);
    ensure_equals("real destination accepted",llfinalverseParseDestinations(reply,"region","actor").size(),1);
    ensure_equals("old session refused",llfinalverseParseDestinations(reply,"region","other").size(),0);
    reply["destinations"][0]["spawn"][0]=256.;
    ensure_equals("out of region refused",llfinalverseParseDestinations(reply,"region","actor").size(),0);
    reply["destinations"][0]=d;reply["destinations"][0]["online"]="true";
    ensure_equals("typed availability required",llfinalverseParseDestinations(reply,"region","actor").size(),0);
}

template<> template<> void object_t::test<12>()
{
    ensure("source closure waits for an active MutSea handoff", llfinalverseDeferSourceDisable(false,true,true,true,0.));
    ensure("short handshake delay is allowed", llfinalverseDeferSourceDisable(false,true,true,true,14.9));
    ensure("timeout cannot hide a broken connection", !llfinalverseDeferSourceDisable(false,true,true,true,15.));
    ensure("ordinary disconnect remains immediate", !llfinalverseDeferSourceDisable(false,true,true,false,0.));
    ensure("destination or child disable remains immediate", !llfinalverseDeferSourceDisable(false,true,false,true,0.));
    ensure("system grid behavior is preserved", !llfinalverseDeferSourceDisable(true,true,true,true,0.));
    ensure("unadvertised grids are unchanged", !llfinalverseDeferSourceDisable(false,false,true,true,0.));
}

template<> template<> void object_t::test<13>()
{
    ensure_equals("explicit FPS shortcut", llfinalverseQuickAction("/fps"), "first_person");
    ensure_equals("bird view alias", llfinalverseQuickAction(" /BIRD "), "overview");
    ensure_equals("worlds opens native destination library", llfinalverseQuickAction("/worlds"), "destinations");
    ensure_equals("inventory reuses inherited action", llfinalverseQuickAction("/inventory"), "inventory");
    ensure("ordinary conversation is not executed locally", llfinalverseQuickAction("open my inventory").empty());
    ensure("unknown slash command cannot invoke arbitrary action", llfinalverseQuickAction("/delete everything").empty());
}

template<> template<> void object_t::test<14>()
{
    auto results = llfinalverseFindActions("  NIGHT  lighting ");
    ensure_equals("semantic keywords find native environment tools",results.size(),1);
    ensure_equals("search maps to a fixed command",results[0]["id"].asString(),"myenvironments");
    ensure_equals("outfit synonyms find existing appearance",llfinalverseFindActions("wardrobe")[0]["id"].asString(),"appearance");
    ensure_equals("unknown tools cannot become executable",llfinalverseFindActions("delete all objects").size(),0);
    ensure_equals("oversize search is bounded",llfinalverseFindActions(std::string(129,'a')).size(),0);
    std::set<std::string> ids;
    const auto catalog = llfinalverseFindActions("");
    for (const auto& action : llsd::inArray(catalog)) ensure("unique fixed action identifiers",ids.insert(action["id"].asString()).second);
    ensure("core tools remain discoverable",ids.count("inventory") && ids.count("preferences") && ids.count("destinations"));
}

template<> template<> void object_t::test<15>()
{
    LLFinalverseMessageLog log;
    log.append("login-a", "conversation", "You", "Private first account message", "12:00");
    ensure_equals("first login retains its message", log.entries().size(), size_t(1));
    const U64 revision = log.revision();
    log.session("login-b");
    ensure("another login cannot see prior messages", log.entries().empty());
    ensure("session change invalidates display", log.revision() > revision);
    log.append("", "conversation", "You", "No active login", "12:01");
    ensure("logged-out traffic is not retained", log.entries().empty());
}
template<> template<> void object_t::test<16>()
{
    LLFinalverseMessageLog log;
    for (int n = 0; n < 300; ++n) log.append("session", "activity", "World", std::string(4096, 'x'), "12:00");
    size_t bytes = 0; for (const auto& entry : log.entries()) bytes += entry["text"].asString().size();
    ensure("world message floods are memory bounded", bytes <= 65536 && log.entries().size() <= 128);
    log.clear();
    ensure("clear removes display history", log.entries().empty());
    log.append("session", "private-im", "Unknown", "Not an accepted stream", "12:01");
    ensure("unknown streams are refused", log.entries().empty());
}
template<> template<> void object_t::test<17>()
{
    LLFinalverseMessageLog log;
    log.append("session", "conversation", "World AI", std::string(4095, 'x') + "🌍", "12:00");
    ensure_equals("truncation never splits a UTF-8 glyph", log.entries().back()["text"].asString(), std::string(4095, 'x'));
    log.append("session", "activity", "World", "safe\x1b[2J\nmessage", "12:01");
    ensure("terminal control bytes are removed", log.entries().back()["text"].asString().find(char(27)) == std::string::npos);
}

LLSD sharedPage()
{
    LLSD page,event;
    page["protocol_version"]="1.0"; page["session"]["id"]="11111111-2222-4333-8444-555555555555";
    page["session"]["mode"]="chat"; page["session"]["title"]="Synthetic session";
    page["cursor"]=1; page["artifacts"]=LLSD::emptyArray();
    event["protocol_version"]="1.0"; event["session_id"]=page["session"]["id"];
    event["sequence"]=1; event["request_id"]="fixture"; event["type"]="message.started";
    event["data"]["user_text"]="Synthetic question"; event["data"]["model"]="fixture model";
    page["events"].append(event); return page;
}
template<> template<> void object_t::test<18>()
{
    std::string error; auto page=sharedPage(); auto state=llfinalverseSessionPage(LLSD(),page,error);
    ensure("valid shared session replays",error.empty());
    ensure_equals("human and AI entries",state["messages"].size(),2);
    page["events"][0]["type"]="message.delta"; page["events"][0]["sequence"]=2;
    page["events"][0]["data"]["text"]="Synthetic answer";page["cursor"]=2;
    state=llfinalverseSessionPage(state,page,error);
    ensure_equals("stream delta updates the existing response",state["messages"][1]["text"].asString(),"Synthetic answer");
    ensure_equals("canonical cursor retained",state["cursor"].asInteger(),2);
}
template<> template<> void object_t::test<19>()
{
    std::string error;auto page=sharedPage();auto state=llfinalverseSessionPage(LLSD(),page,error);
    ensure("duplicate replay refused",llfinalverseSessionPage(state,page,error).isUndefined()&&!error.empty());
    page["session"]["id"]="aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee";
    ensure("another session cannot enter this display",llfinalverseSessionPage(state,page,error).isUndefined());
    page=sharedPage();page["session"]["mode"]="code";
    ensure("local code history never enters a shared spatial view",llfinalverseSessionPage(LLSD(),page,error).isUndefined());
}
template<> template<> void object_t::test<20>()
{
    std::string error; auto page=sharedPage();page["events"][0]["data"]["user_text"]=std::string(4096,'x');
    LLSD state;for(S32 n=1;n<=200;++n){page["events"][0]["sequence"]=n;page["cursor"]=n;state=llfinalverseSessionPage(state,page,error);ensure("bounded pages remain valid",error.empty());}
    size_t bytes=0;for(const auto& entry:llsd::inArray(state["messages"]))bytes+=entry["text"].asString().size();
    ensure("display bounded independently of durable history",bytes<=65536&&state["messages"].size()<=128);
}

}
