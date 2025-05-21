#include "BaseItemData.hpp"
#include "ItemDirector.hpp"
#include "ItemObj.hpp"
#include "gfx/GFXManager.hpp"

namespace Item {

// 0x8079d804 - 0x8079d848
void ItemObj::loadResources() {
        // TODO: Could be inlined loadResModelByName
        this->initDefaultRenderer();
}

// 0x8079d8bc - 0x8079d91c
ItemObj::ItemObj() {
    this->updateRes = 0;
    this->flags = NONE;
    this->flags2 = 0;
    this->curCollisionFlag = 0;
    this->colInfo.bboxLow.x = 0;
    this->colInfo.bboxLow.y = 0;
    this->colInfo.bboxLow.z = 0;
    this->colInfo.bboxHigh.x = 0;
    this->colInfo.bboxHigh.y = 0;
    this->colInfo.bboxHigh.z = 0;
    this->colInfo.drivableColInfo = NULL;
    this->landCollisionFlag = NONE;
    this->shadowModel = NULL;
    this->mainModel = NULL;
    this->lightModel = NULL;
    this->netIdentifier = -1;
}

extern "C" void fn_1_27799C(void* p);

// 0x8079d920 - 0x8079d9f8
void ItemObj::modelSetVisible(bool visible, bool disable) {
    if (this->mainModel) {
        this->mainModel->setEnabled(visible);
    }
    if (this->shadowModel) {
        this->shadowModel->setEnabled(visible);
    }
    if (this->lightModel) {
        bool lightEnabled = this->flags & LIGHT_RING_ENABLED;
        this->lightModel->setEnabled(!lightEnabled);
    }
    if (visible) {
        fn_1_27799C(this->renderer);
    } else if (disable) {
        this->renderer->drawFlags.uint |= 0x21212121;
    }
}

// 0x8079d9fc - 0x8079da98
void ItemObj::modelEnable() {
    if (this->mainModel) {
        this->mainModel->setEnabled(true);
    }
    if (this->shadowModel) {
        this->shadowModel->setEnabled(true);
    }
    if (this->lightModel) {
        bool lightEnabled = this->flags & LIGHT_RING_ENABLED;
        this->lightModel->setEnabled(!lightEnabled);
    }
    fn_1_27799C(this->renderer);
}

void fn_1_5527C();
extern u8 RelItemInfoArr[15][0x74];
extern u8* FUN_80591434();
extern "C" void Panic(...);
extern "C" char fileName;
extern "C" char nw4rException;

// 0x8079da9c - 0x8079dc40
void ItemObj::onActivate() {
    bool special = this->itemId == ITEMOBJ_BANANA && isDKJP;
    if (special) {
        this->lightSet = NULL;
        if (this->mainModel) {
            this->mainModel->setLightSetIdx(6);
        }
        return;
    } 
    
    ItemType item = this->itemId;
    if (ItemData::table[item].useLight) {
        GFX::LightManagerHolder *lightManagerHolder = GFX::gfxManager[0]->lightManagerHolder;
        s16 errorCode = -12;
        if ((u32)lightManagerHolder + 12 <= 0) {
            Panic(&fileName, 0xcf, &nw4rException);
        }
        s32 listLen = lightManagerHolder->inactiveLights.count;
        if (listLen > 0) {
            GFX::LightSet *lightSet = GFX::GFXManager::acquireLightSet();
            this->setLightSet(lightSet);
            bool isDrop = this->flags & FROM_DROP;
            if (!isDrop & ~FROM_DROP) {
                ItemDirector *director = ItemDirector::spInstance;
                Kart::KartObjectProxy *kart = &director->playerList[this->ownerId];
                GFX::LightSet *res = (GFX::LightSet*)kart->kartAccessor_48();
                this->lightSet->copyFrom(res);
            } else {
                this->lightSet->init(0, true);
            } 
        } else {
            this->lightSet = nullptr;
            if (this->mainModel) {
                u8 lightSetIdx;
                bool isDrop = this->flags & FROM_DROP;
                if (!isDrop & ~FROM_DROP) {
                    ItemDirector *director = ItemDirector::spInstance;
                    Kart::KartObjectProxy *kart = &director->playerList[this->ownerId];
                    GFX::LightSet *res = (GFX::LightSet*)kart->kartAccessor_48();
                    lightSetIdx = res->index;
                } else {
                    lightSetIdx = 0;
                }
                this->mainModel->setLightSetIdx(lightSetIdx);
            }
        }
    }
}

// 0x8079dc44 - 0x8079dc5c
void ItemObj::setLightSet(GFX::LightSet *lightSet) {
    this->lightSet = lightSet;
    if (!this->mainModel) {
        return;
    }
    this->mainModel->setLightSet(lightSet);
}

// 0x8079dc60 - 0x8079dcb0
void ItemObj::setLightSetIdx(u8 lightSetIdx) {
    bool special = this->itemId == ITEMOBJ_BANANA && isDKJP;
    if (special) {
      return;
    }

    if (this->lightSet) {
      this->lightSet->index = lightSetIdx;
    } else if (this->mainModel) {
      this->mainModel->setLightSetIdx(lightSetIdx);
    }
}

// 0x8079dcb4 - 0x8079dcb8
bool ItemObj::onUse() {
  return true;
}

// 0x8079dcbc - 0x8079dd68
ItemHitResult ItemObj::onHit() {
  if (this->mainModel) {
      this->mainModel->setEnabled(false);
  }
  if (this->shadowModel) {
      this->shadowModel->setEnabled(false);
  }
  if (this->lightModel) {
      bool lightEnabled = this->flags & LIGHT_RING_ENABLED;
      this->lightModel->setEnabled(!lightEnabled);
  }

  u32 drawFlags = this->renderer->drawFlags.uint;
  this->renderer->drawFlags.uint = drawFlags | 0x21212121;

  return HIT_BREAK;
}

// 0x8079dd6c - 0x8079dd6c
void ItemObj::onDrop() {
  return;
}

// 0x8079dd70 - 0x8079de30
void ItemObj::onDespawn() {
  if (this->mainModel) {
      this->mainModel->setEnabled(false);
  }
  if (this->shadowModel) {
      this->shadowModel->setEnabled(false);
  }
  if (this->lightModel) {
      bool lightEnabled = this->flags & LIGHT_RING_ENABLED;
      this->lightModel->setEnabled(!lightEnabled);
  }

  u32 drawFlags = this->renderer->drawFlags.uint;
  this->renderer->drawFlags.uint = drawFlags | 0x21212121;

  if (this->lightSet) {
    GFX::GFXManager::releaseLightSet(this->lightSet);
    this->lightSet = NULL;
  }
}

// 0x8079de34 - 0x8079dee0
void ItemObj::loadFromRecvEvent(ItemType itemType, ItemRKNetEventItem *queueItem, bool isTrailHit, bool isDrop, u8 ownerId) {
        
}

// 0x8079dee4 - 0x8079e1ec
void ItemObj::onOnlineShot() {

}

// 0x8079e1f0 - 0x8079e220
void ItemObj::onOnlineDrop() {

}

//-------------------------------
//Placeholders:
//-------------------------------

// Inlined, logically exists before 0x807a0040
void ItemObj::initDefaultRenderer() {
        this->initRenderer(0, "wii_controller", 0, 0, 0, 0, 0, 0);
}

}
