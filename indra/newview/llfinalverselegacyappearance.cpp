// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2010, Linden Research, Inc. (inherited readback/protocol code)
// RGBHM compositor readback and AgentSetAppearance follow inherited Linden
// viewer code before d58e7cfbfc (2013-09-19). Existing compositor/protocol remain
// the implementation; this adapter restores the explicitly advertised mode.
#include "llviewerprecompiledheaders.h"
#include "llfinalverselegacyappearance.h"
#include "llagent.h"
#include "llagentwearables.h"
#include "llcoros.h"
#include "llcorehttputil.h"
#include "llimagej2c.h"
#include "llmd5.h"
#include "llviewerregion.h"
#include "llviewernetwork.h"
#include "llviewertexlayer.h"
#include "llvoavatarself.h"
#include "llviewertexturelist.h"
#include <map>
#include <vector>
using namespace LLAvatarAppearanceDefines;
namespace {
LLUUID bake_session;
std::map<U8, U64> generations;
struct CompletedBake { LLUUID asset; LLUUID checksum; U64 generation; };
std::map<U8, CompletedBake> completed;
U32 serial = 0;
void checkSession()
{
    if (bake_session != gAgent.getSessionID())
    { bake_session = gAgent.getSessionID(); generations.clear(); completed.clear(); serial = 0; }
}
void publish()
{
    if (!llfinalverseLegacyAppearanceEnabled() || !isAgentAvatarValid()) return;
    for (auto te : {TEX_HEAD_BAKED, TEX_UPPER_BAKED, TEX_LOWER_BAKED})
    {
        auto bake = completed.find(te);
        if (bake == completed.end() || bake->second.generation != generations[te]) return;
    }
    LLMessageSystem* msg = gMessageSystem;
    msg->newMessageFast(_PREHASH_AgentSetAppearance);
    msg->nextBlockFast(_PREHASH_AgentData);
    msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
    msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
    msg->addVector3Fast(_PREHASH_Size, gAgentAvatarp->mBodySize + gAgentAvatarp->mAvatarOffset);
    msg->addU32Fast(_PREHASH_SerialNum, ++serial);
    for (const auto& [te, bake] : completed)
    {
        if (bake.generation != generations[te]) continue;
        msg->nextBlockFast(_PREHASH_WearableData);
        msg->addUUIDFast(_PREHASH_CacheID, bake.checksum);
        msg->addU8Fast(_PREHASH_TextureIndex, te);
    }
    msg->nextBlockFast(_PREHASH_ObjectData);
    gAgentAvatarp->sendAppearanceMessage(msg);
    for (LLVisualParam* param = gAgentAvatarp->getFirstVisualParam(); param;
         param = gAgentAvatarp->getNextVisualParam())
    {
        if (param->getGroup() == VISUAL_PARAM_GROUP_TWEAKABLE ||
            param->getGroup() == VISUAL_PARAM_GROUP_TRANSMIT_NOT_TWEAKABLE)
        {
            msg->nextBlockFast(_PREHASH_VisualParam);
            msg->addU8Fast(_PREHASH_ParamValue,
                F32_to_U8(param->getWeight(), param->getMinWeight(), param->getMaxWeight()));
        }
    }
    gAgent.sendReliableMessage();
    LL_INFOS("Avatar") << "Published client-baked appearance" << LL_ENDL;
}
}
bool llfinalverseLegacyAppearanceEnabled()
{
    auto region = gAgent.getRegion();
    return llfinalverseClientBakingRequired(region != nullptr,
        !region || region->getCentralBakeVersion() != 0,
        LLGridManager::getInstance()->isSystemGrid());
}
void llfinalverseStartLegacyAppearance()
{
    if (!llfinalverseLegacyAppearanceEnabled() || !isAgentAvatarValid()) return;
    checkSession();
    gAgentAvatarp->enableClientBaking();
    // BakeVersion visual parameter: this region advertises client baking.
    gAgentAvatarp->setVisualParamWeight(11000, 0.f);
    gAgentAvatarp->setCompositeUpdatesEnabled(true);
    gAgentAvatarp->invalidateAll();
    gAgentAvatarp->updateMeshTextures();
    // Publish real inventory item IDs in this protocol mode; the inherited
    // system-grid compatibility message intentionally contains dummy IDs.
    gAgentWearables.sendDummyAgentWearablesUpdate();
    LL_INFOS("Avatar") << "Using advertised client-baked appearance mode" << LL_ENDL;
}
void llfinalverseInvalidateLegacyBake(U8 te)
{
    checkSession();
    ++generations[te];
}
void llfinalverseUploadLegacyBake(LLViewerTexLayerSet* layer, S32 x, S32 y, S32 width, S32 height,
                                LLRenderTarget* bound_target)
{
    if (!llfinalverseLegacyAppearanceEnabled() || !isAgentAvatarValid() ||
        !layer || !layer->isLocalTextureDataFinal() || gAgentAvatarp->isEditingAppearance() ||
        width <= 0 || height <= 0 || width > 2048 || height > 2048) return;
    const U8 te = gAgentAvatarp->getBakedTE(layer);
    checkSession();
    if (!layer->isVisible())
    {
        // Inherited rule (LLViewerTexLayerSetBuffer::doUpload before d58e7cfbfc): a layer set
        // fully hidden by an alpha wearable is not uploaded. Its baked slot is IMG_INVISIBLE,
        // so neither this viewer nor observers draw the system mesh (for example the hair).
        static const LLUUID invisible_cache_id("6d2f1d6c-8f7e-4b1a-9c0e-2a4b6e8f0a11");
        gAgentAvatarp->setTETexture(te, IMG_INVISIBLE);
        completed[te] = {IMG_INVISIBLE, invisible_cache_id, generations[te]};
        publish();
        return;
    }
    const std::string capability = gAgent.getRegion()->getCapability("UploadBakedTexture");
    if (capability.empty()) return;
    const U64 generation = generations[te];
    const LLUUID session = bake_session, region_id = gAgent.getRegion()->getRegionID();
    std::vector<U8> color(width * height * 4);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, color.data());
    LLGLSUIDefault gls_ui;
    LLPointer<LLImageRaw> mask = new LLImageRaw(width, height, 1);
    layer->gatherMorphMaskAlpha(mask->getData(), x, y, width, height, bound_target);
    LLPointer<LLImageRaw> raw = new LLImageRaw(width, height, 5);
    for (S32 pixel = 0; pixel < width * height; ++pixel)
    {
        std::copy_n(color.data() + pixel * 4, 4, raw->getData() + pixel * 5);
        raw->getData()[pixel * 5 + 4] = mask->getData()[pixel];
    }
    LLPointer<LLImageJ2C> image = new LLImageJ2C;
    if (!image->encode(raw, "LL_RGBHM"))
    { LL_WARNS("Avatar") << "Client bake encoding failed" << LL_ENDL; return; }
    auto bytes = std::make_shared<std::vector<U8>>(image->getData(), image->getData() + image->getDataSize());
    LLUUID checksum;
    LLMD5 digest;
    digest.update(bytes->data(), bytes->size());
    digest.finalize();
    digest.raw_digest(checksum.mData);
    LLCoros::instance().launch("FinalverseLegacyBake", [capability, bytes, te, generation, session, region_id, checksum]()
    {
        LLCoreHttpUtil::HttpCoroutineAdapter adapter("FinalverseLegacyBake", LLCore::HttpRequest::DEFAULT_POLICY_ID);
        auto request = std::make_shared<LLCore::HttpRequest>();
        auto options = std::make_shared<LLCore::HttpOptions>();
        options->setTimeout(30);
        LLSD reply = adapter.postAndSuspend(request, capability, LLSD::emptyMap(), options);
        const std::string uploader = reply["uploader"].asString();
        if (reply["state"].asString() != "upload" || uploader.empty())
        { LL_WARNS("Avatar") << "Client bake upload handshake failed" << LL_ENDL; return; }
        LLCore::BufferArray::ptr_t body(new LLCore::BufferArray);
        body->append(bytes->data(), bytes->size());
        auto headers = std::make_shared<LLCore::HttpHeaders>();
        headers->append(HTTP_OUT_HEADER_CONTENT_TYPE, "application/octet-stream");
        reply = adapter.postAndSuspend(request, uploader, body, options, headers);
        const LLUUID asset = reply["new_asset"].asUUID();
        if (reply["state"].asString() != "complete" || asset.isNull())
        { LL_WARNS("Avatar") << "Client bake upload failed" << LL_ENDL; return; }
        if (!llfinalverseLegacyAppearanceEnabled() || !isAgentAvatarValid() ||
            gAgent.getSessionID() != session || gAgent.getRegion()->getRegionID() != region_id ||
            generations[te] != generation) return;
        gAgentAvatarp->setTETexture(te, asset);
        completed[te] = {asset, checksum, generation};
        publish();
    });
}
