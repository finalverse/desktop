// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfloaterfinalverseearth.h"
#include "llagent.h"
#include "llviewerregion.h"
#include "llbutton.h"
#include "lllineeditor.h"
#include "lltexteditor.h"
#include "llcoros.h"
#include "llcorehttputil.h"
#include "llsdutil.h"
#include "llfinalversepresentation.h"
#include "llappviewer.h"
#include <sstream>

bool LLFloaterFinalverseEarth::postBuild()
{
    for(const char* action:{"preview","commit","reject","inspect","undo","lumi"}) {
        const std::string name(action);
        getChild<LLButton>(name)->setCommitCallback([this,name](LLUICtrl*,const LLSD&){run(name);});
    }
    getChild<LLButton>("visit")->setCommitCallback([this](LLUICtrl*,const LLSD&){
        auto region=gAgent.getRegion();const auto position=mDestination["position"];
        if(!region||mDestination["region_id"].asString()!=region->getRegionID().asString()||!position.isArray()||position.size()!=3)return;
        gAgent.teleportViaLocation(region->getOriginGlobal()+LLVector3d(position[0].asReal(),position[1].asReal(),position[2].asReal()));
    });
    return true;
}
void LLFloaterFinalverseEarth::onOpen(const LLSD&)
{
    auto region=gAgent.getRegion();const LLUUID current=region?region->getRegionID():LLUUID::null;
    if(mOwner!=gAgentID||mRegion!=current) {
        mOwner=gAgentID;mRegion=current;mPlan.clear();mStatus.clear();mDestination=LLSD();
        getChild<LLLineEditor>("handoff")->setText(std::string());getChild<LLLineEditor>("plan_id")->setText(std::string());
    }
    if(mBusy)return;
    if(!mPlan.empty()) {
        run("inspect");
        return;
    }
    getChild<LLTextEditor>("review")->setText("Create a proposal in Earth Intelligence, then paste its single-use handoff here. Preview does not create objects. Review source evidence and permissions before approving.\n\nCurrent region: "+(region?region->getName():"Enter a world first")+"\nYour avatar ID for the handoff: "+gAgentID.asString()+"\n\nTo revisit an experience, enter its saved native plan ID and choose Inspect.");buttons();
}
void LLFloaterFinalverseEarth::buttons()
{
    getChild<LLButton>("preview")->setEnabled(!mBusy);
    getChild<LLButton>("inspect")->setEnabled(!mBusy);
    for(const char* action:{"commit","reject"})getChild<LLButton>(action)->setEnabled(!mBusy&&mStatus=="prepared");
    for(const char* action:{"undo","visit"})getChild<LLButton>(action)->setEnabled(!mBusy&&mStatus=="committed");
    getChild<LLButton>("lumi")->setEnabled(!mBusy&&!mPlan.empty()&&llfinalverseWorldServices().citizen_interactive);
}
void LLFloaterFinalverseEarth::run(const std::string& action)
{
    auto region=gAgent.getRegion();if(mBusy||!region)return;
    const std::string cap=region->getCapability("FinalverseWorld");
    if(cap.empty()){getChild<LLTextEditor>("review")->setText(std::string("Enter a MutSea world with Earth Bridge enabled first."));return;}
    LLSD body;body["op"]="earth_bridge";body["action"]=action;
    if(action=="preview") {
        body["handoff"]=getChild<LLLineEditor>("handoff")->getText();
        getChild<LLLineEditor>("handoff")->setText(std::string());
        mStatus.clear();mPlan.clear();
    } else {
        auto saved=getChild<LLLineEditor>("plan_id")->getText();
        if(action=="inspect"&&LLUUID::validate(saved))mPlan=saved;
        if(!LLUUID::validate(mPlan)){getChild<LLTextEditor>("review")->setText(std::string("Enter a valid native plan ID to inspect an existing experience."));return;}body["plan_id"]=mPlan;
    }
    if(action=="lumi") {body=LLSD().with("op","citizen").with("command","experience").with("text",mPlan);}
    const LLUUID actor=gAgentID,region_id=region->getRegionID();
    auto handle=getHandle();mBusy=true;buttons();
    getChild<LLTextEditor>("review")->setText(std::string(action=="preview"?"Validating the handoff, pinned assets, permissions and placement…":"Checking the authoritative world transaction…"));
    LLCoros::instance().launch("FinalverseEarthBridge",[handle,body,cap,actor,region_id,action]{
        LLCoreHttpUtil::HttpCoroutineAdapter adapter("FinalverseEarthBridge",LLCore::HttpRequest::DEFAULT_POLICY_ID);
        auto request=std::make_shared<LLCore::HttpRequest>();auto options=std::make_shared<LLCore::HttpOptions>();
        options->setTimeout(25);options->setRetries(0);
        auto reply=adapter.postJsonAndSuspend(request,cap,body,options);
        // LLTextEditor and floater controls acquire non-coroutine-aware locks.
        // Keep network suspension in this coroutine and apply every UI result
        // through the existing main-loop queue, including error/session paths.
        LLAppViewer::instance()->postToMainCoro([handle,reply,actor,region_id,cap,action]{
        llassert(LLCoros::on_main_coro());
        auto self=dynamic_cast<LLFloaterFinalverseEarth*>(handle.get());if(!self)return;
        self->mBusy=false;
        auto current=gAgent.getRegion();
        if(gAgentID!=actor||!current||current->getRegionID()!=region_id||current->getCapability("FinalverseWorld")!=cap){
            self->mStatus.clear();self->mPlan.clear();self->getChild<LLTextEditor>("review")->setText(std::string("Your world session changed. Reopen the experience in the destination region before taking another action."));self->buttons();return;
        }
        if(reply["ok"].isBoolean()&&reply["ok"].asBoolean()&&reply["result"].isMap()&&(action=="lumi"||reply["result"]["fsip"].isMap())) {
            if(action=="lumi")self->getChild<LLTextEditor>("review")->setText("Lumi · experience review\n\n"+reply["result"]["answer"].asString()+"\n\nThis conversation grants no world-edit permission. Inspect the proposal and approve separately.");
            else self->render(reply["result"]);
        }
        else {
            // An unconfirmed commit must not leave Apply enabled for a retry.
            if(action=="commit"||action=="undo"||action=="reject")self->mStatus="unknown";
            const std::string fallback=action=="preview"
                ?"Preview was not confirmed. No creation was authorized. Check the experience in Earth Intelligence and revoke its handoff before issuing another."
                :"No result was confirmed. Inspect the saved plan ID before taking another action.";
            self->getChild<LLTextEditor>("review")->setText(reply.has("error")?reply["error"].asString():fallback);
        }
        self->buttons();
        });
    });
}
void LLFloaterFinalverseEarth::render(const LLSD& result)
{
    const LLSD f=result["fsip"];if(!f.isMap())return;
    mPlan=result["plan_id"].asString();mStatus=f["status"].asString();mDestination=f["destination"];
    getChild<LLLineEditor>("plan_id")->setText(mPlan);
    std::ostringstream text;
    text<<f["title"].asString()<<"\n"<<(f["layout"].asString()=="virtual"?"Virtual world · no geographic binding":"Schematic exhibition · archival source evidence")<<"\n\n";
    text<<f["context"]["description"].asString()<<"\n\n";
    if(f["source_workspace"].isMap())text<<"Source workspace: "<<f["source_workspace"]["title"].asString()<<"\nArtifact: "<<f["source_workspace"]["artifact_id"].asString()<<"\n\n";
    text<<"AI proposal\n"<<f["narrative"].asString()<<"\n\nPermissions: create your own approved place; inspect and undo it. Native parcel permissions still apply. No scripts.\n\n";
    if(result.has("summary"))text<<result["summary"].asString()<<"\n"<<result["siting"].asString()<<"\n\n";
    for(const auto& p:llsd::inArray(f["provenance"]))text<<(p["generated"].asBoolean()?"Generated geometry: ":"Source records: ")<<p["attribution"].asString()<<"\nLicense: "<<p["license"].asString()<<"\n"<<p["source"].asString()<<"\n\n";
    for(const auto& exhibit:llsd::inArray(f["exhibits"]))text<<exhibit["description"].asString()<<"\n\n";
    text<<"Status: "<<mStatus<<"\nManifest: "<<f["manifest_sha256"].asString()<<"\n";
    if(mStatus=="prepared")text<<"Approval expires: "<<f["expires_at"].asString()<<"\n";
    if(result["receipt"].isMap())text<<"WorldLine: "<<result["receipt"]["worldline_operation_id"].asString()<<"\nBound exhibits/assets: "<<result["receipt"]["bindings"].size()<<"\nEarth receipt: "<<result["receipt_sync"].asString()<<"\n";
    getChild<LLTextEditor>("review")->setText(text.str());
}
