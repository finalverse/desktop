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
    for (const char* name : {"conversation", "activity"})
    {
        const std::string tab(name);
        getChild<LLButton>(tab)->setCommitCallback([this,tab](LLUICtrl*, const LLSD&) { mActivity = tab == "activity"; mRevision = 0; });
    }
    getChild<LLButton>("send")->setCommitCallback([this](LLUICtrl*, const LLSD&) { send(); });
    getChild<LLLineEditor>("message")->setCommitCallback([this](LLUICtrl*, const LLSD&) { send(); });
    getChild<LLButton>("clear")->setCommitCallback([](LLUICtrl*, const LLSD&) { messages.clear(); });
    getChild<LLButton>("review")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterReg::showInstance("finalverse_ai"); });
    getChild<LLButton>("size")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        LLRect rect = getRect();
        rect.mBottom = rect.mTop - (rect.getHeight() > 360 ? 320 : 420);
        setShape(rect);
    });
    getChild<LLComboBox>("target")->selectFirstItem();
    return true;
}
void LLFloaterFinalverseConversation::onOpen(const LLSD& key)
{
    if (key.isString()) mActivity = key.asString() == "activity";
    mRevision = 0;
}
void LLFloaterFinalverseConversation::send()
{
    auto input = getChild<LLLineEditor>("message");
    std::string text = input->getText(); LLStringUtil::trim(text);
    if (text.empty() || text.size() > 2048) return;
    auto hud = LLPanelFinalverse::instance();
    if (text[0] == '/')
    {
        if (hud) hud->command(text);
        input->setText(std::string()); return;
    }
    const auto target = getChild<LLComboBox>("target")->getValue().asString();
    if (target == "nearby")
    {
        LLFloaterIMNearbyChat::sendChatFromViewer(text, CHAT_TYPE_NORMAL, false);
        input->setText(std::string());
    }
    else if (LLFloaterFinalverseAI::sendConversation(text, target == "lumi")) input->setText(std::string());
    mActivity = false; mRevision = 0;
}
void LLFloaterFinalverseConversation::draw()
{
    llassert(LLCoros::on_main_coro());
    static LLTimer text_refresh;
    const U64 revision = llfinalverseMessageRevision();
    if (mRevision != revision && (mRevision == 0 || text_refresh.getElapsedTimeF32() >= .1f))
    {
        auto history = getChild<LLTextEditor>("messages");
        const bool follow = history->scrolledToEnd() || mRevision == 0;
        const LLRect visible = history->getScrollContainer()->getVisibleContentRect();
        std::ostringstream text;
        for (const auto& entry : messages.entries())
            if ((entry["channel"].asString() == "activity") == mActivity)
                text << entry["time"].asString() << "  " << entry["speaker"].asString() << "\n" << entry["text"].asString() << "\n\n";
        history->setText(text.str().empty() ? (mActivity ? "Live region, world-action and system messages appear here." : "Talk to World AI, Lumi, or people nearby. Choose a recipient below. Messages stay in this login session.") : text.str());
        if (follow) history->setCursorAndScrollToEnd();
        else history->getScrollContainer()->scrollToShowRect(visible);
        getChild<LLButton>("conversation")->setToggleState(!mActivity);
        getChild<LLButton>("activity")->setToggleState(mActivity);
        mRevision = revision;
        text_refresh.reset();
    }
    if (mRefresh.getElapsedTimeF32() > .5f)
    {
        const auto service = llfinalverseWorldServices();
        const bool pending = LLFloaterFinalverseAI::conversationPending();
        getChild<LLTextBox>("context")->setText((gAgent.getRegion() ? gAgent.getRegion()->getName() : "Connecting…") + std::string(pending ? " · AI working…" : service.recovery ? " · recovery" : ""));
        getChild<LLButton>("send")->setEnabled(!pending || getChild<LLComboBox>("target")->getValue().asString() == "nearby");
        mRefresh.reset();
    }
    LLFloater::draw();
}
