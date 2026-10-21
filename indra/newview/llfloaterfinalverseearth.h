// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FLOATER_FINALVERSE_EARTH_H
#define LL_FLOATER_FINALVERSE_EARTH_H
#include "llfloater.h"
class LLFloaterFinalverseEarth : public LLFloater
{
public:
    explicit LLFloaterFinalverseEarth(const LLSD& key):LLFloater(key){}
    bool postBuild() override;
    void onOpen(const LLSD& key) override;
private:
    void run(const std::string& action);
    void render(const LLSD& result);
    void buttons();
    bool mBusy=false;
    LLUUID mOwner,mRegion;
    std::string mPlan,mStatus;
    LLSD mDestination;
};
#endif
