/**
 * @file llfloaterdestinations.h
 * @author Leyla Farazha
 * @brief floater for the destinations guide
 *
 * $LicenseInfo:firstyear=2011&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2011, Linden Research, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llfloaterdestinations.h"
#include "llmediactrl.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llweb.h"
#include "llviewernetwork.h"
#include "llfloaterfinalverseexperience.h"
#include "llfloaterfinalverseai.h"
#include "llbutton.h"
#include "llagent.h"
#include "llviewerregion.h"
#include "llscrolllistctrl.h"
#include "lltextbox.h"
#include "llcoros.h"
#include "llcorehttputil.h"
#include "llfinalversepresentation.h"
#include <set>


LLFloaterDestinations::LLFloaterDestinations(const LLSD& key)
    :   LLFloater(key)
{
}

LLFloaterDestinations::~LLFloaterDestinations()
{
    if (auto media = findChild<LLMediaCtrl>("destination_guide_contents")) media->remObserver(this);
}

bool LLFloaterDestinations::postBuild()
{
    LLMediaCtrl* destinations = getChild<LLMediaCtrl>("destination_guide_contents");
    destinations->addObserver(this);
    getChild<LLButton>("explore_current_world")->setCommitCallback([](LLUICtrl*, const LLSD&)
        { LLFloaterFinalverseExperience::openPage("explore"); });
    getChild<LLButton>("library_refresh")->setCommitCallback([this](LLUICtrl*, const LLSD&) { mLibraryAttempts = 0; refreshLibrary(); });
    getChild<LLButton>("library_teleport")->setCommitCallback([this](LLUICtrl*, const LLSD&) {
        auto selected = getChild<LLScrollListCtrl>("world_regions")->getFirstSelected();
        if (!selected || mLibraryBusy || !gAgent.getRegion() || gAgent.getRegion()->getRegionID() != mLibraryRegion) return;
        for (const auto& d : llsd::inArray(mDestinations)) if (d["region_id"].asString() == selected->getValue().asString() && d["online"].asBoolean())
        {
            LLVector3d target(d["global_origin"][0].asReal() + d["spawn"][0].asReal(), d["global_origin"][1].asReal() + d["spawn"][1].asReal(), d["spawn"][2].asReal());
            gAgent.teleportViaLocation(target); break;
        }
    });
    for (const char* command : {"publish", "unpublish", "remix"})
        getChild<LLButton>(std::string("release_") + command)->setCommitCallback([this, command](LLUICtrl*, const LLSD&) { manageRelease(command); });
    getChild<LLButton>("confluence_ai")->setCommitCallback([](LLUICtrl*, const LLSD&) {
        LLFloaterFinalverseAI::openRequest("Describe this workspace and help me plan something we can create together.", false);
    });
    return true;
}

void LLFloaterDestinations::showFallback()
{
    mLoading = false;
    auto media = getChild<LLMediaCtrl>("destination_guide_contents");
    media->setVisible(false);
    if (media->getMediaPlugin()) media->getMediaPlugin()->browse_stop();
    getChild<LLPanel>("destination_fallback")->setVisible(true);
    LL_INFOS("FinalverseDiscovery") << "bundled_cards grid=" << mGrid << LL_ENDL;
}

void LLFloaterDestinations::onOpen(const LLSD& key)
{
    mLibraryAttempts = 0;
    refreshLibrary();
    auto grid = LLGridManager::getInstance();
    mGrid = grid->getGrid();
    mSystemGrid = grid->isSystemGrid();
    auto media = getChild<LLMediaCtrl>("destination_guide_contents");
    media->setTrustedContent(mSystemGrid);
    media->setErrorPageURL(mSystemGrid ? gSavedSettings.getString("GenericErrorPageURL") : "");
    if (!mSystemGrid && gAgent.getRegion() && !gAgent.getRegion()->getCapability("FinalverseWorld").empty()) { showFallback(); return; }
    const std::string url = grid->getDestinationGuideURL();
    if (url.empty()) { showFallback(); return; }
    // Keep usable native content visible until a configured remote page loads.
    getChild<LLPanel>("destination_fallback")->setVisible(!mSystemGrid);
    media->setVisible(true);
    mLoading = true;
    mLoadTimer.reset();
    media->navigateTo(url, HTTP_CONTENT_TEXT_HTML);
    if (mSystemGrid) LLViewerMedia::getInstance()->getOpenIDCookie(media);
    LL_INFOS("FinalverseDiscovery") << "configured_page grid=" << mGrid << " system=" << mSystemGrid << LL_ENDL;
}

void LLFloaterDestinations::draw()
{
    if (mGrid != LLGridManager::getInstance()->getGrid()) onOpen(LLSD());
    if (!mSystemGrid && mLoading && mLoadTimer.getElapsedTimeF32() > 15.f) showFallback();
    if (gAgent.getRegion() && !mLibraryBusy && gAgent.getTeleportState() == LLAgent::TELEPORT_NONE &&
        (gAgent.getRegion()->getRegionID() != mLibraryRegion || gAgent.getRegion()->getCapability("FinalverseWorld") != mLibraryCapability ||
         (mLibraryRetry && mLibraryRetryTimer.getElapsedTimeF32() >= 1.5f))) refreshLibrary();
    if (!mSystemGrid && gAgent.getRegion() && !gAgent.getRegion()->getCapability("FinalverseWorld").empty() && getChild<LLMediaCtrl>("destination_guide_contents")->getVisible()) showFallback();
    if (mPendingRemix.isMap()) { const auto prepared = mPendingRemix; mPendingRemix = LLSD(); LLFloaterFinalverseAI::openPrepared(prepared); }
    if (mLibraryDirty) { mLibraryDirty = false; renderLibrary(); }
    auto selected = getChild<LLScrollListCtrl>("world_regions")->getFirstSelected();
    bool online = false;
    if (selected) for (const auto& d : llsd::inArray(mDestinations)) if (d["region_id"].asString() == selected->getValue().asString()) {
        online = d["online"].asBoolean();
        getChild<LLTextBox>("region_detail")->setText(d["description"].asString() + "\n" + d["license"].asString() + " · " + d["attribution"].asString());
    }
    getChild<LLButton>("library_teleport")->setEnabled(!mLibraryBusy && online);
    getChild<LLButton>("library_refresh")->setEnabled(!mLibraryBusy);
    auto release = getChild<LLScrollListCtrl>("creator_releases")->getFirstSelected();
    bool own = false, free = false;
    if (release) for (const auto& r : llsd::inArray(mReleases)) if (r["release_id"].asString() == release->getValue().asString()) { own = r["owner_id"].asString() == gAgent.getID().asString(); free = r["free"].asBoolean(); }
    getChild<LLButton>("release_publish")->setEnabled(!mLibraryBusy && own);
    getChild<LLButton>("release_unpublish")->setEnabled(!mLibraryBusy && own);
    getChild<LLButton>("release_remix")->setEnabled(!mLibraryBusy && (own || free));
    LLFloater::draw();
}

void LLFloaterDestinations::handleMediaEvent(LLPluginClassMedia* media, EMediaEvent event)
{
    if (mSystemGrid || !getChild<LLMediaCtrl>("destination_guide_contents")->getVisible()) return;
    if (event == MEDIA_EVENT_NAVIGATE_BEGIN) { mLoading = true; mLoadTimer.reset(); }
    if (event == MEDIA_EVENT_NAVIGATE_ERROR_PAGE || event == MEDIA_EVENT_PLUGIN_FAILED ||
        event == MEDIA_EVENT_PLUGIN_FAILED_LAUNCH) { showFallback(); return; }
    if (event == MEDIA_EVENT_LOCATION_CHANGED &&
        !LLGridManager::getInstance()->isDestinationGuideURLAllowed(media->getLocation()))
    {
        showFallback(); return;
    }
    if (event == MEDIA_EVENT_NAVIGATE_COMPLETE)
    {
        if (media->getNavigateResultCode() >= 400 || media->getNavigateResultCode() < 200 ||
            !LLGridManager::getInstance()->isDestinationGuideURLAllowed(media->getNavigateURI()))
        { showFallback(); return; }
        mLoading = false;
        getChild<LLPanel>("destination_fallback")->setVisible(false);
    }
}

namespace
{
LLSD libraryPost(const std::string& cap, const LLSD& body)
{
    LLCoreHttpUtil::HttpCoroutineAdapter adapter("MutSeaLibrary", LLCore::HttpRequest::DEFAULT_POLICY_ID);
    auto request = std::make_shared<LLCore::HttpRequest>();
    auto options = std::make_shared<LLCore::HttpOptions>(); options->setTimeout(8); options->setRetries(0);
    return adapter.postJsonAndSuspend(request, cap, body, options);
}
}
void LLFloaterDestinations::refreshLibrary()
{
    auto region = gAgent.getRegion(); if (!region || mLibraryBusy) return;
    if (gAgent.getTeleportState() != LLAgent::TELEPORT_NONE)
    {
        mLibraryRetry = true; mLibraryRetryTimer.reset();
        mLibraryMessage = "Arriving in your destination…"; mLibraryDirty = true; return;
    }
    const auto capability = region->getCapability("FinalverseWorld");
    if (region->getRegionID() != mLibraryRegion || capability != mLibraryCapability) mLibraryAttempts = 0;
    mLibraryRetry = false;
    mLibraryCapability = capability;
    mLibraryRegion = region->getRegionID(); mDestinations = LLSD::emptyArray(); mReleases = LLSD::emptyArray();
    mLibraryMessage = capability.empty() ? "This grid does not provide a World Library. Use the World Map to explore." : "Checking this grid's destinations and your creator library…";
    mLibraryDirty = true; if (capability.empty()) return;
    mLibraryBusy = true; ++mLibraryAttempts; const U32 generation = ++mLibraryGeneration; const auto handle = getHandle();
    const auto region_id = mLibraryRegion; const auto actor = gAgent.getID();
    LLCoros::instance().launch("MutSeaLibrary", [handle, generation, region_id, actor, capability]() {
        const LLSD reply = libraryPost(capability, LLSD().with("op", "destinations"));
        const LLSD creators = libraryPost(capability, LLSD().with("op", "creator").with("command", "list"));
        auto self = dynamic_cast<LLFloaterDestinations*>(handle.get()); if (!self || self->mLibraryGeneration != generation) return;
        self->mLibraryBusy = false;
        auto current = gAgent.getRegion();
        if (!current || current->getRegionID() != region_id || gAgent.getID() != actor || current->getCapability("FinalverseWorld") != capability) { self->mLibraryDirty = true; return; }
        const bool confirmed = reply["ok"].isBoolean() && reply["ok"].asBoolean();
        self->mDestinations = confirmed ? llfinalverseParseDestinations(reply["result"], region_id.asString(), actor.asString()) : LLSD::emptyArray();
        if (!confirmed)
        {
            self->mLibraryRetry = self->mLibraryAttempts < 3; self->mLibraryRetryTimer.reset();
            self->mLibraryMessage = self->mLibraryRetry ? "Waiting for the region session to finish connecting…" : "World Library is not ready. Refresh or use World Map.";
        }
        else self->mLibraryMessage = self->mDestinations.size() ? "Choose an online region, then Teleport. Shared samples retain their source licenses." : "No curated destinations advertised here yet. Refresh or use the World Map.";
        std::set<std::string> ids;
        if (creators["ok"].asBoolean())
        {
            const auto result = creators["result"];
            if (result["releases"].isArray() && result["releases"].size() <= 101)
                for (const auto& r : llsd::inArray(result["releases"])) if (r["content"]["owner_id"].asString() == actor.asString())
                { LLSD row; row["release_id"] = r["release_id"]; row["owner_id"] = actor.asString(); row["title"] = r["content"]["title"]; row["state"] = "Private release"; row["free"] = true; self->mReleases.append(row); ids.insert(r["release_id"].asString()); }
            if (result["listings"].isArray() && result["listings"].size() <= 256)
                for (const auto& l : llsd::inArray(result["listings"]))
                {
                    if (ids.count(l["release_id"].asString())) for (S32 i = 0; i < self->mReleases.size(); ++i) { auto& r = self->mReleases[i]; if (r["release_id"] == l["release_id"]) r["state"] = l["price_minor"].asInteger() == 0 ? "Published free · CC0" : "Offer · checkout unavailable"; }
                    else { LLSD row = l; row["state"] = l["price_minor"].asInteger() == 0 ? "Free · CC0" : "Offer · checkout unavailable"; row["free"] = l["price_minor"].asInteger() == 0; self->mReleases.append(row); }
                }
        }
        self->mLibraryDirty = true;
    });
}
void LLFloaterDestinations::manageRelease(const std::string& command)
{
    auto selected = getChild<LLScrollListCtrl>("creator_releases")->getFirstSelected(); auto region = gAgent.getRegion();
    if (!selected || mLibraryBusy || !region || region->getRegionID() != mLibraryRegion) return;
    const auto cap = region->getCapability("FinalverseWorld"); if (cap.empty()) return;
    const auto actor = gAgent.getID(); const auto region_id = mLibraryRegion; const auto handle = getHandle();
    LLSD body; body["op"] = "creator"; body["command"] = command; body["release_id"] = selected->getValue();
    mLibraryBusy = true; mLibraryMessage = "Checking your release and world permissions…"; mLibraryDirty = true;
    LLCoros::instance().launch("MutSeaCreator", [handle, cap, actor, region_id, body, command]() {
        const auto reply = libraryPost(cap, body); auto self = dynamic_cast<LLFloaterDestinations*>(handle.get()); if (!self) return;
        self->mLibraryBusy = false; auto current = gAgent.getRegion();
        if (!current || current->getRegionID() != region_id || gAgent.getID() != actor || current->getCapability("FinalverseWorld") != cap) return;
        if (!reply["ok"].asBoolean()) { self->mLibraryMessage = reply["error"].isString() ? reply["error"].asString() : "The service did not confirm the operation. Refresh before retrying."; self->mLibraryDirty = true; return; }
        if (command == "remix") { self->mLibraryMessage = "A remix preview is ready. Open AI Create to review it."; self->mPendingRemix = reply["result"]; self->mLibraryDirty = true; }
        else self->refreshLibrary();
    });
}
void LLFloaterDestinations::renderLibrary()
{
    llassert(LLCoros::on_main_coro());
    getChild<LLTextBox>("library_status")->setText(mLibraryMessage);
    auto list = getChild<LLScrollListCtrl>("world_regions"); const LLSD previous = list->getSelectedValue(); list->deleteAllItems();
    for (const auto& d : llsd::inArray(mDestinations))
    {
        LLSD row; row["value"] = d["region_id"]; row["columns"][0]["column"] = "title"; row["columns"][0]["value"] = d["title"];
        row["columns"][1]["column"] = "status"; row["columns"][1]["value"] = d["online"].asBoolean() ? "Online" : "Offline"; list->addElement(row);
    }
    if (!list->selectByValue(previous) && !list->selectByValue(LLSD(mLibraryRegion.asString()))) list->selectFirstItem();
    if (!list->getFirstSelected()) getChild<LLTextBox>("region_detail")->setText(std::string());
    auto releases = getChild<LLScrollListCtrl>("creator_releases"); releases->deleteAllItems();
    for (const auto& r : llsd::inArray(mReleases))
    {
        LLSD row; row["value"] = r["release_id"]; row["columns"][0]["column"] = "title"; row["columns"][0]["value"] = r["title"];
        row["columns"][1]["column"] = "status"; row["columns"][1]["value"] = r["state"]; releases->addElement(row);
    }
    releases->selectFirstItem();
}
