// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FLOATER_FINALVERSE_STORM_H
#define LL_FLOATER_FINALVERSE_STORM_H
#include "llfloater.h"
#include "lltimer.h"
#include "llviewertexture.h"
#include <memory>
class LLFloaterFinalverseStorm : public LLFloater
{
public:
    explicit LLFloaterFinalverseStorm(const LLSD& key);
    ~LLFloaterFinalverseStorm() override;
    bool postBuild() override;
    void onOpen(const LLSD& key) override;
    void onClose(bool app_quitting) override;
    void draw() override;

private:
    struct Worker;
    void                       stop();
    std::unique_ptr<Worker>    mWorker;
    LLPointer<LLViewerTexture> mImage;
    LLTimer                    mRefresh;
    LLUUID                     mActor, mRegion, mSession;
};
#endif
