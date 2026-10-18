// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llpanelfinalverse.h"
#include "llfloaterfinalverseai.h"
#include "llfloaterfinalverseexperience.h"
#include "llfinalversepresentation.h"
#include "llagent.h"
#include "llagentcamera.h"
#include "llbutton.h"
#include "llfloaterreg.h"
#include "llfloatersidepanelcontainer.h"
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
#include "lluicolortable.h"

LLPanelFinalverse::LLPanelFinalverse() { buildFromFile("panel_finalverse_hud.xml"); }
bool LLPanelFinalverse::postBuild()
{
    for (const char* page : {"create", "me", "tools", "welcome"})
    {
        const std::string name(page);
        getChild<LLButton>(name)->setCommitCallback([name](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::openPage(name); });
    }
    getChild<LLButton>("explore")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseExperience::executeCommand("destinations"); });
    getChild<LLButton>("friends")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseExperience::executeCommand("friends"); });
    getChild<LLButton>("lumi")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, who are you?", true); });
    getChild<LLButton>("send")->setCommitCallback([this](LLUICtrl*, const LLSD&) { submit(); });
    getChild<LLLineEditor>("command")->setCommitCallback([this](LLUICtrl*, const LLSD&) { submit(); });
    getChild<LLButton>("ask_selected")->setCommitCallback([](LLUICtrl*, const LLSD&) {
        auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
        if (!object) return;
        const bool lumi = object->getID() == LLUUID("b151468b-cbcc-4af2-ae2d-6967e8938bb4");
        LLFloaterFinalverseAI::openRequest(lumi ? "Lumi, who are you?" : "What is this object?", lumi);
    });
    getChild<LLButton>("edit_selected")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::executeCommand("build"); });
    getChild<LLButton>("focus_selected")->setCommitCallback([this](LLUICtrl*, const LLSD&) { setViewMode("focus"); });
    for (const char* mode : {"world", "first_person", "overview", "create"})
    {
        const std::string name(mode);
        getChild<LLButton>("view_"+name)->setCommitCallback([this,name](LLUICtrl*,const LLSD&) { setViewMode(name); });
    }
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
    const S32 view_left = width >= 1024 ? 320 + (width-344-320-332)/2 : (width-332)/2;
    const S32 view_top = height - (width >= 1024 ? 16 : 108);
    getChild<LLPanel>("view_surface")->setShape(LLRect(view_left,view_top,view_left+332,view_top-44));
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
    const auto action = llfinalverseQuickAction(text);
    if (!action.empty())
    {
        if (action == "world" || action == "first_person" || action == "overview" || action == "create") setViewMode(action);
        else if (action == "help") LLFloaterFinalverseExperience::openPage("tools");
        else if (action == "friends") LLFloaterSidePanelContainer::showPanel("people","panel_people",LLSD().with("people_panel_tab_name","friends_panel"));
        else LLFloaterFinalverseExperience::executeCommand(action);
        getChild<LLLineEditor>("command")->setText(std::string());
        return;
    }
    std::string trimmed(text); LLStringUtil::trim(trimmed);
    if (!trimmed.empty() && trimmed[0] == '/') { LLFloaterFinalverseExperience::openPage("tools"); return; }
    auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
    const bool citizen = (object && object->getID() == LLUUID("b151468b-cbcc-4af2-ae2d-6967e8938bb4"))
        || text.compare(0, 4, "Lumi") == 0 || text.compare(0, 4, "lumi") == 0;
    LLFloaterFinalverseAI::openRequest(text, citizen, true);
}
void LLPanelFinalverse::setViewMode(const std::string& mode)
{
    if (!gAgent.getRegion()) return;
    if (mode == "first_person")
    {
        if (mSelecting)
        {
            LLToolMgr::getInstance()->clearTransientTool(); mSelecting = false;
            getChild<LLButton>("select_object")->setLabel("Select");
        }
        mViewMode = mode;
        gAgentCamera.changeCameraToMouselook();
        return;
    }
    gAgentCamera.changeCameraToThirdPerson();
    if (mode == "world")
    {
        gAgentCamera.setFocusOnAvatar(true,true);
        mViewMode = mode;
        return;
    }
    const bool selected = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject() != nullptr;
    if (mode == "focus" || (mode == "create" && selected))
    {
        if (selected)
        {
            auto focus = LLUICtrl::CommitCallbackRegistry::getValue("Tools.LookAtSelection");
            if (focus) (*focus)(nullptr,LLSD("zoom"));
        }
    }
    else
    {
        const LLVector3d focus = gAgent.getPositionGlobal();
        const LLVector3d offset = mode == "overview" ? LLVector3d(-32.,-40.,72.) : LLVector3d(-8.,-10.,7.);
        gAgentCamera.setFocusOnAvatar(false,true);
        gAgentCamera.setCameraPosAndFocusGlobal(focus+offset,focus,LLUUID::null);
    }
    mViewMode = mode;
    if (mode == "create") LLFloaterFinalverseExperience::openPage("create");
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
    const bool first_person = gAgentCamera.getCameraMode() == CAMERA_MODE_MOUSELOOK;
    if (first_person) mViewMode = "first_person";
    else if (mViewMode == "first_person" || gAgentCamera.getFocusOnAvatar()) mViewMode = "world";
    for (const char* name : {"identity_surface", "utility_surface", "command_surface", "view_surface"}) getChild<LLPanel>(name)->setVisible(mSimple);
    if (first_person)
        for (const char* name : {"identity_surface", "utility_surface", "command_surface"}) getChild<LLPanel>(name)->setVisible(false);
    for (const char* mode : {"world", "first_person", "overview", "create"})
    {
        const bool active = mViewMode == mode;
        getChild<LLButton>(std::string("view_")+mode)->setImageColor(LLUIColorTable::instance().getColor(active ? "FinalverseAccent" : "FinalverseSurface"));
        getChild<LLButton>(std::string("view_")+mode)->setUnselectedLabelColor(LLUIColorTable::instance().getColor(active ? "FinalverseBackground" : "FinalverseText"));
        getChild<LLButton>(std::string("view_")+mode)->setSelectedLabelColor(LLUIColorTable::instance().getColor(active ? "FinalverseBackground" : "FinalverseText"));
    }
    getChild<LLButton>("return_world")->setVisible(!mSimple);
    const auto region = gAgent.getRegion();
    getChild<LLTextBox>("location")->setText(region ? region->getName() : "Connecting to your world…");
    getChild<LLTextBox>("session_state")->setText(std::string(gAgent.getGodLevel() > 0 ? "Administrator session" : "A living world. Yours to explore."));
    auto node = LLSelectMgr::getInstance()->getSelection()->getFirstRootNode();
    auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
    getChild<LLPanel>("selection_surface")->setVisible(mSimple && !first_person && object);
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
