// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalverseai.h"
#include "llfloaterfinalverseworkspace.h"
#include "llfinalversepresentation.h"
#include "llfinalverseworldrecipe.h"
#include "llviewermenufile.h"
#include "llnotificationsutil.h"
#include "llfloaterreg.h"
#include "lltextbox.h"
#include "llviewercontrol.h"
#include "llagent.h"
#include "llviewerregion.h"
#include "llselectmgr.h"
#include "llviewerobject.h"
#include "lllineeditor.h"
#include "lltexteditor.h"
#include "llbutton.h"
#include "llcheckboxctrl.h"
#include "llcoros.h"
#include "llcorehttputil.h"
#include <iomanip>
#include <sstream>

namespace
{
LLSD post(const std::string& url, const LLSD& body)
{
    LLCoreHttpUtil::HttpCoroutineAdapter adapter("FinalverseWorld", LLCore::HttpRequest::DEFAULT_POLICY_ID);
    auto request = std::make_shared<LLCore::HttpRequest>();
    auto options = std::make_shared<LLCore::HttpOptions>();
    options->setTimeout(110);
    // A dropped response must never automatically replay a world mutation.
    options->setRetries(0);
    LLSD result = adapter.postJsonAndSuspend(request, url, body, options);
    auto status = LLCoreHttpUtil::HttpCoroutineAdapter::getStatusFromLLSD(result[LLCoreHttpUtil::HttpCoroutineAdapter::HTTP_RESULTS]);
    if (!status && !result.has("error")) result["error"] = "The service is unavailable. No result was confirmed; inspect history before retrying.";
    result.erase(LLCoreHttpUtil::HttpCoroutineAdapter::HTTP_RESULTS);
    if (body.has("op") && !result.has("error") &&
        (!result["ok"].isBoolean() || !result["ok"].asBoolean() || !result.has("result")))
        result["error"] = "Your world session ended or returned an incomplete response. Enter the world again before planning.";
    return result;
}
LLSD modelContext(const LLSD& world)
{
    LLSD result;
    for (const char* key : {"region", "avatar", "terrain"}) result[key] = world[key];
    auto entity = [](const LLSD& source)
    {
        LLSD value;
        if (!source.isMap()) return value;
        for (const char* key : {"id", "name", "shape", "position", "scale", "editable"}) value[key] = source[key];
        return value;
    };
    result["selected_object"] = entity(world["selected_object"]);
    result["nearby_entities"] = LLSD::emptyArray();
    for (const auto& item : llsd::inArray(world["nearby_entities"]))
    {
        LLSD next; next["entity"] = entity(item["entity"]); next["distance"] = item["distance"];
        result["nearby_entities"].append(next);
    }
    return result;
}
void describe(std::ostringstream& text, const LLSD& value)
{
    text << "  ID: " << value["id"].asString() << "\n";
    for (const char* key : {"position", "scale", "rotation", "color"})
    {
        text << "  " << key << ": ";
        bool first = true;
        for (const auto& number : llsd::inArray(value[key]))
        {
            if (!first) text << ", ";
            text << std::fixed << std::setprecision(3) << number.asReal();
            first = false;
        }
        text << "\n";
    }
    text << "  Shape: " << value["shape"].asString() << "\n  Description: " << value["description"].asString() << "\n";
}
}

bool LLFloaterFinalverseAI::postBuild()
{
    getChild<LLButton>("release_save")->setCommitCallback([this](LLUICtrl*, const LLSD&) { saveRelease(); });
    getChild<LLButton>("ask")->setCommitCallback([this](LLUICtrl*, const LLSD&) { run(getChild<LLCheckBoxCtrl>("citizen_mode")->get() ? "citizen_say" : "plan"); });
    getChild<LLButton>("inspect")->setCommitCallback([this](LLUICtrl*, const LLSD&) { run("inspect"); });
    getChild<LLButton>("build")->setCommitCallback([this](LLUICtrl*, const LLSD&) { run("commit"); });
    getChild<LLButton>("cancel")->setCommitCallback([this](LLUICtrl*, const LLSD&) { cancel(); });
    getChild<LLButton>("undo")->setCommitCallback([this](LLUICtrl*, const LLSD&) { if (getChild<LLCheckBoxCtrl>("citizen_mode")->get()) saveRecipe(); else run("undo"); });
    getChild<LLButton>("history")->setCommitCallback([this](LLUICtrl*, const LLSD&) { run("journal"); });
    getChild<LLLineEditor>("request")->setCommitCallback([this](LLUICtrl*, const LLSD&) { run(getChild<LLCheckBoxCtrl>("citizen_mode")->get() ? "citizen_say" : "plan"); });
    for (const auto& binding : {std::pair<const char*, const char*>{"citizen_inspect", "inspect"}, {"citizen_pause", "pause"}, {"citizen_resume", "resume"}, {"citizen_cancel", "cancel"}, {"citizen_approve", "approve"}, {"citizen_undo", "undo"}})
    {
        const std::string operation = std::string("citizen_") + binding.second;
        getChild<LLButton>(binding.first)->setCommitCallback([this, operation](LLUICtrl*, const LLSD&) { run(operation); });
    }
    getChild<LLCheckBoxCtrl>("citizen_mode")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        cancel();
        const bool citizen = getChild<LLCheckBoxCtrl>("citizen_mode")->get();
        getChild<LLButton>("ask")->setLabel(citizen ? "Send" : "Plan");
        show(citizen ? "Talk to Lumi. Ask who she is, what she remembers, or say 'Lumi, go home.' Garden creation needs separate approval." : "Ask about the world or propose an approved world change.");
    });
    getChild<LLCheckBoxCtrl>("details")->setCommitCallback([this](LLUICtrl*, const LLSD&) { renderActivity(); });
    getChild<LLButton>("recipe_load")->setCommitCallback([this](LLUICtrl*, const LLSD&) { loadRecipe(); });
    getChild<LLButton>("recipe_save")->setCommitCallback([this](LLUICtrl*, const LLSD&) { saveRecipe(); });
    getChild<LLButton>("recipe_remix")->setCommitCallback([this](LLUICtrl*, const LLSD&) { LLFloaterReg::showInstance("finalverse_recipe", mRecipe); });
    show("Inspect nearby objects or describe a place. Review the plan before building. Every world change needs your approval.");
    busy(false);
    return true;
}
void LLFloaterFinalverseAI::openRecipe()
{
    openRequest("", false);
    auto self = dynamic_cast<LLFloaterFinalverseAI*>(LLFloaterReg::findInstance("finalverse_ai"));
    if (self && !self->mBusy) self->loadRecipe();
}
void LLFloaterFinalverseAI::openNearby()
{
    openRequest("", false);
    auto self = dynamic_cast<LLFloaterFinalverseAI*>(LLFloaterReg::findInstance("finalverse_ai"));
    if (self && !self->mBusy) self->run("inspect");
}
bool LLFloaterFinalverseAI::previewRecipe(const LLSD& recipe)
{
    auto self = dynamic_cast<LLFloaterFinalverseAI*>(LLFloaterReg::showInstance("finalverse_ai"));
    std::string error;
    if (!self || self->mBusy || !llfinalverseValidateRecipe(recipe, error)) return false;
    self->getChild<LLCheckBoxCtrl>("citizen_mode")->set(false);
    self->getChild<LLButton>("ask")->setLabel("Plan");
    self->mRecipe = recipe;
    self->run("recipe");
    return true;
}
void LLFloaterFinalverseAI::loadRecipe()
{
    if (mBusy) return;
    cancel();
    const auto handle = getHandle();
    const U32 generation = mGeneration;
    LLFilePickerReplyThread::startPicker([handle, generation](const std::vector<std::string>& names, LLFilePicker::ELoadFilter, LLFilePicker::ESaveFilter) {
        auto self = dynamic_cast<LLFloaterFinalverseAI*>(handle.get());
        if (!self || self->mBusy || self->mGeneration != generation || names.size() != 1) return;
        std::string error; LLSD recipe = llfinalverseReadRecipe(names.front(), error);
        if (!recipe.isMap()) { self->show(error); return; }
        previewRecipe(recipe);
    }, LLFilePicker::FFLOAD_ALL, false);
}
void LLFloaterFinalverseAI::saveRecipe()
{
    if (mBusy || !mRecipe.isMap()) return;
    const LLSD recipe = mRecipe; const auto handle = getHandle();
    LLFilePickerReplyThread::startPicker([handle, recipe](const std::vector<std::string>& names, LLFilePicker::ELoadFilter, LLFilePicker::ESaveFilter) {
        if (names.size() != 1) return;
        std::string error; const bool saved = llfinalverseSaveRecipe(recipe, names.front(), error);
        auto self = dynamic_cast<LLFloaterFinalverseAI*>(handle.get());
        // Saving authoring data must not replace a live approval preview.
        if (self) LLNotificationsUtil::add("GenericAlert", LLSD().with("MESSAGE", saved ? "Recipe saved. Load it to preview a new place, or share the file with a friend." : error));
    }, LLFilePicker::FFSAVE_ALL, "finalverse-recipe-" + LLUUID::generateNewID().asString() + ".json");
}
void LLFloaterFinalverseAI::openPrepared(const LLSD& prepared)
{
    llassert(LLCoros::on_main_coro());
    if (!prepared.isMap() || prepared["actor_id"].asString() != gAgent.getID().asString() || !LLUUID::validate(prepared["plan_id"].asString()) || !prepared["steps"].isArray()) return;
    auto self = dynamic_cast<LLFloaterFinalverseAI*>(LLFloaterReg::showInstance("finalverse_ai"));
    if (!self || self->mBusy) return;
    self->mPrepared = prepared; self->mCitizenPrepared = LLSD(); self->mRecipe = LLSD();
    self->getChild<LLCheckBoxCtrl>("citizen_mode")->set(false);
    self->show(llfinalversePlanSummary(prepared) + "\n\n" + prepared["siting"].asString() + "\nReview this preview, then Apply to build it as yours."); self->busy(false);
}
void LLFloaterFinalverseAI::saveRelease()
{
    if (mBusy || mLastPlan.empty() || !gAgent.getRegion()) return;
    const auto cap = gAgent.getRegion()->getCapability("FinalverseWorld"); if (cap.empty()) return;
    const auto actor = gAgent.getID(); const auto region = gAgent.getRegion()->getRegionID(); const auto handle = getHandle();
    LLSD body; body["op"] = "creator"; body["command"] = "save"; body["plan_id"] = mLastPlan;
    std::string title = getChild<LLLineEditor>("request")->getText();
    body["title"] = title.empty() ? "My creator place" : title.substr(0, 80);
    busy(true, true); show("Saving your place recipe as a private Creator Library release…");
    LLCoros::instance().launch("MutSeaSaveRelease", [handle, cap, actor, region, body]() {
        const auto reply = post(cap, body); auto self = dynamic_cast<LLFloaterFinalverseAI*>(handle.get()); if (!self) return;
        self->busy(false);
        if (!gAgent.getRegion() || gAgent.getRegion()->getRegionID() != region || gAgent.getID() != actor) { self->show("Region changed. Open World Library to check whether the release was saved."); return; }
        self->show(reply["ok"].asBoolean() ? "Private release saved. Open World Library to preview a remix or publish it as a free CC0 recipe. Your live scene remains yours." : reply["error"].asString());
    });
}
void LLFloaterFinalverseAI::openRequest(const std::string& text, bool citizen, bool submit)
{
    LLTimer timer;
    auto self = dynamic_cast<LLFloaterFinalverseAI*>(LLFloaterReg::showInstance("finalverse_ai"));
    if (!self || self->mBusy) return;
    self->mPrepared = LLSD(); self->mCitizenPrepared = LLSD();
    self->getChild<LLCheckBoxCtrl>("citizen_mode")->set(citizen);
    self->getChild<LLButton>("ask")->setLabel(citizen ? "Send" : "Plan");
    self->getChild<LLLineEditor>("request")->setText(text);
    self->show(citizen ? "Talk to Lumi. Ask about her home, memories, or what she is doing." : "Ask about nearby objects or describe a change. Review and approve before anything is built.");
    self->busy(false);
    self->getChild<LLLineEditor>("request")->setFocus(true);
    if (gSavedSettings.getBOOL("FinalverseUXMetrics")) LL_INFOS("FinalverseUX") << "ai_open_ms=" << timer.getElapsedTimeF64().value()*1000 << LL_ENDL;
    const auto services = llfinalverseWorldServices();
    if (citizen && !services.citizen_interactive) { self->show(llfinalverseCitizenUnavailable(services)); return; }
    if (submit) self->run(citizen ? "citizen_say" : "plan");
}
bool LLFloaterFinalverseAI::sendConversation(const std::string& text, bool citizen)
{
    auto self = LLFloaterReg::getTypedInstance<LLFloaterFinalverseAI>("finalverse_ai");
    if (!self || self->mBusy)
    { llfinalverseMessage("activity", "AI", "A request is still running. Wait for its result before sending another."); return false; }
    if (citizen && !llfinalverseWorldServices().citizen_interactive)
    { llfinalverseMessage("activity", "Lumi", llfinalverseCitizenUnavailable(llfinalverseWorldServices())); return false; }
    self->getChild<LLCheckBoxCtrl>("citizen_mode")->set(citizen);
    self->getChild<LLLineEditor>("request")->setText(text);
    self->run(citizen ? "citizen_say" : "plan");
    return self->mBusy;
}
bool LLFloaterFinalverseAI::conversationPending()
{
    auto self = dynamic_cast<LLFloaterFinalverseAI*>(LLFloaterReg::findInstance("finalverse_ai"));
    return self && self->mBusy;
}
void LLFloaterFinalverseAI::renderActivity()
{
    llassert(LLCoros::on_main_coro());
    getChild<LLTextEditor>("activity")->setText(mHumanText + (getChild<LLCheckBoxCtrl>("details")->get() && !mTechnicalText.empty() ? "\n\nTechnical details\n" + mTechnicalText : ""));
}
void LLFloaterFinalverseAI::show(const std::string& text, const std::string& details)
{
    // Network continuations run on fibers. Text layout may upload a missing
    // font glyph and acquire a non-coroutine-aware GL mutex, so draw owns it.
    mHumanText = text; mTechnicalText = details; mActivityDirty = true;
    llfinalverseMessage("activity", "World AI", text);
}
void LLFloaterFinalverseAI::draw()
{
    if (mActivityDirty)
    {
        mActivityDirty = false;
        renderActivity();
        auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
        auto node = LLSelectMgr::getInstance()->getSelection()->getFirstRootNode();
        const std::string name = object ? node && !node->mName.empty() ? node->mName : "Selected object" : "No object selected";
        getChild<LLTextBox>("context")->setText((gAgent.getRegion() ? gAgent.getRegion()->getName() : "Enter a world") + " · " + name);
    }
    if (mServicesRefresh.getElapsedTimeF32() >= .5f) { busy(mBusy, mMutation); mServicesRefresh.reset(); }
    LLFloater::draw();
}
void LLFloaterFinalverseAI::busy(bool active, bool mutation)
{
    mBusy = active; mMutation = active && mutation;
    if (!LLCoros::on_main_coro()) return; // draw refreshes controls on the main loop.
    const auto services = llfinalverseWorldServices();
    for (const char* name : {"ask", "inspect", "history", "citizen_inspect", "citizen_pause", "citizen_resume", "citizen_cancel", "citizen_undo"}) getChild<LLButton>(name)->setEnabled(!active);
    // Human overrides can interrupt reasoning without waiting for the model.
    for (const char* name : {"citizen_pause", "citizen_cancel"}) getChild<LLButton>(name)->setEnabled(!mMutation && services.citizen_interactive);
    const bool citizen = getChild<LLCheckBoxCtrl>("citizen_mode")->get();
    getChild<LLButton>("ask")->setEnabled(!active && (citizen ? services.citizen_interactive : services.canPlan()));
    for (const char* name : {"inspect", "history"}) getChild<LLButton>(name)->setEnabled(!active && services.world);
    for (const char* name : {"citizen_inspect", "citizen_resume", "citizen_undo"}) getChild<LLButton>(name)->setEnabled(!active && services.citizen_interactive);
    getChild<LLButton>("build")->setVisible(!citizen);
    getChild<LLButton>("build")->setEnabled(!active && services.canPlan() && mPrepared.has("plan_id"));
    getChild<LLButton>("citizen_approve")->setVisible(citizen);
    getChild<LLButton>("citizen_approve")->setEnabled(!active && services.citizen_interactive && services.canPlan() && mCitizenPrepared.has("plan_id"));
    for (const char* name : {"citizen_inspect", "citizen_pause", "citizen_resume", "citizen_cancel", "citizen_undo"}) getChild<LLButton>(name)->setVisible(citizen);
    for (const char* name : {"recipe_load", "recipe_save", "recipe_remix"}) getChild<LLButton>(name)->setVisible(!citizen);
    getChild<LLButton>("recipe_load")->setEnabled(!active && services.canCompose());
    getChild<LLButton>("release_save")->setVisible(!citizen);
    getChild<LLButton>("release_save")->setEnabled(!active && !mLastPlan.empty());
    getChild<LLButton>("recipe_save")->setEnabled(!active && mRecipe.isMap());
    getChild<LLButton>("recipe_remix")->setEnabled(!active && services.canCompose() && mRecipe.isMap());
    getChild<LLButton>("undo")->setLabel(citizen ? "Save recipe" : "Undo last");
    getChild<LLButton>("undo")->setEnabled(!active && (citizen ? mRecipe.isMap() : !mLastPlan.empty()));
    getChild<LLButton>("cancel")->setEnabled(!mMutation);
    getChild<LLLineEditor>("request")->setEnabled(!active && (!citizen || services.citizen_interactive));
    getChild<LLCheckBoxCtrl>("citizen_mode")->setEnabled(!active && services.citizen_interactive);
    getChild<LLCheckBoxCtrl>("citizen_mode")->setToolTip(services.citizen_interactive ? "Use your authorized citizen relay." : llfinalverseCitizenUnavailable(services));
}
void LLFloaterFinalverseAI::cancel()
{
    if (mMutation) return;
    if (mCitizenPrepared.has("plan_id")) { run("citizen_cancel"); return; }
    ++mGeneration; mPrepared = LLSD(); mCitizenPrepared = LLSD(); busy(false);
    show("Proposal canceled. No world changes were requested.");
}
void LLFloaterFinalverseAI::run(const std::string& operation)
{
    const bool override = operation == "citizen_pause" || operation == "citizen_cancel";
    if (mBusy && (!override || mMutation)) return;
    if (operation == "commit")
    {
        if (!mPrepared.has("plan_id")) return;
        auto selected = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
        if ((selected ? selected->getID() : LLUUID::null) != mPlanSelection)
        { cancel(); show("Your selection changed. Plan again for the current object."); return; }
    }
    if (operation == "citizen_approve" && !mCitizenPrepared.has("plan_id")) return;
    // A proposal is actionable only while its own preview is displayed.
    if (operation != "commit") mPrepared = LLSD();
    if (operation != "citizen_approve") mCitizenPrepared = LLSD();
    busy(false);
    auto region = gAgent.getRegion();
    if (!region) { show("Enter a world first."); return; }
    const std::string capability = region->getCapability("FinalverseWorld");
    if (capability.empty()) { show("This region has not enabled Finalverse world actions."); return; }
    const LLUUID region_id = region->getRegionID();
    const std::string prompt = getChild<LLLineEditor>("request")->getText();
    if (operation == "plan" && (prompt.empty() || prompt.size() > 2048)) { show("Enter a request of up to 2048 characters."); return; }
    const bool citizen = operation.compare(0, 8, "citizen_") == 0;
    const auto services = llfinalverseWorldServices();
    if (citizen && !services.citizen_interactive) { show(llfinalverseCitizenUnavailable(services)); return; }
    if (operation == "plan" && services.checking) { show("Checking the world's AI service… Please try again shortly."); return; }
    if (operation == "recipe" && !services.canCompose()) { show("Composed places are unavailable here. You can still inspect the world and use creator tools."); return; }
    if (services.recovery && (operation == "plan" || operation == "commit" || operation == "undo" || operation == "citizen_approve" || operation == "citizen_undo"))
    { show("World changes are paused for recovery. Use Nearby or History to inspect what happened."); return; }
    if (citizen && operation == "citizen_say" && (prompt.empty() || prompt.size() > 2048)) { show("Enter a message for Lumi of up to 2048 characters."); return; }
    if (operation == "plan" || operation == "citizen_say")
        llfinalverseMessage("conversation", citizen ? "You → Lumi" : "You → World AI", prompt);
    LLSD body; body["op"] = citizen ? "citizen" : (operation == "plan" || operation == "recipe") ? "inspect" : operation;
    if (citizen) { body["command"] = operation.substr(8); body["text"] = prompt; }
    if (auto object = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject()) body["selected_id"] = object->getID().asString();
    if (operation == "commit") body["plan_id"] = mPrepared["plan_id"];
    if (operation == "undo") body["plan_id"] = mLastPlan;
    const U32 generation = ++mGeneration;
    const LLHandle<LLFloater> handle = getHandle();
    busy(true, operation == "commit" || operation == "undo" || operation == "citizen_approve" || operation == "citizen_undo");
    const LLSD recipe = mRecipe;
    const LLUUID actor_id = gAgentID, login_session = gAgentSessionID;
    const bool server_planning = services.server_planning;
    if (server_planning && (operation == "plan" || operation == "recipe"))
    {
        body["op"] = "plan";
        if (operation == "recipe") { body.erase("selected_id"); body["world_spec"] = recipe; }
        else body["request"] = prompt;
    }
    show(operation == "recipe" ? "Replaying your recipe and checking placement…" : operation == "plan" ? "Looking around and planning…" : operation == "citizen_say" ? "Lumi is considering your message… You can pause or cancel while she thinks." : "Waiting for the world service…");
    LLCoros::instance().launch("FinalverseWorld", [handle, generation, region_id, capability, operation, body, prompt, recipe, server_planning, actor_id, login_session]()
    {
        auto current = [&]() -> LLFloaterFinalverseAI*
        {
            auto self = dynamic_cast<LLFloaterFinalverseAI*>(handle.get());
            if (!self || self->mGeneration != generation) return nullptr;
            if (gAgentID != actor_id || gAgentSessionID != login_session || !gAgent.getRegion() ||
                gAgent.getRegion()->getRegionID() != region_id || gAgent.getRegion()->getCapability("FinalverseWorld") != capability)
            { self->mPrepared = LLSD(); self->mCitizenPrepared = LLSD(); self->mLastPlan.clear(); self->busy(false); self->show("Your world session changed. Please inspect and plan again."); return nullptr; }
            return self;
        };
        // Recipe replay has no inference context. Going directly to prepare
        // avoids a redundant inspection racing the inherited 100 ms throttle.
        LLSD reply = operation == "recipe" && !server_planning ? LLSD() : post(capability, body);
        auto self = current(); if (!self) return;
        if (reply.has("error")) { self->mPrepared = LLSD(); self->mCitizenPrepared = LLSD(); self->busy(false); self->show(reply["error"].asString()); llfinalverseMessage("conversation", "System", "Request failed: " + reply["error"].asString()); return; }
        if (operation.compare(0, 8, "citizen_") == 0)
        {
            const LLSD result = reply["result"];
            llfinalverseMessage("conversation", "Lumi", result["answer"].asString());
            std::string recipe_error;
            if (llfinalverseValidateRecipe(result["world_spec"], recipe_error)) self->mRecipe = result["world_spec"];
            self->mCitizenPrepared = LLSD();
            std::ostringstream display, technical;
            display << "Lumi: " << result["answer"].asString() << "\n\n";
            if (operation == "citizen_inspect")
            {
                const LLSD identity = result["citizen"]["identity"];
                display << "Activity: " << identity["lifecycle"].asString() << "\n";
                technical << "Home: " << identity["home_entity_id"].asString() << "\n";
                technical << "Capabilities: ";
                for (const auto& capability : llsd::inArray(identity["capabilities"])) technical << capability.asString() << " ";
                display << "\n";
                const LLSD goals = result["citizen"]["goals"];
                if (goals.size())
                {
                    const LLSD goal = goals[goals.size() - 1];
                    display << "Goal: " << goal["description"].asString() << " · " << goal["status"].asString() << "\n";
                    display << "Intention: " << goal["intention"].asString() << "\n";
                    if (!goal["reason"].asString().empty()) display << "Reason: " << goal["reason"].asString() << "\n";
                }
            }
            if (result["preview"].isMap())
            {
                self->mCitizenPrepared = result["preview"];
                self->getChild<LLButton>("citizen_approve")->setLabel(result["world_spec"].isMap() ? "Build together" : "Create garden");
                display << "\n" << llfinalversePlanSummary(result["preview"]) << "\n";
                for (const auto& step : llsd::inArray(result["preview"]["steps"]))
                { technical << "• " << step["after"]["name"].asString() << "\n"; describe(technical, step["after"]); }
            }
            self->getChild<LLButton>("citizen_undo")->setLabel("Undo creation");
            if (operation == "citizen_inspect")
            {
                display << "\nPersistent memories\n";
                for (const auto& memory : llsd::inArray(result["memories"])) display << "• " << memory["content"].asString() << "\n";
                technical << "\nRecent agent events\n";
                for (const auto& entry : llsd::inArray(result["recent_events"])) technical << "• " << entry["kind"].asString() << "\n";
            }
            self->show(display.str(), technical.str()); self->busy(false); return;
        }
        if (operation == "inspect" || operation == "plan") self->mLastPlan = reply["result"]["undoable_plan_id"].asString();
        if (operation == "plan" || operation == "recipe")
        {
            LLSD request;
            if (operation == "recipe") request["world_spec"] = recipe;
            else { request["request"] = prompt; request["world_context"] = modelContext(reply["result"]); }
            LLSD generated = server_planning ? reply["result"] : post(operation == "recipe" ? "http://127.0.0.1:18765/v1/world/spec/plan" : "http://127.0.0.1:18765/v1/world/plan", request);
            self = current(); if (!self) return;
            if (generated.has("error")) { self->busy(false); self->show(generated["error"].asString()); llfinalverseMessage("conversation", "System", "Request failed: " + generated["error"].asString()); return; }
            llfinalverseMessage("conversation", "World AI", generated["answer"].asString());
            std::string recipe_error;
            self->mRecipe = llfinalverseValidateRecipe(generated["world_spec"], recipe_error) ? generated["world_spec"] : LLSD();
            self->mPrepared = LLSD();
            if (!generated["plan"].isMap()) { self->busy(false); self->show(generated["answer"].asString()); return; }
            self->show("Checking placement and permissions…");
            LLSD prepare; prepare["op"] = "prepare"; prepare["plan"] = generated["plan"];
            LLSD validated = post(capability, prepare);
            self = current(); if (!self) return;
            if (validated.has("error")) { self->busy(false); self->show(validated["error"].asString()); return; }
            self->mPrepared = validated["result"];
            self->mPlanSelection = LLUUID(body["selected_id"].asString());
            std::ostringstream preview;
            preview << generated["answer"].asString() << "\n\n" << self->mPrepared["summary"].asString() << "\n"
                    << "Region: " << region_id.asString() << "\n\n";
            for (const auto& step : llsd::inArray(self->mPrepared["steps"]))
            {
                const LLSD value = step["after"].isMap() ? step["after"] : step["before"];
                preview << "• " << step["type"].asString() << " " << value["name"].asString() << "\n";
                if (step["before"].isMap() && step["after"].isMap())
                {
                    preview << "  Before: " << step["before"]["name"].asString() << "\n";
                    describe(preview, step["before"]);
                    preview << "  After: " << value["name"].asString() << "\n";
                }
                describe(preview, value);
            }
            preview << "\nModel: " << generated["plan"]["provider"].asString() << " / " << generated["plan"]["model"].asString() << "\nApprove within two minutes. Build applies this exact proposal.";
            // The validated steps determine the approval copy. The model's
            // answer can describe a different stage or imply an unmade change.
            preview << "\nPlanner answer: " << generated["answer"].asString();
            self->show(llfinalversePlanSummary(self->mPrepared), preview.str()); self->busy(false); return;
        }
        std::ostringstream text, technical;
        if (operation == "inspect")
        {
            text << "Nearby world objects\n\n";
            for (const auto& item : llsd::inArray(reply["result"]["nearby_entities"]))
                text << "• " << item["entity"]["name"].asString() << " (" << (S32)item["distance"].asReal() << "m)\n";
            text << "\nInspection is limited to 64 meters and 64 objects.";
        }
        else if (operation == "journal")
        {
            text << "WorldLine · your recent operations\n\n";
            for (const auto& entry : llsd::inArray(reply["result"]))
                text << entry["status"].asString() << " · " << entry["proposal"]["request"].asString() << "\n";
        }
        else
        {
            const std::string status = reply["result"]["status"].asString();
            text << (status == "committed" ? "Done. Your world has changed. Use Undo last to restore it." : status == "undone" ? "Undone. The previous world state was restored." : "World operation: " + status) << "\n";
            if (status == "committed") self->mLastPlan = reply["result"]["plan_id"].asString();
            if (status == "undone") self->mLastPlan.clear();
            technical << "Operation: " << reply["result"]["operation_id"].asString() << "\n";
            if (reply["result"]["error"].isString()) text << reply["result"]["error"].asString();
            self->mPrepared = LLSD();
        }
        self->show(text.str(), technical.str()); self->busy(false);
    });
}
