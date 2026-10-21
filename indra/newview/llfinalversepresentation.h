// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FINALVERSE_PRESENTATION_H
#define LL_FINALVERSE_PRESENTATION_H
#include "llsd.h"
#include <string>
// Human-readable deterministic preview of validated steps. Never fabricates
// recipes or hides deletions. Protocol identifiers belong in technical details.
std::string llfinalversePlanSummary(const LLSD& plan);
// Vendor account benefits are optional on a non-system grid. An advertised
// service still requires its normal error notification if initialization fails.
bool llfinalverseBenefitsExpected(bool system_grid, const LLSD& response);

struct LLFinalverseServices
{
    bool world = false, checking = false, known = false, recovery = false;
    bool compose = false, citizen_present = false, citizen_interactive = false;
    bool server_planning = false;
    bool earth_bridge = false;
    std::string citizen_state;
    bool canCompose() const { return known && compose && !recovery; }
    bool canPlan() const { return world && !checking && !recovery; }
};
// Session-bound discovery is presentation only; native policy remains authority.
LLFinalverseServices llfinalverseParseServices(const LLSD& result, const std::string& region, const std::string& actor);
std::string llfinalverseCitizenUnavailable(const LLFinalverseServices& services);
// Cached lightweight capability request, never a model call or world mutation.
LLFinalverseServices llfinalverseWorldServices();
// Read-only region destinations are accepted only for the active actor/session.
LLSD llfinalverseParseDestinations(const LLSD& result, const std::string& region, const std::string& actor);
// Explicit slash commands use only inherited local actions; never an LLM call.
std::string llfinalverseQuickAction(const std::string& text);
// Search only the curated local navigation/tool catalog, never a remote tool name.
LLSD llfinalverseFindActions(const std::string& query);

#endif
