// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FLOATER_FINALVERSE_WORKSPACE_H
#define LL_FLOATER_FINALVERSE_WORKSPACE_H
#include "llfloater.h"
#include "lltimer.h"
void llfinalverseMessage(const std::string& channel, const std::string& speaker, const std::string& text);
U64 llfinalverseMessageRevision();
class LLFloaterFinalverseWorkspace : public LLFloater
{
public:
    explicit LLFloaterFinalverseWorkspace(const LLSD& key) : LLFloater(key) {}
    bool postBuild() override;
    void draw() override;
private:
    LLTimer mRefresh;
};
class LLFloaterFinalverseConversation : public LLFloater
{
public:
    explicit LLFloaterFinalverseConversation(const LLSD& key) : LLFloater(key) {}
    bool postBuild() override;
    void draw() override;
    void onOpen(const LLSD& key) override;
private:
    void send();
    bool mActivity = false;
    U64 mRevision = 0;
    LLTimer mRefresh;
};
#endif
