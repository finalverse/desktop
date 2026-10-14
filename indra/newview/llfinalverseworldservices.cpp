// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalversepresentation.h"
#include "llagent.h"
#include "llviewerregion.h"
#include "llcoros.h"
#include "llcorehttputil.h"
#include "lltimer.h"

LLFinalverseServices llfinalverseWorldServices()
{
    static LLFinalverseServices cached;
    static LLUUID current_region, current_actor;
    static std::string current_capability;
    static U32 generation = 0;
    static bool pending = false, polled = false;
    static LLTimer age;
    auto region = gAgent.getRegion();
    const LLUUID region_id = region ? region->getRegionID() : LLUUID::null;
    const LLUUID actor_id = gAgent.getID();
    const std::string capability = region ? region->getCapability("FinalverseWorld") : "";
    if (region_id != current_region || actor_id != current_actor || capability != current_capability)
    {
        ++generation; cached = LLFinalverseServices(); pending = polled = false;
        current_region = region_id; current_actor = actor_id; current_capability = capability;
    }
    if (region_id.isNull() || actor_id.isNull() || capability.empty()) return cached;
    cached.world = true; // Mature inspection remains usable with older servers.
    if (!pending && (!polled || age.getElapsedTimeF32() >= 30.f))
    {
        pending = polled = true; age.reset();
        const U32 request_generation = generation;
        LLCoros::instance().launch("FinalverseExperienceDiscovery", [region_id, actor_id, capability, request_generation]()
        {
            LLCoreHttpUtil::HttpCoroutineAdapter adapter("FinalverseExperienceDiscovery", LLCore::HttpRequest::DEFAULT_POLICY_ID);
            auto request = std::make_shared<LLCore::HttpRequest>();
            auto options = std::make_shared<LLCore::HttpOptions>(); options->setTimeout(5);
            const LLSD reply = adapter.postJsonAndSuspend(request, capability, LLSD().with("op", "experience"), options);
            // A reply from the old region/session must never enable new controls.
            auto active = gAgent.getRegion();
            if (request_generation != generation || !active || active->getRegionID() != region_id ||
                gAgent.getID() != actor_id || active->getCapability("FinalverseWorld") != capability) return;
            pending = false;
            cached = reply["ok"].isBoolean() && reply["ok"].asBoolean()
                ? llfinalverseParseServices(reply["result"], region_id.asString(), actor_id.asString()) : LLFinalverseServices();
            cached.world = true;
        });
    }
    LLFinalverseServices result = cached; result.checking = pending && !cached.known;
    return result;
}
