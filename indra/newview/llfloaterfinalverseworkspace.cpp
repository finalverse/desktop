// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalverseworkspace.h"
#include "llfinalversepresentation.h"
#include "llpanelfinalverse.h"
#include "llfloaterfinalverseai.h"
#include "llfloaterfinalverseexperience.h"
#include "llfloaterimnearbychat.h"
#include "llagent.h"
#include "llbutton.h"
#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llfloaterreg.h"
#include "lllineeditor.h"
#include "lltexteditor.h"
#include "llscrollcontainer.h"
#include "lltextbox.h"
#include "llviewercontrol.h"
#include "llviewerregion.h"
#include "llcoros.h"
#include "llappviewer.h"
#include "llcorehttputil.h"
#include "llweb.h"
#include <sstream>

namespace { LLFinalverseMessageLog messages; }
void llfinalverseMessage(const std::string& channel, const std::string& speaker, const std::string& text)
{
    // Data only. Coroutine callbacks never lay out text or acquire GL locks.
    messages.append(gAgentSessionID.isNull() ? "" : gAgentSessionID.asString(), channel,
                    speaker, text, LLDate::now().asString().substr(11, 8));
}
U64 llfinalverseMessageRevision()
{
    messages.session(gAgentSessionID.isNull() ? "" : gAgentSessionID.asString());
    return messages.revision();
}
bool LLFloaterFinalverseWorkspace::postBuild()
{
    for (const char* mode : {"world", "first_person", "overview", "create", "select"})
    {
        const std::string action(mode);
        getChild<LLButton>(action)->setCommitCallback([action](LLUICtrl*, const LLSD&) {
            if (auto hud = LLPanelFinalverse::instance()) hud->action(action);
        });
    }
    for (const char* page : {"welcome", "tools", "me"})
    {
        const std::string name(page);
        getChild<LLButton>(name)->setCommitCallback([name](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::openPage(name); });
    }
    for (const char* command : {"inventory", "friends"})
    {
        const std::string name(command);
        getChild<LLButton>(name)->setCommitCallback([name](LLUICtrl*, const LLSD&) { LLFloaterFinalverseExperience::executeCommand(name); });
    }
    return true;
}
void LLFloaterFinalverseWorkspace::draw()
{
    if (mRefresh.getElapsedTimeF32() > .5f)
    {
        getChild<LLButton>("select")->setLabel(LLPanelFinalverse::instance() && LLPanelFinalverse::instance()->selecting() ? "Cancel" : "Select");
        mRefresh.reset();
    }
    LLFloater::draw();
}
bool LLFloaterFinalverseConversation::postBuild()
{
    for (const char* name : {"conversation", "activity", "artifacts"})
    {
        const std::string tab(name);
        getChild<LLButton>(tab)->setCommitCallback([this,tab](LLUICtrl*, const LLSD&) { mActivity = tab == "activity"; mArtifactView = tab == "artifacts"; mRevision = 0; });
    }
    getChild<LLButton>("send")->setCommitCallback([this](LLUICtrl*, const LLSD&) { send(); });
    getChild<LLLineEditor>("message")->setCommitCallback([this](LLUICtrl*, const LLSD&) { send(); });
    getChild<LLButton>("clear")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        messages.clear(); mShared["messages"] = LLSD::emptyArray(); mShared["activity"] = LLSD::emptyArray(); mSharedDirty = true;
    });
    getChild<LLButton>("review")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        if (mArtifactView) remote("artifact", getChild<LLComboBox>("artifact_list")->getValue().asString());
        else LLFloaterReg::showInstance("finalverse_ai");
    });
    getChild<LLButton>("size")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        LLRect rect = getRect();
        rect.mBottom = rect.mTop - (rect.getHeight() > 360 ? 320 : 420);
        setShape(rect);
    });
    getChild<LLComboBox>("target")->setCommitCallback([this](LLUICtrl*,const LLSD&){mRevision=0;});
    getChild<LLComboBox>("artifact_list")->setCommitCallback([this](LLUICtrl*,const LLSD&){mArtifactPreview=LLSD();mRevision=0;});
    getChild<LLComboBox>("target")->selectFirstItem();
    return true;
}
void LLFloaterFinalverseConversation::onOpen(const LLSD& key)
{
    if (key.isString()) {mActivity = key.asString() == "activity";mArtifactView=false;}
    mRevision = 0;
}
void LLFloaterFinalverseConversation::send()
{
    auto input = getChild<LLLineEditor>("message");
    std::string text = input->getText(); LLStringUtil::trim(text);
    if (text.empty() || text.size() > 2048) return;
    auto hud = LLPanelFinalverse::instance();
    if (text.compare(0, 9, "/connect ") == 0)
    { if (remote("join", text.substr(9))) input->setText(std::string()); return; }
    if (text == "/disconnect") { if (remote("leave")) input->setText(std::string()); return; }
    if (text == "/earth") { LLWeb::loadURLExternal("http://127.0.0.1:18651/"); input->setText(std::string()); return; }
    if (text[0] == '/')
    {
        if (hud) hud->command(text);
        input->setText(std::string()); return;
    }
    const auto target = getChild<LLComboBox>("target")->getValue().asString();
    if (target == "session")
    { if (remote("message", text)) input->setText(std::string()); }
    else if (target == "nearby")
    {
        LLFloaterIMNearbyChat::sendChatFromViewer(text, CHAT_TYPE_NORMAL, false);
        input->setText(std::string());
    }
    else if (LLFloaterFinalverseAI::sendConversation(text, target == "lumi")) input->setText(std::string());
    mActivity = false; mArtifactView = false; mRevision = 0;
}
void LLFloaterFinalverseConversation::draw()
{
    llassert(LLCoros::on_main_coro());
    static LLTimer text_refresh;
    if (mComposerSession != gAgentSessionID || (mLinked && (mLinkedActor != gAgentID || mLinkedSession != gAgentSessionID || !gAgent.getRegion() || mLinkedRegion != gAgent.getRegion()->getRegionID())))
    {
        ++mNetworkGeneration; mComposerSession = gAgentSessionID; mLinked = mNetworkBusy = mRecovering = false; mReadFailures = 0;
        mShared = LLSD(); mArtifactPreview = LLSD(); mQueuedAction.clear(); mQueuedValue.clear(); mRemoteAction.clear(); mSharedDirty = true;
        getChild<LLLineEditor>("message")->setText(std::string());
    }
    if (isMinimized()) { LLFloater::draw(); return; }
    if (mLinked && !mNetworkBusy && mReplay.getElapsedTimeF32() > F32(1 << llmin(mReadFailures,3))) { mReplay.reset(); remote("read"); }
    const bool shared = getChild<LLComboBox>("target")->getValue().asString() == "session";
    getChild<LLComboBox>("artifact_list")->setVisible(mArtifactView);
    getChild<LLComboBox>("target")->setVisible(!mArtifactView);
    getChild<LLButton>("review")->setLabel(mArtifactView ? "Inspect source" : "Review / Create");
    const U64 revision = llfinalverseMessageRevision();
    if ((mRevision != revision || mSharedDirty) && (mRevision == 0 || text_refresh.getElapsedTimeF32() >= .1f))
    {
        auto history = getChild<LLTextEditor>("messages");
        const bool follow = history->scrolledToEnd() || mRevision == 0;
        const LLRect visible = history->getScrollContainer()->getVisibleContentRect();
        std::ostringstream text;
        if (mArtifactView)
        {
            if (mArtifactPreview.isMap())
            {
                text << mArtifactPreview["title"].asString() << "\n\n";
                for (const auto& facility : llsd::inArray(mArtifactPreview["facilities"])) text << "• " << facility["properties"]["name"].asString() << " · " << facility["properties"]["fuel"].asString() << "\n";
                text << "\nArchival source · up to six facilities shown\n" << mArtifactPreview["provenance"]["source"].asString() << "\nLicense: " << mArtifactPreview["provenance"]["license"].asString();
                text << "\n\nSource receipt: " << mArtifactPreview["receipt"]["execution_id"].asString() << "\nNative exhibition geometry is schematic.";
            }
            else
            {
                text << "Saved spatial artifacts\n\n";
                for (const auto& artifact : llsd::inArray(mShared["artifacts"]))
                    text << artifact["title"].asString() << "\nVersion " << artifact["version"].asInteger() << " · original source snapshot\n\n";
                if (mLinked) text << "Choose a workspace below and inspect its sources. Then return to Conversation to ask about it.\nUse /earth for the full source view.";
            }
            if (!mLinked) text << "Connect an explicitly shared Earth session first.\n/connect <scoped handoff>\nNo token is saved. Use /earth to open the local source view.";
        }
        else if (shared && !mActivity)
        {
            for (const auto& entry : llsd::inArray(mShared["messages"])) text << entry["speaker"].asString() << "\n" << entry["text"].asString() << "\n\n";
            if (!mLinked) text << "Use /connect <scoped handoff> to resume an authorized Earth session.\nThe owner grants access with earth confluence invite.";
        }
        else
        {
            for (const auto& entry : messages.entries())
                if ((entry["channel"].asString() == "activity") == mActivity)
                    text << entry["time"].asString() << "  " << entry["speaker"].asString() << "\n" << entry["text"].asString() << "\n\n";
            if (mActivity && mLinked) {
                text << "Shared session · " << mShared["session_id"].asString() << "\n";
                for (const auto& entry : llsd::inArray(mShared["activity"])) text << "#" << entry["sequence"].asInteger() << " · " << entry["type"].asString() << "\n";
            }
        }
        history->setText(text.str().empty() ? (mActivity ? "Live region, world-action and system messages appear here." : "Talk to World AI, Lumi, or people nearby. Choose a recipient below. Messages stay in this login session.") : text.str());
        if (follow) history->setCursorAndScrollToEnd();
        else history->getScrollContainer()->scrollToShowRect(visible);
        getChild<LLButton>("conversation")->setToggleState(!mActivity && !mArtifactView);
        getChild<LLButton>("artifacts")->setToggleState(mArtifactView);
        getChild<LLButton>("activity")->setToggleState(mActivity);
        mRevision = revision; mSharedDirty = false;
        text_refresh.reset();
    }
    if (mRefresh.getElapsedTimeF32() > .5f)
    {
        const auto service = llfinalverseWorldServices();
        const bool pending = shared ? (mNetworkBusy && mRemoteAction != "read") || !mQueuedAction.empty() : LLFloaterFinalverseAI::conversationPending();
        const bool thinking = shared ? mShared["status"].asString() == "running" : pending;
        getChild<LLTextBox>("context")->setText((gAgent.getRegion() ? gAgent.getRegion()->getName() : "Connecting…") + std::string(shared && mRecovering ? " · Reconnecting…" : thinking ? " · AI working…" : shared && mLinked ? " · Shared session" : service.recovery ? " · recovery" : ""));
        getChild<LLButton>("send")->setEnabled(!mArtifactView && (!shared || !mRecovering) && (!pending || getChild<LLComboBox>("target")->getValue().asString() == "nearby"));
        mRefresh.reset();
    }
    LLFloater::draw();
}

bool LLFloaterFinalverseConversation::remote(const std::string& action, const std::string& value)
{
    auto region = gAgent.getRegion();
    if (mNetworkBusy)
    {
        // Queue one new user intent behind a read; never retry a sent POST.
        if (action != "read" && mQueuedAction.empty()) {mQueuedAction=action;mQueuedValue=value;return true;}
        return false;
    }
    if (!region) return false;
    if (mRecovering && action != "read" && action != "leave" && action != "join") return false;
    const auto cap = region->getCapability("FinalverseWorld");
    if (cap.empty()) { llfinalverseMessage("activity", "Confluence", "Session continuity is unavailable here."); return false; }
    LLSD body; body["op"] = "confluence"; body["action"] = action;
    if (action == "join")
    {
        if (value.size() != 64 || !std::all_of(value.begin(),value.end(),[](unsigned char c){return std::isxdigit(c);}))
        { llfinalverseMessage("activity", "Confluence", "Paste a valid scoped 64-character handoff after /connect."); return false; }
        body["handoff"] = value; mShared = LLSD(); mArtifactPreview = LLSD(); mLinked = false;
    }
    else if (!mLinked) { llfinalverseMessage("activity", "Confluence", "Connect an explicitly shared session first."); return false; }
    if (action == "read") body["after"] = mShared["cursor"].asInteger();
    if (action == "artifact") body["artifact_id"] = value;
    if (action == "message") {
        body["text"] = value; body["request_id"] = LLUUID::generateNewID().asString();
        const auto artifact = getChild<LLComboBox>("artifact_list")->getValue().asString();
        if (LLUUID::validate(artifact)) body["artifact_id"] = artifact;
    }
    const LLUUID actor = gAgentID, login = gAgentSessionID, region_id = region->getRegionID();
    const U64 generation = ++mNetworkGeneration; auto handle = getHandle(); mNetworkBusy = true; mRemoteAction = action; mReplay.reset();
    LLCoros::instance().launch("ConfluenceSession",[handle,body,cap,actor,login,region_id,generation,action] {
        LLCoreHttpUtil::HttpCoroutineAdapter adapter("ConfluenceSession",LLCore::HttpRequest::DEFAULT_POLICY_ID);
        auto request = std::make_shared<LLCore::HttpRequest>(); auto options = std::make_shared<LLCore::HttpOptions>();
        options->setTimeout(12); options->setRetries(0);
        auto reply = adapter.postJsonAndSuspend(request,cap,body,options);
        LLAppViewer::instance()->postToMainCoro([handle,reply,cap,actor,login,region_id,generation,action] {
            auto self = dynamic_cast<LLFloaterFinalverseConversation*>(handle.get());
            if (!self || self->mNetworkGeneration != generation) return;
            self->mNetworkBusy = false;
            if (actor != gAgentID || login != gAgentSessionID || !gAgent.getRegion() || region_id != gAgent.getRegion()->getRegionID() || cap != gAgent.getRegion()->getCapability("FinalverseWorld"))
            { self->mLinked = self->mRecovering = false; self->mReadFailures = 0; self->mShared = LLSD(); self->mArtifactPreview = LLSD(); self->mQueuedAction.clear(); self->mQueuedValue.clear(); self->mSharedDirty = true; return; }
            bool display_changed = action != "read";
            const LLSD result = reply["result"];
            if (!reply["ok"].asBoolean() || !result.isMap())
            {
                const auto session_id = self->mShared["session_id"];
                self->mLinked = action == "read" && ++self->mReadFailures <= 3;
                self->mRecovering = self->mLinked;
                self->mShared = LLSD(); self->mShared["session_id"] = session_id; self->mArtifactPreview = LLSD(); self->mQueuedAction.clear(); self->mQueuedValue.clear(); display_changed = true; self->mActivity = true; self->mArtifactView = false;
                llfinalverseMessage("activity", "Confluence", self->mRecovering ? "Source connection interrupted. Display cleared while read access is revalidated; no message is resent." : "Connection refused, expired or interrupted. Review the source session and issue a new grant; no message is retried automatically.");
            }
            else if (action == "join")
            {
                self->mReadFailures = 0; self->mRecovering = false;
                self->mLinkedActor = actor; self->mLinkedSession = login; self->mLinkedRegion = region_id;
                self->mShared["session_id"] = result["session_id"]; self->mLinked = true;
                self->getChild<LLComboBox>("target")->setValue("session");
                llfinalverseMessage("activity", "Confluence", "Connected to the existing shared Earth session. World editing still requires separate approval.");
            }
            else if (action == "read")
            {
                std::string error; LLSD updated = llfinalverseSessionPage(self->mShared,result,error);
                if (!error.empty()) { self->mLinked = false; self->mShared = LLSD(); self->mArtifactPreview = LLSD(); llfinalverseMessage("activity", "Confluence",error); }
                else
                {
                    const bool changed = self->mShared["artifacts"].size() != updated["artifacts"].size() || (updated["artifacts"].size() > 0 && self->mShared["artifacts"][0]["artifact_id"].asString() != updated["artifacts"][0]["artifact_id"].asString());
                    display_changed = changed || self->mShared["cursor"].asInteger() != updated["cursor"].asInteger() || self->mShared["status"].asString() != updated["status"].asString();
                    self->mReadFailures = 0; self->mRecovering = false;
                    self->mShared = updated;
                    if (changed) {
                    auto list = self->getChild<LLComboBox>("artifact_list"); auto selection = list->getValue(); list->clearRows();
                    for (const auto& artifact : llsd::inArray(updated["artifacts"])) list->add(artifact["title"].asString().empty() ? artifact["kind"].asString() : artifact["title"].asString(),artifact["artifact_id"]);
                    if (!list->setSelectedByValue(selection, true)) list->selectFirstItem();
                    }
                }
            }
            else if (action == "artifact") self->mArtifactPreview = result;
            else if (action == "leave") { self->mLinked = false; self->mShared = LLSD(); self->mArtifactPreview = LLSD(); }
            if (display_changed) self->mSharedDirty = true;
            if (!self->mQueuedAction.empty()) {
                auto action = std::exchange(self->mQueuedAction, std::string());
                auto value = std::exchange(self->mQueuedValue, std::string());
                self->remote(action,value);
            }
        });
    });
    return true;
}
