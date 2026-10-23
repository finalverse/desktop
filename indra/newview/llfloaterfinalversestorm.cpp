// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalversestorm.h"
#include "finalstorm/finalstorm_desktop.h"
#include "llagent.h"
#include "llappviewer.h"
#include "llbutton.h"
#include "llfloaterreg.h"
#include "llimage.h"
#include "llsdjson.h"
#include "lltextbox.h"
#include "llviewercamera.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llvolume.h"
#include "lltextureentry.h"
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>
#if LL_DARWIN || LL_LINUX
#include <dlfcn.h>
#endif

namespace {
constexpr int WIDTH=640, HEIGHT=360, TEX_WIDTH=1024, TEX_HEIGHT=512;
LLSD vector(const F32* v, int count) { LLSD a=LLSD::emptyArray();for(int i=0;i<count;++i)a.append(F64(v[i]));return a; }
// Called only on the UI thread: copy data already decoded by the native viewer.
// No capability, session secret, texture URL or world write crosses this boundary.
std::string captureNativeScene()
{
    auto region=gAgent.getRegion();if(!region)return {};
    LLSD root;root["schema"]="finalverse.desktop-scene/v1";root["actor"]=gAgentID.asString();root["region"]=region->getRegionID().asString();
    auto camera=LLViewerCamera::getInstance();
    auto global=gAgent.getPosGlobalFromAgent(camera->getOrigin())-region->getOriginGlobal();
    LLSD p=LLSD::emptyArray();for(int i=0;i<3;++i)p.append(global.mdV[i]);
    root["camera"]["position"]=p;root["camera"]["forward"]=vector(camera->getAtAxis().mV,3);
    root["camera"]["up"]=vector(camera->getUpAxis().mV,3);root["camera"]["fov"]=camera->getView();
    root["objects"]=LLSD::emptyArray();int vertices=0,indices=0,faces=0;
    for(int i=0;i<gObjectList.getNumObjects()&&root["objects"].size()<256;++i)
    {
        auto object=gObjectList.getObject(i);
        if(!object||object->isDead()||object->getRegion()!=region||object->isAttachment()||object->isAvatar())continue;
        auto volume=object->getVolume();if(!volume)continue;
        // The viewer resolves child/linkset transforms into region coordinates.
        LLSD item;item["id"]=object->getID().asString();item["position"]=vector(object->getPositionRegion().mV,3);
        const auto rotation=object->getRotationRegion();item["rotation"]=vector(rotation.mQ,4);item["scale"]=vector(object->getScale().mV,3);
        item["faces"]=LLSD::emptyArray();
        for(int f=0;f<volume->getNumVolumeFaces()&&f<64;++f)
        {
            const auto& face=volume->getVolumeFace(f);
            if(face.mNumVertices<3||face.mNumIndices<3||!face.mPositions||!face.mNormals||!face.mIndices
                ||vertices+face.mNumVertices>65536||indices+face.mNumIndices>196608||faces>=1024)continue;
            LLSD out;out["positions"]=LLSD::emptyArray();out["normals"]=LLSD::emptyArray();out["uvs"]=LLSD::emptyArray();out["indices"]=LLSD::emptyArray();
            for(int v=0;v<face.mNumVertices;++v) {
                out["positions"].append(vector(face.mPositions[v].getF32ptr(),3));out["normals"].append(vector(face.mNormals[v].getF32ptr(),3));
                if(face.mTexCoords)out["uvs"].append(vector(face.mTexCoords[v].mV,2));
            }
            for(int v=0;v<face.mNumIndices;++v)out["indices"].append(int(face.mIndices[v]));
            auto te=object->getTE(f);const LLColor4 color=te?te->getColor():LLColor4::white;out["color"]=vector(color.mV,4);
            vertices+=face.mNumVertices;indices+=face.mNumIndices;++faces;item["faces"].append(out);
        }
        if(item["faces"].size())root["objects"].append(item);
    }
    auto json=boost::json::serialize(LlsdToJson(root));return json.size()<=8*1024*1024?json:std::string();
}
}

struct LLFloaterFinalverseStorm::Worker
{
    struct Result {std::vector<U8> pixels;std::string report;bool ok=false;};
    std::mutex mutex;std::condition_variable changed;std::thread thread;bool stopping=false;
    std::string pending;std::unique_ptr<Result> ready;
    explicit Worker(const std::string& path) : thread([this,path] { run(path); }) {}
    ~Worker() { {std::lock_guard<std::mutex> lock(mutex);stopping=true;}changed.notify_all();if(thread.joinable())thread.join(); }
    void run(const std::string& path)
    {
#if LL_DARWIN || LL_LINUX
        // Only an explicitly configured absolute local library is loaded. No search path.
        void* library=nullptr;
        std::error_code ec;const auto info=std::filesystem::symlink_status(path,ec);
        if(!ec&&std::filesystem::path(path).is_absolute()&&std::filesystem::is_regular_file(info))library=dlopen(path.c_str(),RTLD_LOCAL|RTLD_NOW);
        auto abi=library?reinterpret_cast<decltype(&fs_desktop_abi)>(dlsym(library,"fs_desktop_abi")):nullptr;
        auto create=library?reinterpret_cast<decltype(&fs_desktop_create)>(dlsym(library,"fs_desktop_create")):nullptr;
        auto render=library?reinterpret_cast<decltype(&fs_desktop_render)>(dlsym(library,"fs_desktop_render")):nullptr;
        auto destroy=library?reinterpret_cast<decltype(&fs_desktop_destroy)>(dlsym(library,"fs_desktop_destroy")):nullptr;
        void* context=abi&&create&&render&&destroy&&abi()==1?create(WIDTH,HEIGHT):nullptr;
        if(!context) {
            auto result=std::make_unique<Result>();result->report="FinalStorm could not start. Check the explicitly configured local library and its architecture. The native world view is available.";
            {std::lock_guard<std::mutex> lock(mutex);ready=std::move(result);}if(library)dlclose(library);return;
        }
        for(;;) {
            std::string input;
            {std::unique_lock<std::mutex> lock(mutex);changed.wait(lock,[this]{return stopping||!pending.empty();});if(stopping)break;input.swap(pending);}
            auto result=std::make_unique<Result>();result->pixels.resize(WIDTH*HEIGHT*4);char report[4096]={};
            result->ok=render(context,reinterpret_cast<const uint8_t*>(input.data()),input.size(),result->pixels.data(),result->pixels.size(),report,sizeof(report))==0;
            result->report=result->ok?report:"FinalStorm refused this frame. The native world view remains available.";
            {std::lock_guard<std::mutex> lock(mutex);ready=std::move(result);}
        }
        destroy(context);dlclose(library);
#else
        auto result=std::make_unique<Result>();result->report="This embedded preview is currently supported on macOS and Linux source builds.";
        {std::lock_guard<std::mutex> lock(mutex);ready=std::move(result);}
#endif
    }
};
LLFloaterFinalverseStorm::LLFloaterFinalverseStorm(const LLSD& key):LLFloater(key) {}
LLFloaterFinalverseStorm::~LLFloaterFinalverseStorm() {stop();}
bool LLFloaterFinalverseStorm::postBuild()
{
    getChild<LLButton>("world")->setCommitCallback([this](LLUICtrl*,const LLSD&){closeFloater();});
    getChild<LLButton>("conversation")->setCommitCallback([](LLUICtrl*,const LLSD&){LLFloaterReg::showInstance("finalverse_ai");});
    return true;
}
void LLFloaterFinalverseStorm::stop() {mImage=nullptr;mWorker.reset();mActor.setNull();mRegion.setNull();mSession.setNull();}
void LLFloaterFinalverseStorm::onClose(bool) {stop();}
void LLFloaterFinalverseStorm::onOpen(const LLSD&) {mRefresh.reset();}
void LLFloaterFinalverseStorm::draw()
{
    auto region=(gDisconnected||LLAppViewer::instance()->logoutRequestSent())?nullptr:gAgent.getRegion();
    const auto actor=gAgentID,session=gAgent.getSessionID(),regionID=region?region->getRegionID():LLUUID::null;
    if(actor.isNull()||!region||actor!=mActor||regionID!=mRegion||session!=mSession) {
        stop();mActor=actor;mRegion=regionID;mSession=session;
        const char* path=getenv("FINALSTORM_DESKTOP_LIBRARY");
        if(region&&!actor.isNull()&&path&&*path) {mWorker=std::make_unique<Worker>(path);mRefresh.setTimerExpirySec(0);}
        getChild<LLTextBox>("status")->setText(std::string(!region?"Enter a world to view it with FinalStorm.":mWorker?"Starting FinalStorm with your current native session…":"FinalStorm preview is not configured for this source build. Return to the native world view."));
    }
    if(isMinimized()) {LLFloater::draw();return;}
    if(mWorker) {
        std::unique_ptr<Worker::Result> result;
        {std::lock_guard<std::mutex> lock(mWorker->mutex);result=std::move(mWorker->ready);}
        if(result) {
            if(result->ok) {
                LLPointer<LLImageRaw> raw=new LLImageRaw(TEX_WIDTH,TEX_HEIGHT,4);
                LLImageDataLock image_lock(raw);
                if(raw->isBufferInvalid()) {mImage=nullptr;return;}
                memset(raw->getData(),0,TEX_WIDTH*TEX_HEIGHT*4);
                // wgpu readback is top-down; UI textures use a bottom-left origin.
                for(int y=0;y<HEIGHT;++y)memcpy(raw->getData()+y*TEX_WIDTH*4,result->pixels.data()+(HEIGHT-1-y)*WIDTH*4,WIDTH*4);
                if(mImage.isNull())mImage=LLViewerTextureManager::getLocalTexture(raw.get(),false);
                else mImage->setSubImage(raw.get(),0,0,TEX_WIDTH,TEX_HEIGHT);
                try {
                    auto report=LlsdFromJson(boost::json::parse(result->report));
                    getChild<LLTextBox>("status")->setText("FinalStorm · "+report["backend"].asString()+" · "+report["objects"].asString()+" objects · "+report["triangles"].asString()+" triangles · frame "+report["frame"].asString());
                    LL_INFOS("FinalStormDesktop")<<result->report<<LL_ENDL;
                } catch(...) {mImage=nullptr;}
            } else {mImage=nullptr;getChild<LLTextBox>("status")->setText(result->report);}
        }
        if(mRefresh.getElapsedTimeF32()>=0.2f) {
            auto input=captureNativeScene();
            if(input.empty())mImage=nullptr;
            else {std::lock_guard<std::mutex> lock(mWorker->mutex);mWorker->pending=std::move(input);mWorker->changed.notify_all();}
            mRefresh.reset();
        }
    }
    LLFloater::draw();
    if(!isMinimized()&&mImage.notNull()) {
        auto rect=getChildView("viewport")->getRect();LLGLSUIDefault gls_ui;
        gGL.color4f(1,1,1,1);gl_draw_scaled_image(rect.mLeft,rect.mBottom,rect.getWidth(),rect.getHeight(),mImage,LLColor4::white,LLRectf(0.f,F32(HEIGHT)/TEX_HEIGHT,F32(WIDTH)/TEX_WIDTH,0.f));
    }
}
