// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FLOATER_FINALVERSE_AI_H
#define LL_FLOATER_FINALVERSE_AI_H
#include "llfloater.h"
#include "lltimer.h"

class LLFloaterFinalverseAI : public LLFloater
{
public:
    explicit LLFloaterFinalverseAI(const LLSD& key) : LLFloater(key) {}
    bool postBuild() override;
    void draw() override;
    static void openRequest(const std::string& text, bool citizen, bool submit = false);
    static void openRecipe();
    static void openNearby();
    static bool previewRecipe(const LLSD& recipe);
private:
    void run(const std::string& operation);
    void cancel();
    void busy(bool active, bool mutation = false);
    void show(const std::string& text, const std::string& details = "");
    void renderActivity();
    void loadRecipe();
    void saveRecipe();
    LLSD mRecipe;
    U32 mGeneration = 0;
    bool mBusy = false;
    bool mMutation = false;
    LLSD mPrepared;
    LLSD mCitizenPrepared;
    LLUUID mPlanSelection;
    std::string mHumanText;
    std::string mTechnicalText;
    bool mActivityDirty = false;
    std::string mLastPlan;
    LLTimer mServicesRefresh;
};
#endif
