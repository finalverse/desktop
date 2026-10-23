// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalversestorm.h"
#include "finalstorm/finalstorm_desktop.h"
#include "llagent.h"
#include "llappviewer.h"
#include "llbutton.h"
#include "llfloaterreg.h"
#include "llimage.h"
#include "lldrawable.h"
#include "llface.h"
#include "llsdjson.h"
#include "lltextbox.h"
#include "llviewercamera.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llviewertexture.h"
#include "llvolume.h"
#include "lltextureentry.h"
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <map>
#include <thread>
#if LL_DARWIN || LL_LINUX
#include <dlfcn.h>
#endif

namespace
{
constexpr int WIDTH = 640, HEIGHT = 360, TEX_WIDTH = 1024, TEX_HEIGHT = 512;
LLSD          vector(const F32* v, int count)
{
    LLSD a = LLSD::emptyArray();
    for (int i = 0; i < count; ++i)
        a.append(F64(v[i]));
    return a;
}
// Called only on the UI thread: copy data already decoded by the native viewer.
// No capability, session secret, texture URL or world write crosses this boundary.
std::string captureNativeScene()
{
    auto region = gAgent.getRegion();
    if (!region)
        return {};
    LLSD root;
    root["schema"] = "finalverse.desktop-scene/v2";
    root["actor"]  = gAgentID.asString();
    root["region"] = region->getRegionID().asString();
    auto camera    = LLViewerCamera::getInstance();
    auto global    = gAgent.getPosGlobalFromAgent(camera->getOrigin()) - region->getOriginGlobal();
    LLSD p         = LLSD::emptyArray();
    for (int i = 0; i < 3; ++i)
        p.append(global.mdV[i]);
    root["camera"]["position"] = p;
    root["camera"]["forward"]  = vector(camera->getAtAxis().mV, 3);
    root["camera"]["up"]       = vector(camera->getUpAxis().mV, 3);
    root["camera"]["fov"]      = camera->getView();
    root["objects"]            = LLSD::emptyArray();
    root["textures"]           = LLSD::emptyArray();
    std::map<LLUUID, bool> textureCopies;
    auto                   copyTexture = [&](LLViewerTexture* image)
    {
        if (!image || (image->getType() != LLViewerTexture::FETCHED_TEXTURE && image->getType() != LLViewerTexture::LOD_TEXTURE))
            return false; // Media surfaces and private capability URLs never cross the ABI.
        const auto id    = image->getID();
        auto       found = textureCopies.find(id);
        if (found != textureCopies.end())
            return found->second;
        if (id.isNull() || textureCopies.size() >= 16)
            return false;
        textureCopies[id] = false;
        auto fetched      = static_cast<LLViewerFetchedTexture*>(image);
        // Ask the existing asynchronous decoder for a small retained level.
        // Never wait for a network fetch or read back the native GL texture.
        fetched->forceToSaveRawImage(MAX_DISCARD_LEVEL, 2.f);
        LLPointer<LLImageRaw> raw = fetched->getSavedRawImage();
        if (!raw)
            return false;
        LLSD texture;
        texture["id"] = id.asString();
        std::string hex;
        {
            LLImageDataLock lock(raw);
            const int       width = raw->getWidth(), height = raw->getHeight(), components = raw->getComponents();
            if (!raw->getData() || width < 1 || height < 1 || width > 4096 || height > 4096 || components < 1 || components > 4)
                return false;
            const int w = llmin(width, 128), h = llmin(height, 128);
            texture["width"]  = w;
            texture["height"] = h;
            hex.reserve(w * h * 8);
            constexpr char digits[] = "0123456789abcdef";
            for (int y = 0; y < h; ++y)
                for (int x = 0; x < w; ++x)
                {
                    const auto pixel  = raw->getData() + ((y * height / h) * width + x * width / w) * components;
                    const U8   rgba[] = { pixel[0], pixel[components < 3 ? 0 : 1], pixel[components < 3 ? 0 : 2],
                                          U8(components == 2   ? pixel[1]
                                             : components == 4 ? pixel[3]
                                                               : 255) };
                    for (auto channel : rgba)
                    {
                        hex.push_back(digits[channel >> 4]);
                        hex.push_back(digits[channel & 15]);
                    }
                }
        }
        texture["rgba_hex"] = hex;
        root["textures"].append(texture);
        textureCopies[id] = true;
        return true;
    };
    int vertices = 0, indices = 0, faces = 0;
    for (int i = 0; i < gObjectList.getNumObjects() && root["objects"].size() < 256; ++i)
    {
        auto object = gObjectList.getObject(i);
        if (!object || object->isDead() || object->getRegion() != region || object->isAttachment() || object->isAvatar())
            continue;
        auto volume = object->getVolume();
        if (!volume)
            continue;
        // The viewer resolves child/linkset transforms into region coordinates.
        LLSD item;
        item["id"]          = object->getID().asString();
        item["position"]    = vector(object->getPositionRegion().mV, 3);
        const auto rotation = object->getRotationRegion();
        item["rotation"]    = vector(rotation.mQ, 4);
        item["scale"]       = vector(object->getScale().mV, 3);
        item["faces"]       = LLSD::emptyArray();
        for (int f = 0; f < volume->getNumVolumeFaces() && f < 64; ++f)
        {
            const auto& face = volume->getVolumeFace(f);
            if (face.mNumVertices < 3 || face.mNumIndices < 3 || !face.mPositions || !face.mNormals || !face.mIndices ||
                vertices + face.mNumVertices > 65536 || indices + face.mNumIndices > 196608 || faces >= 1024)
                continue;
            LLSD out;
            out["positions"] = LLSD::emptyArray();
            out["normals"]   = LLSD::emptyArray();
            out["uvs"]       = LLSD::emptyArray();
            out["indices"]   = LLSD::emptyArray();
            // Procedural volume buffers may contain unused vertices or zero/
            // unnormalized normals. Export referenced nondegenerate triangles,
            // normalize the native normals as its shader does, and reconstruct
            // missing normals from the actual triangle geometry.
            std::vector<int>       remap(face.mNumVertices, -1), used;
            std::vector<LLVector3> geometricNormals(face.mNumVertices);
            for (int t = 0; t + 2 < face.mNumIndices; t += 3)
            {
                const int a = face.mIndices[t], b = face.mIndices[t + 1], c = face.mIndices[t + 2];
                if (a >= face.mNumVertices || b >= face.mNumVertices || c >= face.mNumVertices)
                    continue;
                const LLVector3 p0(face.mPositions[a].getF32ptr()), p1(face.mPositions[b].getF32ptr()), p2(face.mPositions[c].getF32ptr());
                auto            normal = (p1 - p0) % (p2 - p0);
                if (!normal.isFinite() || normal.lengthSquared() <= 1e-16f)
                    continue;
                for (int index : { a, b, c })
                {
                    geometricNormals[index] += normal;
                    if (remap[index] < 0)
                    {
                        remap[index] = used.size();
                        used.push_back(index);
                    }
                    out["indices"].append(remap[index]);
                }
            }
            if (out["indices"].size() < 3)
                continue;
            auto te         = object->getTE(f);
            auto nativeFace = object->mDrawable ? object->mDrawable->getFace(f) : nullptr;
            // Legacy default UV mapping includes native TE offsets, rotation,
            // repeats and animation. PBR and planar projection use native fallback.
            const bool mapped =
                te && !te->getGLTFRenderMaterial() && te->getTexGen() == LLTextureEntry::TEX_GEN_DEFAULT && nativeFace && face.mTexCoords;
            auto image = mapped ? object->getTEImage(f) : nullptr;
            if (image && copyTexture(image))
                out["texture"] = image->getID().asString();
            bool validNormals = true;
            for (int v : used)
            {
                out["positions"].append(vector(face.mPositions[v].getF32ptr(), 3));
                LLVector3 normal(face.mNormals[v].getF32ptr());
                if (!normal.isFinite() || normal.lengthSquared() < 1e-12f)
                    normal = geometricNormals[v];
                if (!normal.isFinite() || normal.normalize() < 1e-6f)
                {
                    validNormals = false;
                    break;
                }
                out["normals"].append(vector(normal.mV, 3));
                if (face.mTexCoords)
                {
                    const auto uv = mapped ? nativeFace->surfaceToTexture(face.mTexCoords[v], face.mPositions[v], face.mNormals[v])
                                           : face.mTexCoords[v];
                    out["uvs"].append(vector(uv.mV, 2));
                }
            }
            if (!validNormals)
                continue;
            const LLColor4 color = te ? te->getColor() : LLColor4::white;
            out["color"]         = vector(color.mV, 4);
            vertices += used.size();
            indices += out["indices"].size();
            ++faces;
            item["faces"].append(out);
        }
        if (item["faces"].size())
            root["objects"].append(item);
    }
    auto json = boost::json::serialize(LlsdToJson(root));
    return json.size() <= 8 * 1024 * 1024 ? json : std::string();
}
} // namespace

struct LLFloaterFinalverseStorm::Worker
{
    struct Result
    {
        std::vector<U8> pixels;
        std::string     report;
        bool            ok = false;
    };
    std::mutex              mutex;
    std::condition_variable changed;
    std::thread             thread;
    bool                    stopping = false;
    std::string             pending;
    std::unique_ptr<Result> ready;
    explicit Worker(const std::string& path) : thread([this, path] { run(path); }) {}
    ~Worker()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = true;
        }
        changed.notify_all();
        if (thread.joinable())
            thread.join();
    }
    void run(const std::string& path)
    {
#if LL_DARWIN || LL_LINUX
        // Only an explicitly configured absolute local library is loaded. No search path.
        void*           library = nullptr;
        std::error_code ec;
        const auto      info = std::filesystem::symlink_status(path, ec);
        if (!ec && std::filesystem::path(path).is_absolute() && std::filesystem::is_regular_file(info))
            library = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);
        auto  abi     = library ? reinterpret_cast<decltype(&fs_desktop_abi)>(dlsym(library, "fs_desktop_abi")) : nullptr;
        auto  create  = library ? reinterpret_cast<decltype(&fs_desktop_create)>(dlsym(library, "fs_desktop_create")) : nullptr;
        auto  render  = library ? reinterpret_cast<decltype(&fs_desktop_render)>(dlsym(library, "fs_desktop_render")) : nullptr;
        auto  destroy = library ? reinterpret_cast<decltype(&fs_desktop_destroy)>(dlsym(library, "fs_desktop_destroy")) : nullptr;
        void* context = abi && create && render && destroy && abi() == 2 ? create(WIDTH, HEIGHT) : nullptr;
        if (!context)
        {
            auto result    = std::make_unique<Result>();
            result->report = "FinalStorm could not start. Check the explicitly configured local library and its architecture. The native "
                             "world view is available.";
            {
                std::lock_guard<std::mutex> lock(mutex);
                ready = std::move(result);
            }
            if (library)
                dlclose(library);
            return;
        }
        for (;;)
        {
            std::string input;
            {
                std::unique_lock<std::mutex> lock(mutex);
                changed.wait(lock, [this] { return stopping || !pending.empty(); });
                if (stopping)
                    break;
                input.swap(pending);
            }
            auto result = std::make_unique<Result>();
            result->pixels.resize(WIDTH * HEIGHT * 4);
            char report[4096] = {};
            result->ok        = render(context,
                                       reinterpret_cast<const uint8_t*>(input.data()),
                                       input.size(),
                                       result->pixels.data(),
                                       result->pixels.size(),
                                       report,
                                       sizeof(report)) == 0;
            result->report    = result->ok ? report : "FinalStorm refused this frame. The native world view remains available.";
            if (!result->ok && report[0])
                LL_WARNS_ONCE("FinalStormDesktop") << "Native frame validation: " << report << LL_ENDL;
            {
                std::lock_guard<std::mutex> lock(mutex);
                ready = std::move(result);
            }
        }
        destroy(context);
        dlclose(library);
#else
        auto result    = std::make_unique<Result>();
        result->report = "This embedded preview is currently supported on macOS and Linux source builds.";
        {
            std::lock_guard<std::mutex> lock(mutex);
            ready = std::move(result);
        }
#endif
    }
};
LLFloaterFinalverseStorm::LLFloaterFinalverseStorm(const LLSD& key) : LLFloater(key)
{
}
LLFloaterFinalverseStorm::~LLFloaterFinalverseStorm()
{ stop(); }
bool LLFloaterFinalverseStorm::postBuild()
{
    getChild<LLButton>("world")->setCommitCallback([this](LLUICtrl*, const LLSD&) { closeFloater(); });
    getChild<LLButton>("conversation")->setCommitCallback([](LLUICtrl*, const LLSD&) { LLFloaterReg::showInstance("finalverse_ai"); });
    return true;
}
void LLFloaterFinalverseStorm::stop()
{
    mImage = nullptr;
    mWorker.reset();
    mActor.setNull();
    mRegion.setNull();
    mSession.setNull();
}
void LLFloaterFinalverseStorm::onClose(bool)
{ stop(); }
void LLFloaterFinalverseStorm::onOpen(const LLSD&)
{ mRefresh.reset(); }
void LLFloaterFinalverseStorm::draw()
{
    auto       region = (gDisconnected || LLAppViewer::instance()->logoutRequestSent()) ? nullptr : gAgent.getRegion();
    const auto actor = gAgentID, session = gAgent.getSessionID(), regionID = region ? region->getRegionID() : LLUUID::null;
    if (actor.isNull() || !region || actor != mActor || regionID != mRegion || session != mSession)
    {
        stop();
        mActor           = actor;
        mRegion          = regionID;
        mSession         = session;
        const char* path = getenv("FINALSTORM_DESKTOP_LIBRARY");
        if (region && !actor.isNull() && path && *path)
        {
            mWorker = std::make_unique<Worker>(path);
            mRefresh.setTimerExpirySec(0);
        }
        getChild<LLTextBox>("status")->setText(
            std::string(!region   ? "Enter a world to view it with FinalStorm."
                        : mWorker ? "Starting FinalStorm with your current native session…"
                                  : "FinalStorm preview is not configured for this source build. Return to the native world view."));
    }
    if (isMinimized())
    {
        LLFloater::draw();
        return;
    }
    if (mWorker)
    {
        std::unique_ptr<Worker::Result> result;
        {
            std::lock_guard<std::mutex> lock(mWorker->mutex);
            result = std::move(mWorker->ready);
        }
        if (result)
        {
            if (result->ok)
            {
                LLPointer<LLImageRaw> raw = new LLImageRaw(TEX_WIDTH, TEX_HEIGHT, 4);
                {
                    LLImageDataLock image_lock(raw);
                    if (raw->isBufferInvalid())
                    {
                        mImage = nullptr;
                        return;
                    }
                    memset(raw->getData(), 0, TEX_WIDTH * TEX_HEIGHT * 4);
                    // wgpu readback is top-down; UI textures use a bottom-left origin.
                    for (int y = 0; y < HEIGHT; ++y)
                        memcpy(raw->getData() + y * TEX_WIDTH * 4, result->pixels.data() + (HEIGHT - 1 - y) * WIDTH * 4, WIDTH * 4);
                }
                if (mImage.isNull())
                    mImage = LLViewerTextureManager::getLocalTexture(raw.get(), false);
                else
                    mImage->setSubImage(raw.get(), 0, 0, TEX_WIDTH, TEX_HEIGHT);
                try
                {
                    auto report = LlsdFromJson(boost::json::parse(result->report));
                    getChild<LLTextBox>("status")->setText("FinalStorm · " + report["backend"].asString() + " · " +
                                                           report["objects"].asString() + " objects · " + report["triangles"].asString() +
                                                           " triangles · frame " + report["frame"].asString());
                    const auto frame = report["frame"].asInteger();
                    if (frame == 1 || frame % 50 == 0)
                        LL_INFOS("FinalStormDesktop") << result->report << LL_ENDL;
                }
                catch (...)
                {
                    mImage = nullptr;
                }
            }
            else
            {
                mImage = nullptr;
                getChild<LLTextBox>("status")->setText(result->report);
            }
        }
        if (mRefresh.getElapsedTimeF32() >= 0.2f)
        {
            auto input = captureNativeScene();
            if (input.empty())
                mImage = nullptr;
            else
            {
                std::lock_guard<std::mutex> lock(mWorker->mutex);
                mWorker->pending = std::move(input);
                mWorker->changed.notify_all();
            }
            mRefresh.reset();
        }
    }
    LLFloater::draw();
    if (!isMinimized() && mImage.notNull())
    {
        auto           rect = getChildView("viewport")->getRect();
        LLGLSUIDefault gls_ui;
        gGL.color4f(1, 1, 1, 1);
        gl_draw_scaled_image(rect.mLeft,
                             rect.mBottom,
                             rect.getWidth(),
                             rect.getHeight(),
                             mImage,
                             LLColor4::white,
                             LLRectf(0.f, F32(HEIGHT) / TEX_HEIGHT, F32(WIDTH) / TEX_WIDTH, 0.f));
    }
}
