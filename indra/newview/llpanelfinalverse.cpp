// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llpanelfinalverse.h"
#include "llfloaterfinalverseai.h"
#include "llfloaterfinalverseexperience.h"
#include "llfloaterfinalverseworkspace.h"
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

LLPanelFinalverse* LLPanelFinalverse::sInstance = nullptr;
LLPanelFinalverse::LLPanelFinalverse() { sInstance = this; buildFromFile("panel_finalverse_hud.xml"); }
LLPanelFinalverse::~LLPanelFinalverse() { if (sInstance == this) sInstance = nullptr; }
bool LLPanelFinalverse::postBuild()
{
    getChild<LLButton>("workspace")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterReg::toggleInstance("finalverse_workspace"); });
    getChild<LLButton>("chat")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterReg::toggleInstance("finalverse_conversation"); });
    getChild<LLButton>("collapse")->setCommitCallback([this](LLUICtrl*, const LLSD&) { gSavedSettings.setBOOL("FinalverseDockCollapsed", !gSavedSettings.getBOOL("FinalverseDockCollapsed")); layout(); });
    getChild<LLButton>("create")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::openPage("create"); });
    getChild<LLButton>("explore")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::executeCommand("destinations"); });
    getChild<LLButton>("location")->setCommitCallback([](LLUICtrl*, const LLSD&) {
        if (auto chat = LLFloaterReg::getTypedInstance<LLFloaterFinalverseConversation>("finalverse_conversation")) chat->openFloater(LLSD("activity"));
    });
    getChild<LLButton>("ask_selected")->setCommitCallback([](LLUICtrl*, const LLSD&) {
        LLFloaterFinalverseAI::openRequest("What is this object?", false);
    });
    getChild<LLButton>("edit_selected")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::executeCommand("build"); });
    getChild<LLButton>("focus_selected")->setCommitCallback([this](LLUICtrl*, const LLSD&) { setViewMode("focus"); });
    getChild<LLButton>("clear_selection")->setCommitCallback([this](LLUICtrl*, const LLSD&) { LLSelectMgr::getInstance()->deselectAll(); mSelection = LLObjectSelectionHandle(); });
    getChild<LLButton>("return_world")->setCommitCallback([this](LLUICtrl*, const LLSD&) { setExperience(true); });
    mSimple = gSavedSettings.getBOOL("FinalverseExperienceEnabled"); layout(); return true;
}
void LLPanelFinalverse::action(const std::string& name)
{
    if (name != "select") { setViewMode(name); return; }
    if (mSelecting) { LLToolMgr::getInstance()->clearTransientTool(); mSelection = LLObjectSelectionHandle(); }
    else { LLSelectMgr::getInstance()->deselectAll(); mSelection = LLSelectMgr::getInstance()->getSelection(); LLToolMgr::getInstance()->setTransientTool(LLToolCompInspect::getInstance()); }
    mSelecting = !mSelecting;
}
void LLPanelFinalverse::reshape(S32 width, S32 height, bool parent)
{
    LLPanel::reshape(width, height, parent);
    if (findChild<LLPanel>("command_surface", false)) layout();
}
void LLPanelFinalverse::layout()
{
    const S32 width = getRect().getWidth(), height = getRect().getHeight();
    const bool collapsed = gSavedSettings.getBOOL("FinalverseDockCollapsed");
    const S32 dock_width = collapsed ? 80 : 332;
    getChild<LLPanel>("command_surface")->setShape(LLRect(12, 46, 12+dock_width, 12));
    getChild<LLButton>("workspace")->setShape(LLRect(6,30,collapsed?50:86,4));
    getChild<LLButton>("workspace")->setLabel(collapsed ? "F" : "Workspace");
    getChild<LLButton>("collapse")->setShape(LLRect(dock_width-30,30,dock_width-6,4));
    getChild<LLButton>("collapse")->setLabel(collapsed ? "+" : "−");
    for (const char* name : {"create","explore","chat"}) getChild<LLButton>(name)->setVisible(!collapsed);
    getChild<LLButton>("location")->setShape(LLRect(12,height-12,llmin(236,width-12),height-38));
    getChild<LLPanel>("selection_surface")->setShape(LLRect(12,132,llmin(292,width-12),56));
    getChild<LLButton>("return_world")->setShape(LLRect(12,height-12,156,height-40));
}
void LLPanelFinalverse::setExperience(bool simple)
{
    gSavedSettings.setBOOL("FinalverseExperienceEnabled", simple);
    mSimple = simple;
    refreshExperience();
}
void LLPanelFinalverse::command(const std::string& text)
{
    const auto quick = llfinalverseQuickAction(text);
    if (quick == "world" || quick == "first_person" || quick == "overview" || quick == "create") setViewMode(quick);
    else if (quick == "help") LLFloaterFinalverseExperience::openPage("tools");
    else if (!quick.empty()) LLFloaterFinalverseExperience::executeCommand(quick);
    else LLFloaterFinalverseExperience::openPage("tools");
}
void LLPanelFinalverse::setViewMode(const std::string& mode)
{
    if (!gAgent.getRegion()) return;
    if (mode == "first_person")
    {
        if (mSelecting)
        {
            LLToolMgr::getInstance()->clearTransientTool(); mSelecting = false;
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
    auto root = gViewerWindow->getRootView();
    root->getChild<LLPanel>("status_bar_container")->setVisible(!mSimple);
    root->getChild<LLPanel>("topinfo_bar_container")->setVisible(!mSimple);
    // Keep the toolbar view itself alive: inherited stand/stop-flying controls,
    // conversation indicators and floater geometry still use its containers.
    if (gToolBarView) gToolBarView->setToolBarsVisible(!mSimple);
    const bool first_person = gAgentCamera.getCameraMode() == CAMERA_MODE_MOUSELOOK;
    if (first_person) mViewMode = "first_person";
    else if (mViewMode == "first_person" || gAgentCamera.getFocusOnAvatar()) mViewMode = "world";
    getChild<LLPanel>("command_surface")->setVisible(mSimple && !first_person);
    getChild<LLButton>("location")->setVisible(mSimple && !first_person && gSavedSettings.getBOOL("FinalverseShowLocation"));
    getChild<LLButton>("return_world")->setVisible(!mSimple);
    const auto region = gAgent.getRegion();
    const std::string name = region ? region->getName() : "Connecting…";
    getChild<LLButton>("location")->setLabel(name);
    getChild<LLButton>("location")->setToolTip(name + " · Open live activity");
    if (region && name != mRegionName) { mRegionName = name; llfinalverseMessage("activity", "World", "Entered " + name + "."); }
    const U64 revision = llfinalverseMessageRevision();
    if (LLFloaterReg::instanceVisible("finalverse_conversation")) mReadRevision = revision;
    getChild<LLButton>("chat")->setLabel(revision > mReadRevision ? "Chat •" : "Chat");
    auto node = LLSelectMgr::getInstance()->getSelection()->getFirstRootNode();
    auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
    getChild<LLPanel>("selection_surface")->setVisible(mSimple && !first_person && object && gSavedSettings.getBOOL("FinalverseShowSelection"));
    if (mSelecting && ((!mSimple) || (object && !LLToolCompInspect::getInstance()->hasMouseCapture())))
    {
        LLToolMgr::getInstance()->clearTransientTool(); mSelecting = false;
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
