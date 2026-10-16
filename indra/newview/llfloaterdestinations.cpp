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
    // These are suggestions, not teleports to hard-coded coordinates. The
    // existing World API resolves the connected world's actual entities.
    for (const char* place : {"harbor", "home", "pavilion", "workshop", "dock"})
    {
        const std::string name(place);
        getChild<LLButton>("discover_" + name)->setCommitCallback([this, name](LLUICtrl*, const LLSD&)
        {
            LLFloaterFinalverseAI::openRequest("Tell me about " + getChild<LLButton>("discover_" + name)->getLabelUnselected() + " in this world.", false);
        });
    }
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
    auto grid = LLGridManager::getInstance();
    mGrid = grid->getGrid();
    mSystemGrid = grid->isSystemGrid();
    auto media = getChild<LLMediaCtrl>("destination_guide_contents");
    media->setTrustedContent(mSystemGrid);
    media->setErrorPageURL(mSystemGrid ? gSavedSettings.getString("GenericErrorPageURL") : "");
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
