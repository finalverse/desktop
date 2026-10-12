// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalversevisualbenchmark.h"
#include "llgroundplane.h"
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
#include "llviewermenu.h"
#include "llviewercontrol.h"
#include "llviewernetwork.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llwindow.h"
#include "llvoavatarself.h"
#include "llvovolume.h"
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#if LL_DARWIN || LL_LINUX
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace
{
namespace fs = std::filesystem;
void privateFile(const fs::path& path)
{
    std::error_code ec;
    fs::permissions(path, fs::perms::owner_read | fs::perms::owner_write, fs::perm_options::replace, ec);
}
bool ownedPrivate(const fs::path& path, bool directory)
{
#if LL_DARWIN || LL_LINUX
    struct stat info;
    return !lstat(path.c_str(), &info) && info.st_uid == getuid() &&
        (directory ? S_ISDIR(info.st_mode) && (info.st_mode & 0777) == 0700 :
         S_ISREG(info.st_mode) && (info.st_mode & 0777) == 0600 && info.st_size > 0 && info.st_size <= 1048576);
#else
    return false; // No private command-file endpoint on unvalidated platforms.
#endif
}
class Benchmark
{
public:
    explicit Benchmark(const std::string& path = {}) : mPath(path), mStarted(std::chrono::steady_clock::now()) {}
    bool done() const { return mDone; }
    const LLSD& report() const { return mReport; }
    void idle(double dt, bool quitting)
    {
        if (mDone || quitting || LLStartUp::getStartupState() != STATE_STARTED || !gAgent.getRegion()) return;
        if (!mLoaded)
        {
            const std::string path = mPath.empty() ? gSavedSettings.getString("FinalverseVisualBenchmarkFile") : mPath;
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
            mQuick = mSpec["quick_mode"].asBoolean();
            if (mQuick)
            {
                auto* camera = LLViewerCamera::getInstance();
                mSavedOrigin = camera->getOrigin(); mSavedFocus = camera->getPointOfInterest(); mSavedFov = camera->getView();
                mSavedCamera = true;
            }
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
        if (mQuick && std::chrono::duration<double>(std::chrono::steady_clock::now() - mStarted).count() > 180.)
        {
            fail("Quick check exceeded its wall-clock deadline; keep the viewer visible and focused.");
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
        if (mSpec.has("interaction") && !interaction(dt)) return;
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
        result["avatar"] = avatarState();
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
        const auto stem = view["name"].asString() + (mInteractionPhase == 3 ? "_seated" : mInteractionPhase == 5 ? "_standing" : "");
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
        if (!result["screenshot_saved"].asBoolean() || !result["frame_csv_saved"].asBoolean())
            fail("Benchmark evidence write failed.");
        else if (mInteractionPhase == 3)
        {
            gAgent.standUp(); mInteractionPhase = 4; mPhaseTime = 0.; resetSample(); saveReport();
        }
        else if (++mView == mSpec["views"].size())
        {
            mDone = true;
            restore();
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
    LLSD avatarState() const
    {
        LLSD result;
        result["position"] = ll_sd_from_vector3d(gAgent.getPositionGlobal() - gAgent.getRegion()->getOriginGlobal());
        result["seated"] = gAgent.isSitting();
        result["flying"] = gAgent.getFlying();
        result["autopilot"] = gAgent.getAutoPilot();
        if (gAgentAvatarp)
        {
            result["foot_plane"] = LLSD::emptyArray();
            for (int i = 0; i != 4; ++i) result["foot_plane"].append(gAgentAvatarp->mFootPlane.mV[i]);
            result["foot_plane_has_surface"] = llFootPlaneHasSurface(gAgentAvatarp->mFootPlane.mV);
            result["rotation"] = ll_sd_from_quaternion(gAgentAvatarp->getRotationRegion());
            auto* parent = static_cast<LLViewerObject*>(gAgentAvatarp->getParent());
            result["seat_root_id"] = parent ? parent->getRootEdit()->getID() : LLUUID::null;
            result["playing_animations"] = LLSD::emptyArray();
            for (const auto& animation : gAgentAvatarp->mPlayingAnimations) result["playing_animations"].append(animation.first);
        }
        return result;
    }
    bool interaction(double dt)
    {
        mPhaseTime += dt; mTraceTime += dt;
        if (mTraceTime >= .2)
        {
            mTraceTime = 0.; LLSD state = avatarState(); state["phase"] = mInteractionPhase;
            mReport["interaction_trace"].append(state);
        }
        const LLSD& action = mSpec["interaction"];
        auto* seat = gObjectList.findObject(action["seat_id"].asUUID());
        // Owned fixture benches by default; any_owner is an explicit opt-in for shared
        // (for example estate-provisioned) seating. Sitting is a native, server-permitted
        // action that changes no content; the exact ID and position checks still apply.
        if (!seat || seat->getRegion() != gAgent.getRegion() || !(seat->permYouOwner() || action["any_owner"].asBoolean()) ||
            dist_vec(seat->getPositionRegion(), ll_vector3_from_sd(action["expected_position"])) > .05f)
        {
            fail("Fixture bench, its ownership policy or its expected native position did not match."); return false;
        }
        // Approach in front of the seat in its own frame (benches face their local -Y).
        const LLVector3d approach = gAgent.getRegion()->getOriginGlobal() +
            LLVector3d(seat->getPositionRegion() + LLVector3(0.f, -1.4f, .8f) * seat->getRotationRegion());
        if (mInteractionPhase == 0)
        {
            if (gAgent.isSitting() || gAgent.getAutoPilot() || gAgent.getFlying())
            { fail("Bench check requires an idle, standing, grounded avatar."); return false; }
            mInteractionPhase = 1; mPhaseTime = 0.; mOwnAutoPilot = true;
            gAgent.startAutoPilotGlobal(approach, "Finalverse bench acceptance", nullptr, nullptr, nullptr, .4f, .03f, false);
            return false;
        }
        if (mInteractionPhase == 1)
        {
            if (mPhaseTime > 45.) { fail("Grounded native approach timed out."); return false; }
            if (gAgent.getAutoPilot()) return false;
            mOwnAutoPilot = false;
            const auto offset = gAgent.getPositionGlobal() - approach;
            if (offset.mdV[0] * offset.mdV[0] + offset.mdV[1] * offset.mdV[1] > 1.)
            { fail("Grounded native approach was obstructed or cancelled."); return false; }
            mReport["approach"] = avatarState();
            const auto png = mOutput / (mSpec["views"][0]["name"].asString() + "_approach.png");
            const bool saved = gViewerWindow->saveSnapshot(png.string(), gViewerWindow->getWindowWidthRaw(),
                gViewerWindow->getWindowHeightRaw(), false, false, true, false,
                LLSnapshotModel::SNAPSHOT_TYPE_COLOR, LLSnapshotModel::SNAPSHOT_FORMAT_PNG);
            privateFile(png);
            if (!saved) { fail("Approach screenshot failed."); return false; }
            handle_object_sit(seat->getID()); mRequestedSit = true; mInteractionPhase = 2; mPhaseTime = 0.;
            return false;
        }
        if (mInteractionPhase == 2)
        {
            if (mPhaseTime > 10.) { fail("Native sit confirmation timed out."); return false; }
            if (!gAgent.isSitting()) return false;
            if (avatarState()["seat_root_id"].asUUID() != seat->getRootEdit()->getID())
            { fail("Avatar seated on a different object."); return false; }
            mInteractionPhase = 3; resetSample();
        }
        if (mInteractionPhase == 4)
        {
            if (mPhaseTime > 10.) { fail("Native stand confirmation timed out."); return false; }
            if (gAgent.isSitting()) return false;
            mRequestedSit = false; mInteractionPhase = 5; resetSample();
        }
        return mInteractionPhase == 3 || mInteractionPhase == 5;
    }
    void restore()
    {
        if (mOwnAutoPilot) { gAgent.stopAutoPilot(); mOwnAutoPilot = false; }
        if (mRequestedSit) { gAgent.standUp(); mRequestedSit = false; }
        if (mSavedCamera)
        {
            auto* camera = LLViewerCamera::getInstance();
            camera->lookAt(mSavedOrigin, mSavedFocus); camera->setViewNoBroadcast(mSavedFov);
            mReport["camera_restored"] = true;
            mReport["restored_fov"] = camera->getView();
            mSavedCamera = false;
        }
    }
    void resetSample() { mWarmup = mSampleTime = 0.; mFrames.clear(); }
    void fail(const char* reason)
    {
        mDone = true; mReport["status"] = "failed"; mReport["error"] = reason;
        restore();
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
    std::string mPath;
    std::chrono::steady_clock::time_point mStarted;
    bool mQuick = false, mSavedCamera = false, mOwnAutoPilot = false, mRequestedSit = false;
    LLVector3 mSavedOrigin, mSavedFocus;
    F32 mSavedFov = 1.f;
    S32 mInteractionPhase = 0;
    double mPhaseTime = 0., mTraceTime = 0.;
};

class QuickChecks
{
public:
    void idle(double dt, bool quitting)
    {
        if (mActive)
        {
            if (quitting) return; // Viewer shutdown performs normal grid logout.
            mActive->idle(dt, quitting);
            if (mActive->done())
            {
                std::ofstream output(mRoot / "status.xml");
                LLSDSerialize::toPrettyXML(mActive->report(), output); output.close(); privateFile(mRoot / "status.xml");
                std::error_code ec; fs::remove(mRoot / "active.xml", ec); mActive.reset();
            }
            return;
        }
        if (quitting || LLStartUp::getStartupState() != STATE_STARTED || !gAgent.getRegion()) return;
        const auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(now - mLastPoll).count() < .25) return;
        mLastPoll = now;
        const auto configured = gSavedSettings.getString("FinalverseQuickVisualDirectory");
        if (configured.empty()) return;
        mRoot = fs::path(configured);
        if (!mRoot.is_absolute() || !ownedPrivate(mRoot, true) || !ownedPrivate(mRoot / "request.xml", false)) return;
        std::vector<std::string> uris; LLGridManager::getInstance()->getLoginURIs(uris);
        if (uris.size() != 1 || uris.front() != "http://127.0.0.1:18094/" ||
            gAgent.getRegion()->getRegionID() != LLUUID("a9d44538-26ca-4586-a8a9-e551722ffa15")) return;
        LLSD spec; std::ifstream input(mRoot / "request.xml"); std::string error;
        const bool valid = input && LLSDSerialize::fromXML(spec, input) != LLSDParser::PARSE_FAILURE &&
            llfinalverseValidateVisualBenchmark(spec, error) && spec["quick_mode"].asBoolean() &&
            fs::path(spec["output_dir"].asString()).parent_path() == mRoot;
        input.close();
        std::error_code ec;
        if (fs::exists(mRoot / "active.xml", ec)) return;
        fs::rename(mRoot / "request.xml", mRoot / "active.xml", ec);
        if (ec) return;
        if (!valid)
        {
            LLSD rejected; rejected["status"] = "failed"; rejected["error"] = "Invalid bounded quick-check request.";
            std::ofstream output(mRoot / "status.xml"); LLSDSerialize::toPrettyXML(rejected, output);
            output.close(); privateFile(mRoot / "status.xml"); fs::remove(mRoot / "active.xml", ec);
            return;
        }
        mActive = std::make_unique<Benchmark>((mRoot / "active.xml").string());
    }
private:
    fs::path mRoot;
    std::unique_ptr<Benchmark> mActive;
    std::chrono::steady_clock::time_point mLastPoll;
};
}
void llfinalverseVisualBenchmarkIdle(double frame_seconds, bool quitting)
{
    static Benchmark benchmark;
    benchmark.idle(frame_seconds, quitting);
    if (gSavedSettings.getString("FinalverseVisualBenchmarkFile").empty() || benchmark.done())
    {
        static QuickChecks quick;
        quick.idle(frame_seconds, quitting);
    }
}
