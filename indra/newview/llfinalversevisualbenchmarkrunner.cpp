// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalversevisualbenchmark.h"
#include "llagent.h"
#include "llappviewer.h"
#include "llenvironment.h"
#include "llfocusmgr.h"
#include "llsdserialize.h"
#include "llsdutil_math.h"
#include "llsettingsvo.h"
#include "llstartup.h"
#include "llversioninfo.h"
#include "llviewercamera.h"
#include "llviewercontrol.h"
#include "llviewernetwork.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llwindow.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>

namespace
{
namespace fs = std::filesystem;
void privateFile(const fs::path& path)
{
    std::error_code ec;
    fs::permissions(path, fs::perms::owner_read | fs::perms::owner_write, fs::perm_options::replace, ec);
}
class Benchmark
{
public:
    void idle(double dt, bool quitting)
    {
        if (mDone || quitting || LLStartUp::getStartupState() != STATE_STARTED || !gAgent.getRegion()) return;
        if (!mLoaded)
        {
            const std::string path = gSavedSettings.getString("FinalverseVisualBenchmarkFile");
            if (path.empty()) return;
            mLoaded = true;
            std::ifstream input(path);
            std::string error;
            if (!input || LLSDSerialize::fromXML(mSpec, input) == LLSDParser::PARSE_FAILURE ||
                !llfinalverseValidateVisualBenchmark(mSpec, error))
            {
                fail("Invalid local benchmark specification.");
                return;
            }
            std::vector<std::string> uris;
            LLGridManager::getInstance()->getLoginURIs(uris);
            if (uris.size() != 1 || uris.front() != mSpec["grid_login_uri"].asString() ||
                gAgent.getRegion()->getRegionID() != mSpec["region_id"].asUUID())
            {
                fail("Benchmark requires its designated loopback grid and region.");
                return;
            }
            std::error_code ec;
            const fs::path destination(mSpec["output_dir"].asString());
            if (!fs::create_directory(destination, ec) || ec)
            {
                fail("Benchmark output directory must be new and have an existing parent.");
                return;
            }
            fs::permissions(destination, fs::perms::owner_all, fs::perm_options::replace, ec);
            if (ec) { fail("Cannot protect benchmark output directory."); return; }
            mOutput = destination;
            if (mSpec.has("capture_size"))
            {
                // Cocoa setSize takes logical points while reshape/snapshots
                // report backing pixels. Other native implementations use
                // pixel dimensions; do not divide their DPI-scaled UI units.
#if LL_DARWIN
                mNativeScale = gViewerWindow->getWindow()->getSystemUISize();
#endif
                if (!std::isfinite(mNativeScale) || mNativeScale < .5f || mNativeScale > 4.f)
                {
                    fail("Invalid native backing-pixel scale.");
                    return;
                }
                mNativeSize = LLCoordWindow(ll_round(mSpec["capture_size"][0].asInteger() / mNativeScale),
                                           ll_round(mSpec["capture_size"][1].asInteger() / mNativeScale));
                if (!gViewerWindow->getWindow()->setSize(mNativeSize))
                {
                    fail("Native window API rejected the requested capture size.");
                    return;
                }
            }
            if (mSpec.has("environment"))
            {
                LLSD sky_data = mSpec["environment"]["sky"], water_data = mSpec["environment"]["water"];
                auto sky_rules = LLSettingsSky::validationList(), water_rules = LLSettingsWater::validationList();
                const auto sky_validation = LLSettingsBase::settingValidation(sky_data, sky_rules);
                const auto water_validation = LLSettingsBase::settingValidation(water_data, water_rules);
                if (!sky_validation["success"].asBoolean() || !water_validation["success"].asBoolean())
                {
                    fail("Native environment validation rejected the local preview.");
                    return;
                }
                auto& environment = LLEnvironment::instance();
                environment.setEnvironment(LLEnvironment::ENV_LOCAL, LLSettingsVOSky::buildSky(sky_data), LLSettingsVOWater::buildWater(water_data));
                environment.setSelectedEnvironment(LLEnvironment::ENV_LOCAL, LLSettingsBase::Seconds(0.), true);
                environment.updateEnvironment(LLSettingsBase::Seconds(0.), true);
            }
            mReport["spec"] = mSpec;
            mReport["viewer"] = LLVersionInfo::instance().getChannelAndVersion();
            mReport["status"] = "running";
            mReport["started_utc"] = LLDate::now();
            saveReport();
        }
        if (gAgent.getRegion()->getRegionID() != mSpec["region_id"].asUUID())
        {
            fail("Region changed during benchmark.");
            return;
        }
        auto* window = gViewerWindow->getWindow();
        if (!gFocusMgr.getAppHasFocus() || window->getMinimized() || !window->getVisible())
        {
            resetSample();
            return;
        }
        if (!std::isfinite(dt) || dt <= 0.) { resetSample(); return; }
        const LLSD& view = mSpec["views"][mView];
        auto* camera = LLViewerCamera::getInstance();
        const auto origin = gAgent.getRegion()->getOriginGlobal();
        camera->lookAt(gAgent.getPosAgentFromGlobal(origin + ll_vector3d_from_sd(view["position"])),
                       gAgent.getPosAgentFromGlobal(origin + ll_vector3d_from_sd(view["focus"])));
        camera->setViewNoBroadcast(static_cast<F32>(view["fov"].asReal()));
        const S32 count = gObjectList.getNumObjects();
        const S32 width = gViewerWindow->getWindowWidthRaw(), height = gViewerWindow->getWindowHeightRaw();
        if (mSpec.has("capture_size") && (width != mSpec["capture_size"][0].asInteger() ||
            height != mSpec["capture_size"][1].asInteger()))
        {
            // Native resize callbacks may arrive on a later frame. Bound the
            // wait and fail rather than recording a mismatched comparison.
            mReport["observed_pixels"] = LLSD::emptyArray();
            mReport["observed_pixels"].append(width);
            mReport["observed_pixels"].append(height);
            mResizeWait += dt;
            if (!mResizeCorrected && mResizeWait >= .5)
            {
                // The inherited Cocoa implementation adds a fixed titlebar
                // allowance. Measure any actual decoration difference once;
                // avoid hard-coding a current macOS titlebar height.
                mResizeCorrected = true;
                mNativeSize.mX += ll_round((mSpec["capture_size"][0].asInteger() - width) / mNativeScale);
                mNativeSize.mY += ll_round((mSpec["capture_size"][1].asInteger() - height) / mNativeScale);
                if (mNativeSize.mX <= 0 || mNativeSize.mY <= 0 ||
                    mNativeSize.mX > 5120 || mNativeSize.mY > 3200 || !window->setSize(mNativeSize))
                {
                    fail("Native window API rejected the measured size correction.");
                    return;
                }
            }
            if (mResizeWait > 5.) fail("Capture pixels did not match the requested native window size.");
            return;
        }
        if (count != mObjectCount || width != mWidth || height != mHeight)
        {
            mObjectCount = count; mWidth = width; mHeight = height;
            resetSample();
        }
        if (mWarmup < mSpec["warmup_seconds"].asReal()) { mWarmup += dt; return; }
        mFrames.push_back(dt);
        mSampleTime += dt;
        if (mSampleTime < mSpec["sample_seconds"].asReal()) return;

        LLSD result = llfinalverseFrameSummary(mFrames);
        result["camera"] = view;
        result["width_raw"] = width; result["height_raw"] = height;
        result["cached_objects"] = count; // Cached viewer objects, not authoritative simulator population.
        result["captured_utc"] = LLDate::now();
        for (const char* key : {"RenderFarClip", "RenderShadowDetail", "RenderShadowResolutionScale",
            "RenderDeferredSSAO", "RenderFSAAType", "RenderFSAASamples", "RenderAnisotropic",
            "RenderAnisotropicLevel", "RenderReflectionProbeDetail", "RenderReflectionsEnabled",
            "RenderTonemapType", "RenderExposure", "RenderAvatarMaxNonImpostors", "TextureMemory"})
        {
            if (gSavedSettings.controlExists(key)) result["graphics"][key] = gSavedSettings.getLLSD(key);
        }
        auto& environment = LLEnvironment::instance();
        result["environment_selection"] = static_cast<S32>(environment.getSelectedEnvironment());
        if (auto sky = environment.getCurrentSky()) result["sky"] = sky->getSettings();
        if (auto water = environment.getCurrentWater()) result["water"] = water->getSettings();
        const auto stem = view["name"].asString();
        std::ofstream csv(mOutput / (stem + ".csv"));
        csv << "frame,seconds\n" << std::setprecision(10);
        for (size_t i = 0; i < mFrames.size(); ++i) csv << i << ',' << mFrames[i] << '\n';
        csv.close(); privateFile(mOutput / (stem + ".csv"));
        result["frame_csv_saved"] = !csv.fail();
        const auto png = mOutput / (stem + ".png");
        result["screenshot_saved"] = gViewerWindow->saveSnapshot(png.string(), width, height,
            false, false, true, false, LLSnapshotModel::SNAPSHOT_TYPE_COLOR, LLSnapshotModel::SNAPSHOT_FORMAT_PNG);
        privateFile(png);
        mReport["views"].append(result);
        LL_INFOS("FinalverseBenchmark") << stem << " " << result["fps"].asReal() << " FPS" << LL_ENDL;
        ++mView;
        if (!result["screenshot_saved"].asBoolean() || !result["frame_csv_saved"].asBoolean())
            fail("Benchmark evidence write failed.");
        else if (mView == mSpec["views"].size())
        {
            mDone = true;
            mReport["status"] = "complete";
            if (saveReport() && mSpec["quit_after_capture"].asBoolean())
            {
                // Already validated against the one loopback grid and active
                // region. Use the normal save/upload/logout state machine;
                // avoid both the UI confirmation and signal-based termination.
                LLAppViewer::instance()->requestQuit();
            }
        }
        else { resetSample(); saveReport(); }
    }
private:
    void resetSample() { mWarmup = mSampleTime = 0.; mFrames.clear(); }
    void fail(const char* reason)
    {
        mDone = true; mReport["status"] = "failed"; mReport["error"] = reason;
        LL_WARNS("FinalverseBenchmark") << reason << LL_ENDL;
        if (!mOutput.empty() && fs::is_directory(mOutput)) saveReport();
    }
    bool saveReport()
    {
        std::ofstream output(mOutput / "results.xml");
        LLSDSerialize::toPrettyXML(mReport, output);
        output.close(); privateFile(mOutput / "results.xml");
        return !output.fail();
    }
    double mResizeWait = 0.;
    F32 mNativeScale = 1.f;
    LLCoordWindow mNativeSize;
    bool mResizeCorrected = false;
    LLSD mSpec, mReport;
    fs::path mOutput;
    bool mLoaded = false, mDone = false;
    S32 mView = 0, mObjectCount = -1, mWidth = 0, mHeight = 0;
    double mWarmup = 0., mSampleTime = 0.;
    std::vector<double> mFrames;
};
}
void llfinalverseVisualBenchmarkIdle(double frame_seconds, bool quitting)
{
    static Benchmark benchmark;
    benchmark.idle(frame_seconds, quitting);
}
