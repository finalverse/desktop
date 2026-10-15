// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "../llviewerprecompiledheaders.h"
#include "../llfinalversepresentation.h"
#include "../llfinalverselegacyappearance.h"
#include "../test/lltut.h"
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

}
