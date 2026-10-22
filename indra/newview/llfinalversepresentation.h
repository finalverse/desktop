// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FINALVERSE_PRESENTATION_H
#define LL_FINALVERSE_PRESENTATION_H
#include "llsd.h"
#include <string>
#include <deque>

// Ephemeral display history only: never sent as model context or written to disk.
// A new native login clears the previous account's messages.
class LLFinalverseMessageLog
{
public:
    void session(const std::string& id);
    void append(const std::string& id, const std::string& channel,
                const std::string& speaker, const std::string& text, const std::string& time);
    void clear();
    const std::deque<LLSD>& entries() const { return mEntries; }
    U64 revision() const { return mRevision; }
private:
    std::string mSession;
    std::deque<LLSD> mEntries;
    size_t mBytes = 0;
    U64 mRevision = 0;
};
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



// Pure display projection of authenticated canonical session pages. No execution.
LLSD llfinalverseSessionPage(const LLSD& state, const LLSD& page, std::string& error);

#endif
