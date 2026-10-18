// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalverseexperience.h"
#include "llfloaterfinalverseai.h"
#include "llfinalversepresentation.h"
#include "lliconctrl.h"
#include "llbutton.h"
#include "llcommandmanager.h"
#include "llfloaterreg.h"
#include "llfloatersidepanelcontainer.h"
#include "lltextbox.h"
#include "lllineeditor.h"
#include "llscrolllistctrl.h"
#include "llviewercontrol.h"
#include <map>

namespace
{
bool actionAvailable(const std::string& name)
{
    if (name == "friends" || name == "nearby") return true;
    auto command = LLCommandManager::instance().getCommand(LLCommandId(name));
    if (!command || !LLUICtrl::CommitCallbackRegistry::getValue(command->executeFunctionName())) return false;
    auto enabled = LLUICtrl::EnableCallbackRegistry::getValue(command->isEnabledFunctionName());
    return !enabled || (*enabled)(nullptr,command->isEnabledParameters());
}
}

void LLFloaterFinalverseExperience::executeCommand(const std::string& name)
{
    if (name == "friends" || name == "nearby")
    {
        LLFloaterSidePanelContainer::showPanel("people","panel_people",LLSD().with("people_panel_tab_name",name == "friends" ? "friends_panel" : "nearby_panel"));
        return;
    }
    auto command = LLCommandManager::instance().getCommand(LLCommandId(name));
    if (!command) return;
    auto enabled = LLUICtrl::EnableCallbackRegistry::getValue(command->isEnabledFunctionName());
    if (enabled && !(*enabled)(nullptr, command->isEnabledParameters())) return;
    auto execute = LLUICtrl::CommitCallbackRegistry::getValue(command->executeFunctionName());
    if (execute) (*execute)(nullptr, command->executeParameters());
}
void LLFloaterFinalverseExperience::openPage(const std::string& page)
{
    LLTimer timer;
    // Page names are navigation state, not separate floater identities.
    // A registry key per page would stack duplicate facade windows.
    auto self = LLFloaterReg::getTypedInstance<LLFloaterFinalverseExperience>("finalverse_experience");
    if (self) self->openFloater(LLSD(page));
    if (gSavedSettings.getBOOL("FinalverseUXMetrics")) LL_INFOS("FinalverseUX") << "experience_open_ms=" << timer.getElapsedTimeF64().value()*1000 << " page=" << page << LL_ENDL;
}
bool LLFloaterFinalverseExperience::postBuild()
{
    for (const auto& binding : {std::pair<const char*,const char*>{"movement","move"}, {"camera","view"}, {"map","map"}, {"world_library","destinations"}, {"places","places"}, {"share","snapshot"}, {"inventory","inventory"}, {"profile","profile"}, {"outfits","appearance"}, {"settings","preferences"}, {"conversations","chat"}})
    {
        const std::string command(binding.second);
        getChild<LLButton>(binding.first)->setCommitCallback([command](LLUICtrl*, const LLSD&) { executeCommand(command); });
    }
    getChild<LLButton>("friends_list")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterSidePanelContainer::showPanel("people","panel_people",LLSD().with("people_panel_tab_name","friends_panel")); });
    getChild<LLButton>("nearby_people")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterSidePanelContainer::showPanel("people","panel_people",LLSD().with("people_panel_tab_name","nearby_panel")); });
    getChild<LLButton>("talk_lumi")->setCommitCallback([](LLUICtrl*,const LLSD&) {
        if (llfinalverseWorldServices().citizen_interactive) LLFloaterFinalverseAI::openRequest("Lumi, who are you?",true);
        else LLFloaterFinalverseAI::openNearby();
    });
    getChild<LLButton>("go_home")->setCommitCallback([](LLUICtrl*,const LLSD&) {
        if (llfinalverseWorldServices().citizen_interactive) LLFloaterFinalverseAI::openRequest("Lumi, go home.",true,true);
        else executeCommand("appearance");
    });
    getChild<LLButton>("create_home")->setCommitCallback([](LLUICtrl*,const LLSD&) {
        LLFloaterFinalverseAI::openRequest(llfinalverseWorldServices().citizen_interactive ? "Create a small lakeside home for Lumi here." : "Create a small reading nook nearby with one bench and warm path lighting.",false);
    });
    getChild<LLButton>("create_garden")->setCommitCallback([](LLUICtrl*,const LLSD&) {
        const bool citizen = llfinalverseWorldServices().citizen_interactive;
        LLFloaterFinalverseAI::openRequest(citizen ? "Lumi, improve the area around your home with a small garden." : "Create a small garden nearby with one bench, shrubs and warm path lighting.",citizen);
    });
    getChild<LLButton>("create_anything")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("",false); });
    getChild<LLButton>("create_recipe")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRecipe(); });
    getChild<LLButton>("create_lumi")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, create a quiet reading nook nearby.",true); });
    getChild<LLButton>("nearby_world")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openNearby(); });
    for (const char* page : {"explore", "create", "friends"})
    {
        const std::string name(page);
        getChild<LLButton>("welcome_"+name)->setCommitCallback([name](LLUICtrl*,const LLSD&) { openPage(name); });
    }
    getChild<LLButton>("advanced")->setCommitCallback([this](LLUICtrl*,const LLSD&) { gSavedSettings.setBOOL("FinalverseExperienceEnabled",false); closeFloater(); });
    getChild<LLLineEditor>("action_search")->setKeystrokeCallback([this](LLLineEditor*,void*) { filterActions(); },nullptr);
    getChild<LLLineEditor>("action_search")->setCommitCallback([this](LLUICtrl*,const LLSD&) { openAction(); });
    getChild<LLScrollListCtrl>("action_results")->setDoubleClickCallback([this] { openAction(); });
    getChild<LLButton>("action_open")->setCommitCallback([this](LLUICtrl*,const LLSD&) { openAction(); });
    return true;
}
void LLFloaterFinalverseExperience::onOpen(const LLSD& key)
{
    const std::string page = key.asString();
    for (const char* name : {"explore","create","friends","me","tools","welcome"}) getChild<LLPanel>(std::string(name)+"_page")->setVisible(page==name);
    getChild<LLPanel>("lumi_card")->setVisible(page != "tools");
    getChild<LLTextBox>("intro")->setVisible(page != "tools");
    const std::map<std::string,std::string> titles{{"explore","Explore your world"},{"create","What will you create?"},{"friends","A world feels better together"},{"me","Your corner of Finalverse"},{"tools","Find an action"},{"welcome","Welcome to your living world"}};
    auto title=titles.find(page);
    getChild<LLTextBox>("heading")->setText(title==titles.end()?"Finalverse":title->second);
    refreshServices();
    if (page == "tools")
    {
        auto search = getChild<LLLineEditor>("action_search"); search->setText(std::string());
        filterActions(); search->setFocus(true);
    }
}

void LLFloaterFinalverseExperience::filterActions()
{
    auto list = getChild<LLScrollListCtrl>("action_results"); list->deleteAllItems();
    const auto actions = llfinalverseFindActions(getChild<LLLineEditor>("action_search")->getText());
    for (const auto& action : llsd::inArray(actions))
    {
        LLSD row; row["value"] = action["id"]; row["columns"][0]["column"] = "title"; row["columns"][0]["value"] = action["title"];
        row["columns"][1]["column"] = "state"; row["columns"][1]["value"] = actionAvailable(action["id"].asString()) ? "" : "Unavailable";
        list->addElement(row);
    }
    list->setCommentText("No matching action. Try inventory, outfits, lighting or map.");
    list->selectFirstItem();
    getChild<LLTextBox>("tools_description")->setText(std::string("Type to filter, then Return to open. Local tools: no AI call. Native permissions still apply."));
}

void LLFloaterFinalverseExperience::openAction()
{
    auto item = getChild<LLScrollListCtrl>("action_results")->getFirstSelected(); if (!item) return;
    const auto id = item->getValue().asString();
    if (!actionAvailable(id))
    {
        getChild<LLTextBox>("tools_description")->setText(std::string("This action is unavailable in the current context. Choose another action or return to World view."));
        return;
    }
    // Never dispatch search text or server-provided command identifiers.
    const auto actions = llfinalverseFindActions(std::string());
    for (const auto& action : llsd::inArray(actions)) if (action["id"].asString() == id)
    {
        executeCommand(id); closeFloater(); return;
    }
}

void LLFloaterFinalverseExperience::refreshServices()
{
    const auto services = llfinalverseWorldServices();
    const bool citizen = services.citizen_interactive;
    getChild<LLIconCtrl>("lumi_portrait")->setValue(citizen || services.citizen_present ? "first_login_image" : "SL_Logo");
    getChild<LLTextBox>("lumi_name")->setText(std::string(citizen ? "Lumi · AI citizen" : services.citizen_present ? "Lumi · local citizen" : "Your first creation"));
    getChild<LLButton>("talk_lumi")->setLabel(citizen ? "Talk to Lumi" : "What’s nearby?");
    getChild<LLButton>("talk_lumi")->setEnabled(services.world);
    getChild<LLButton>("go_home")->setLabel(citizen ? "Go home" : "My appearance");
    getChild<LLButton>("go_home")->setEnabled(!citizen || services.canPlan());
    getChild<LLButton>("create_home")->setLabel(citizen ? "A home for Lumi" : "A reading nook");
    getChild<LLButton>("create_garden")->setLabel(citizen ? "Lumi’s garden" : "A small garden");
    for (const char* name : {"create_home", "create_garden"}) getChild<LLButton>(name)->setEnabled(citizen ? services.canPlan() : services.canCompose());
    getChild<LLButton>("create_lumi")->setVisible(citizen);
    getChild<LLButton>("create_lumi")->setEnabled(services.canCompose());
    getChild<LLTextBox>("create_hint")->setVisible(!citizen);
    getChild<LLTextBox>("create_hint")->setText(std::string(services.canCompose() ? "Start small on clear ground. Review the plan, then build. You can inspect and undo your creation." : services.checking ? "Checking this world’s creation tools…" : "Composed places are unavailable here. You can explore, inspect objects and use the existing creator tools."));
    getChild<LLButton>("create_anything")->setEnabled(services.canPlan());
    getChild<LLButton>("create_recipe")->setEnabled(services.canCompose());
    getChild<LLButton>("nearby_world")->setEnabled(services.world);
    getChild<LLTextBox>("intro")->setText(std::string(services.recovery ? "World changes are paused for recovery. Exploration is still available." : citizen ? "A living world for creators, friends and AI citizens." : "Make a place worth returning to. Start with a small creation."));
    getChild<LLTextBox>("friends_description")->setText(std::string(citizen ? "Find people, start a conversation, and spend time together. Lumi is here too." : "Find people, start a conversation, and spend time together."));
    getChild<LLTextBox>("welcome_description")->setText(std::string(citizen ? "1 · Explore with arrows or W/A/S/D.\n2 · Meet Lumi and ask about the world.\n3 · Open Create, review a small change, then build." : "1 · Explore with arrows or W/A/S/D.\n2 · Open Me to see your avatar and outfits.\n3 · Open Create, review a small change, then build."));
    getChild<LLButton>("talk_lumi")->setToolTip(citizen ? "Talk through your authorized citizen relay." : services.citizen_present ? llfinalverseCitizenUnavailable(services) : "Read nearby objects directly from the world; no model call.");
}

void LLFloaterFinalverseExperience::draw()
{
    if (mServicesRefresh.getElapsedTimeF32() >= .5f) { refreshServices(); mServicesRefresh.reset(); }
    if (getChild<LLPanel>("tools_page")->getVisible())
    {
        auto selected = getChild<LLScrollListCtrl>("action_results")->getFirstSelected();
        getChild<LLButton>("action_open")->setEnabled(selected && actionAvailable(selected->getValue().asString()));
    }
    LLFloater::draw();
}
