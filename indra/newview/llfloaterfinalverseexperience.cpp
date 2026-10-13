// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalverseexperience.h"
#include "llfloaterfinalverseai.h"
#include "llbutton.h"
#include "llcommandmanager.h"
#include "llfloaterreg.h"
#include "llfloatersidepanelcontainer.h"
#include "lltextbox.h"
#include "llviewercontrol.h"
#include <map>

void LLFloaterFinalverseExperience::executeCommand(const std::string& name)
{
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
    for (const auto& binding : {std::pair<const char*,const char*>{"movement","move"}, {"camera","view"}, {"map","map"}, {"places","places"}, {"share","snapshot"}, {"objects","build"}, {"inventory","inventory"}, {"environment","myenvironments"}, {"profile","profile"}, {"outfits","appearance"}, {"settings","preferences"}, {"conversations","chat"}, {"performance","performance"}})
    {
        const std::string command(binding.second);
        getChild<LLButton>(binding.first)->setCommitCallback([command](LLUICtrl*, const LLSD&) { executeCommand(command); });
    }
    getChild<LLButton>("friends_list")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterSidePanelContainer::showPanel("people","panel_people",LLSD().with("people_panel_tab_name","friends_panel")); });
    getChild<LLButton>("nearby_people")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterSidePanelContainer::showPanel("people","panel_people",LLSD().with("people_panel_tab_name","nearby_panel")); });
    getChild<LLButton>("talk_lumi")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, who are you?",true); });
    getChild<LLButton>("go_home")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, go home.",true,true); });
    getChild<LLButton>("create_home")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("Create a small lakeside home for Lumi here.",false); });
    getChild<LLButton>("create_garden")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, improve the area around your home with a small garden.",true); });
    getChild<LLButton>("create_anything")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("",false); });
    getChild<LLButton>("create_recipe")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRecipe(); });
    getChild<LLButton>("create_lumi")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("Lumi, create a quiet reading nook nearby.",true); });
    getChild<LLButton>("nearby_world")->setCommitCallback([](LLUICtrl*,const LLSD&) { LLFloaterFinalverseAI::openRequest("What objects are around me?",false,true); });
    for (const char* page : {"explore", "create", "friends"})
    {
        const std::string name(page);
        getChild<LLButton>("welcome_"+name)->setCommitCallback([name](LLUICtrl*,const LLSD&) { openPage(name); });
    }
    getChild<LLButton>("advanced")->setCommitCallback([this](LLUICtrl*,const LLSD&) { gSavedSettings.setBOOL("FinalverseExperienceEnabled",false); closeFloater(); });
    return true;
}
void LLFloaterFinalverseExperience::onOpen(const LLSD& key)
{
    const std::string page = key.asString();
    for (const char* name : {"explore","create","friends","me","tools","welcome"}) getChild<LLPanel>(std::string(name)+"_page")->setVisible(page==name);
    const std::map<std::string,std::string> titles{{"explore","Explore your world"},{"create","What will you create?"},{"friends","A world feels better together"},{"me","Your corner of Finalverse"},{"tools","Creator & developer tools"},{"welcome","Welcome to your living world"}};
    auto title=titles.find(page);
    getChild<LLTextBox>("heading")->setText(title==titles.end()?"Finalverse":title->second);
}
