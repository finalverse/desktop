// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FLOATER_FINALVERSE_EXPERIENCE_H
#define LL_FLOATER_FINALVERSE_EXPERIENCE_H
#include "llfloater.h"
#include "lltimer.h"
class LLFloaterFinalverseExperience : public LLFloater
{
public:
    explicit LLFloaterFinalverseExperience(const LLSD& key) : LLFloater(key) {}
    bool postBuild() override;
    void onOpen(const LLSD& key) override;
    void draw() override;
    static void openPage(const std::string& page);
    static void executeCommand(const std::string& name);
private:
    void refreshServices();
    LLTimer mServicesRefresh;
};
#endif
