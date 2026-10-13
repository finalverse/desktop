// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FLOATER_FINALVERSE_RECIPE_H
#define LL_FLOATER_FINALVERSE_RECIPE_H
#include "llfloater.h"
class LLFloaterFinalverseRecipe : public LLFloater
{
public:
    explicit LLFloaterFinalverseRecipe(const LLSD& key) : LLFloater(key) {}
    bool postBuild() override;
    void onOpen(const LLSD& recipe) override;
private:
    LLSD mRecipe;
    void preview();
};
#endif
