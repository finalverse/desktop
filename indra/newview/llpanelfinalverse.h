// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_PANEL_FINALVERSE_H
#define LL_PANEL_FINALVERSE_H
#include "llpanel.h"
#include "lltimer.h"
#include "llselectmgr.h"

// Thin experience facade. Existing commands, floaters and world capabilities
// remain responsible for their own permissions and behavior.
class LLPanelFinalverse : public LLPanel
{
public:
    LLPanelFinalverse();
    ~LLPanelFinalverse() override;
    static LLPanelFinalverse* instance() { return sInstance; }
    void action(const std::string& name);
    void command(const std::string& text);
    bool selecting() const { return mSelecting; }
    bool postBuild() override;
    void reshape(S32 width, S32 height, bool called_from_parent = true) override;
    void draw() override;
private:
    void refreshExperience();
    void drawExperience();
    void layout();
    void setExperience(bool simple);
    void setViewMode(const std::string& mode);
    static LLPanelFinalverse* sInstance;
    std::string mRegionName;
    U64 mReadRevision = 0;
    LLTimer mRefresh;
    bool mSimple = true;
    bool mSelecting = false;
    std::string mViewMode = "world";
    // The native picker relies on a UI owner retaining the selection. Otherwise
    // LLViewerWindow's normal deselectUnused() cleanup releases it next frame.
    LLObjectSelectionHandle mSelection;
    F64 mDrawSeconds = 0.;
    U32 mDrawSamples = 0;
};
#endif
