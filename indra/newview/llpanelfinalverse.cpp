// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llpanelfinalverse.h"
#include "llfloaterfinalverseai.h"
#include "llfloaterfinalverseexperience.h"
#include "llfinalversepresentation.h"
#include "llagent.h"
#include "llbutton.h"
#include "llfloaterreg.h"
#include "lllineeditor.h"
#include "llselectmgr.h"
#include "lltextbox.h"
#include "lltoolcomp.h"
#include "lltoolmgr.h"
#include "lltoolbarview.h"
#include "llviewercontrol.h"
#include "llviewerobject.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llrootview.h"

LLPanelFinalverse::LLPanelFinalverse() { buildFromFile("panel_finalverse_hud.xml"); }
bool LLPanelFinalverse::postBuild()
{
    for (const char* page : {"explore", "create", "friends", "me", "tools", "welcome"})
    {
        const std::string name(page);
        getChild<LLButton>(name)->setCommitCallback([name](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::openPage(name); });
    }
    getChild<LLButton>("lumi")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, who are you?", true); });
    getChild<LLButton>("send")->setCommitCallback([this](LLUICtrl*, const LLSD&) { submit(); });
    getChild<LLLineEditor>("command")->setCommitCallback([this](LLUICtrl*, const LLSD&) { submit(); });
    getChild<LLButton>("ask_selected")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseAI::openRequest("What is this object?", false); });
    getChild<LLButton>("edit_selected")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::executeCommand("build"); });
    getChild<LLButton>("select_object")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        if (mSelecting)
        {
            LLToolMgr::getInstance()->clearTransientTool();
            mSelection = LLObjectSelectionHandle();
        }
        else
        {
            LLSelectMgr::getInstance()->deselectAll();
            mSelection = LLSelectMgr::getInstance()->getSelection();
            LLToolMgr::getInstance()->setTransientTool(LLToolCompInspect::getInstance());
        }
        mSelecting = !mSelecting;
        getChild<LLButton>("select_object")->setLabel(mSelecting ? "Cancel" : "Select");
    });
    getChild<LLButton>("clear_selection")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        LLSelectMgr::getInstance()->deselectAll();
        mSelection = LLObjectSelectionHandle();
    });
    getChild<LLButton>("return_world")->setCommitCallback([this](LLUICtrl*, const LLSD&) { setExperience(true); });
    mSimple = gSavedSettings.getBOOL("FinalverseExperienceEnabled");
    layout();
    return true;
}
void LLPanelFinalverse::reshape(S32 width, S32 height, bool parent)
{
    LLPanel::reshape(width, height, parent);
    if (findChild<LLPanel>("command_surface", false)) layout();
}
void LLPanelFinalverse::layout()
{
    const S32 width = getRect().getWidth(), height = getRect().getHeight();
    const S32 command_width = llmax(240, llmin(620, width - 32));
    auto command = getChild<LLPanel>("command_surface");
    command->setShape(LLRect((width-command_width)/2, 120, (width+command_width)/2, 16));
    getChild<LLLineEditor>("command")->setShape(LLRect(16, 84, command_width-104, 48));
    getChild<LLButton>("send")->setShape(LLRect(command_width-96, 84, command_width-16, 48));
    const S32 nav_width = command_width-32, item_width = nav_width/(mLumiAvailable ? 5 : 4);
    S32 x = 16;
    for (const char* name : {"explore", "create", "lumi", "friends", "me"})
    {
        const bool visible = std::string(name) != "lumi" || mLumiAvailable;
        getChild<LLButton>(name)->setVisible(visible);
        if (visible) { getChild<LLButton>(name)->setShape(LLRect(x, 40, x+item_width-4, 8)); x += item_width; }
    }
    getChild<LLPanel>("identity_surface")->setShape(LLRect(16, height-16, llmin(320, width-16), height-96));
    getChild<LLPanel>("utility_surface")->setShape(LLRect(llmax(16,width-344), height-16, width-16, height-60));
    getChild<LLPanel>("selection_surface")->setShape(LLRect(16, 230, llmin(350,width-16), 138));
    getChild<LLButton>("return_world")->setShape(LLRect(16,height-16,176,height-52));
}
void LLPanelFinalverse::setExperience(bool simple)
{
    gSavedSettings.setBOOL("FinalverseExperienceEnabled", simple);
    mSimple = simple;
    refreshExperience();
}
void LLPanelFinalverse::submit()
{
    const std::string text = getChild<LLLineEditor>("command")->getText();
    if (text.empty()) return;
    auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
    const bool citizen = (object && object->getID() == LLUUID("b151468b-cbcc-4af2-ae2d-6967e8938bb4"))
        || text.compare(0, 4, "Lumi") == 0 || text.compare(0, 4, "lumi") == 0;
    LLFloaterFinalverseAI::openRequest(text, citizen, true);
}
void LLPanelFinalverse::refreshExperience()
{
    mSimple = gSavedSettings.getBOOL("FinalverseExperienceEnabled");
    const bool lumi = llfinalverseWorldServices().citizen_interactive;
    if (lumi != mLumiAvailable) { mLumiAvailable = lumi; layout(); }
    auto root = gViewerWindow->getRootView();
    root->getChild<LLPanel>("status_bar_container")->setVisible(!mSimple);
    root->getChild<LLPanel>("topinfo_bar_container")->setVisible(!mSimple);
    // Keep the toolbar view itself alive: inherited stand/stop-flying controls,
    // conversation indicators and floater geometry still use its containers.
    if (gToolBarView) gToolBarView->setToolBarsVisible(!mSimple);
    for (const char* name : {"identity_surface", "utility_surface", "command_surface"}) getChild<LLPanel>(name)->setVisible(mSimple);
    getChild<LLButton>("return_world")->setVisible(!mSimple);
    const auto region = gAgent.getRegion();
    getChild<LLTextBox>("location")->setText(region ? region->getName() : "Connecting to your world…");
    getChild<LLTextBox>("session_state")->setText(std::string(gAgent.getGodLevel() > 0 ? "Administrator session" : "A living world. Yours to explore."));
    auto node = LLSelectMgr::getInstance()->getSelection()->getFirstRootNode();
    auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
    getChild<LLPanel>("selection_surface")->setVisible(mSimple && object);
    if (mSelecting && ((!mSimple) || (object && !LLToolCompInspect::getInstance()->hasMouseCapture())))
    {
        LLToolMgr::getInstance()->clearTransientTool(); mSelecting = false;
        getChild<LLButton>("select_object")->setLabel("Select");
    }
    if (!mSimple || (!mSelecting && !object)) mSelection = LLObjectSelectionHandle();
    if (object)
    {
        const bool lumi = object->getID() == LLUUID("b151468b-cbcc-4af2-ae2d-6967e8938bb4");
        getChild<LLTextBox>("selected_name")->setText(lumi ? "Lumi · AI citizen" : node && !node->mName.empty() ? node->mName : "Selected object");
        getChild<LLButton>("ask_selected")->setCommitCallback([lumi](LLUICtrl*, const LLSD&) { LLFloaterFinalverseAI::openRequest(lumi ? "Lumi, who are you?" : "What is this object?", lumi); });
    }
}
void LLPanelFinalverse::draw()
{
    static LLCachedControl<bool> measure(gSavedSettings, "FinalverseUXMetrics");
    if (!measure) { drawExperience(); return; }
    LLTimer timer;
    drawExperience();
    mDrawSeconds += timer.getElapsedTimeF64();
    if (++mDrawSamples == 300)
    {
        LL_INFOS("FinalverseUX") << "hud_draw_cpu_ms=" << (mDrawSeconds*1000/mDrawSamples)
                                << " samples=" << mDrawSamples << " compact=" << mSimple << LL_ENDL;
        mDrawSamples = 0; mDrawSeconds = 0.;
    }
}
void LLPanelFinalverse::drawExperience()
{
    if (mRefresh.getElapsedTimeF32() >= .5f) { refreshExperience(); mRefresh.reset(); }
    LLPanel::draw();
}
