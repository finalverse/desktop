// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalverserecipe.h"
#include "llfloaterfinalverseai.h"
#include "llfinalverseworldrecipe.h"
#include "llbutton.h"
#include "llcombobox.h"
#include "lllineeditor.h"
#include "llspinctrl.h"
#include "lltextbox.h"
bool LLFloaterFinalverseRecipe::postBuild()
{
    getChild<LLButton>("preview")->setCommitCallback([this](LLUICtrl*, const LLSD&) { preview(); });
    getChild<LLButton>("cancel")->setCommitCallback([this](LLUICtrl*, const LLSD&) { closeFloater(); });
    return true;
}
void LLFloaterFinalverseRecipe::onOpen(const LLSD& recipe)
{
    std::string error;
    mRecipe = llfinalverseValidateRecipe(recipe, error) ? recipe : LLSD();
    getChild<LLButton>("preview")->setEnabled(mRecipe.isMap());
    if (!mRecipe.isMap()) { getChild<LLTextBox>("status")->setText(error); return; }
    getChild<LLLineEditor>("title")->setText(mRecipe["place"]["title"].asString());
    getChild<LLSpinCtrl>("benches")->setValue(mRecipe["place"]["seating"]["count"]);
    for (const char* field : {"size", "planting", "lighting"}) getChild<LLComboBox>(field)->setValue(mRecipe["place"][field]);
    getChild<LLComboBox>("site")->setValue(mRecipe.has("site") ? mRecipe["site"] : LLSD("in_front_of_actor"));
    getChild<LLTextBox>("status")->setText(std::string("Make a variation. Preview checks the layout and placement before you approve."));
}
void LLFloaterFinalverseRecipe::preview()
{
    LLSD remix = mRecipe;
    remix["place"]["title"] = getChild<LLLineEditor>("title")->getText();
    remix["place"]["seating"]["count"] = (S32)getChild<LLSpinCtrl>("benches")->getValue().asInteger();
    for (const char* field : {"size", "planting", "lighting"}) remix["place"][field] = getChild<LLComboBox>(field)->getValue();
    const LLSD site = getChild<LLComboBox>("site")->getValue();
    if (mRecipe.has("site") || site.asString() != "in_front_of_actor")
    { remix["schema_version"] = 2; remix["site"] = site; }
    std::string error;
    if (!llfinalverseValidateRecipe(remix, error)) { getChild<LLTextBox>("status")->setText(error); return; }
    if (LLFloaterFinalverseAI::previewRecipe(remix)) closeFloater();
    else getChild<LLTextBox>("status")->setText(std::string("Finish the current operation before previewing this remix."));
}
