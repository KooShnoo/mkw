#include <nw4r/math/mathTypes.hpp>

#include "ItemObj.hpp"
#include "BaseItemData.hpp"
#include "ItemDirector.hpp"
#include "egg/math/eggVector.hpp"
#include "geo/BoxColManager.hpp"
#include "gfx/GFXManager.hpp"
#include "kart/KartMove.hpp"
#include "kart/KartObject.hpp"
#include "kart/KartObjectManager.hpp"
#include "kart/KartObjectProxy.hpp"
#include "rk_types.h"

namespace Item {

const f32 MAX_ITEM_SPEED = 100.0f;

// 0x8079d804 - 0x8079d848
void ItemObj::loadResources() {
    // TODO: Could be inlined loadResModelByName
    initRenderer(0, "wii_controller", 0, 0, 0, 0, 0, 0);
}

// 0x8079d8bc - 0x8079d91c
ItemObj::ItemObj() {
    mUpdateRes = 0;
    mFlags = NONE;
    mFlags2 = 0;
    mCurCollisionFlag = 0;
    mColInfo.bboxLow.x = 0;
    mColInfo.bboxLow.y = 0;
    mColInfo.bboxLow.z = 0;
    mColInfo.bboxHigh.x = 0;
    mColInfo.bboxHigh.y = 0;
    mColInfo.bboxHigh.z = 0;
    mColInfo.drivableColInfo = NULL;
    mLandCollisionFlag = NONE;
    mShadowModel = NULL;
    mMainModel = NULL;
    mLightModel = NULL;
    mNetIdentifier = -1;
}

extern "C" void fn_1_27799C(void* p);

// 0x8079d920 - 0x8079d9f8
void ItemObj::modelSetVisible(bool visible, bool disable) {
    if (mMainModel) {
        mMainModel->setEnabled(visible);
    }
    if (mShadowModel) {
        mShadowModel->setEnabled(visible);
    }
    if (mLightModel) {
        bool lightEnabled = mFlags & LIGHT_RING_ENABLED;
        mLightModel->setEnabled(!lightEnabled);
    }
    if (visible) {
        fn_1_27799C(mRenderer);
    } else if (disable) {
        mRenderer->drawFlags.uint |= 0x21212121;
    }
}

// 0x8079d9fc - 0x8079da98
void ItemObj::modelEnable() {
    if (mMainModel) {
        mMainModel->setEnabled(true);
    }
    if (mShadowModel) {
        mShadowModel->setEnabled(true);
    }
    if (mLightModel) {
        bool lightEnabled = mFlags & LIGHT_RING_ENABLED;
        mLightModel->setEnabled(!lightEnabled);
    }
    fn_1_27799C(mRenderer);
}

void fn_1_5527C();
extern u8* FUN_80591434();
extern "C" void Panic(...);
extern "C" char fileName;
extern "C" char nw4rException;

// 0x8079da9c - 0x8079dc40
void ItemObj::onActivate() {
    bool special = mItemId == ITEMOBJ_BANANA && isDKJP;
    if (special) {
        mLightSet = NULL;
        if (mMainModel) {
            mMainModel->setLightSetIdx(6);
        }
        return;
    } 
    
    ItemType item = mItemId;
    if (ItemData::table[item].useLight) {
        GFX::LightManagerHolder *lightManagerHolder = GFX::gfxManager[0]->lightManagerHolder;
        s16 errorCode = -12;
        if ((u32)lightManagerHolder + 12 <= 0) {
            Panic(&fileName, 0xcf, &nw4rException);
        }
        s32 listLen = lightManagerHolder->inactiveLights.count;
        if (listLen > 0) {
            GFX::LightSet *lightSet = GFX::GFXManager::acquireLightSet();
            setLightSet(lightSet);
            bool isDrop = mFlags & FROM_DROP;
            if (!isDrop & ~FROM_DROP) {
                ItemDirector *director = ItemDirector::spInstance;
                Kart::KartObjectProxy *kart = &director->playerList[mOwnerId];
                GFX::LightSet *res = (GFX::LightSet*)kart->kartAccessor_48();
                mLightSet->copyFrom(res);
            } else {
                mLightSet->init(0, true);
            } 
        } else {
            mLightSet = nullptr;
            if (mMainModel) {
                u8 lightSetIdx;
                bool isDrop = mFlags & FROM_DROP;
                if (!isDrop & ~FROM_DROP) {
                    ItemDirector *director = ItemDirector::spInstance;
                    Kart::KartObjectProxy *kart = &director->playerList[mOwnerId];
                    GFX::LightSet *res = (GFX::LightSet*)kart->kartAccessor_48();
                    lightSetIdx = res->index;
                } else {
                    lightSetIdx = 0;
                }
                mMainModel->setLightSetIdx(lightSetIdx);
            }
        }
    }
}

// 0x8079dc44 - 0x8079dc5c
void ItemObj::setLightSet(GFX::LightSet *lightSet) {
    mLightSet = lightSet;
    if (!mMainModel) {
        return;
    }
    mMainModel->setLightSet(lightSet);
}

// 0x8079dc60 - 0x8079dcb0
void ItemObj::setLightSetIdx(u8 lightSetIdx) {
    bool special = mItemId == ITEMOBJ_BANANA && isDKJP;
    if (special) {
      return;
    }

    if (mLightSet) {
      mLightSet->index = lightSetIdx;
    } else if (mMainModel) {
      mMainModel->setLightSetIdx(lightSetIdx);
    }
}

// 0x8079dcb4 - 0x8079dcb8
bool ItemObj::onUse() {
  return true;
}

// 0x8079dcbc - 0x8079dd68
ItemHitResult ItemObj::onHit() {
  if (mMainModel) {
      mMainModel->setEnabled(false);
  }
  if (mShadowModel) {
      mShadowModel->setEnabled(false);
  }
  if (mLightModel) {
      bool lightEnabled = mFlags & LIGHT_RING_ENABLED;
      mLightModel->setEnabled(!lightEnabled);
  }

  u32 drawFlags = mRenderer->drawFlags.uint;
  mRenderer->drawFlags.uint = drawFlags | 0x21212121;

  return HIT_BREAK;
}

// 0x8079dd6c - 0x8079dd6c
void ItemObj::onDrop() {
  return;
}

// 0x8079dd70 - 0x8079de30
void ItemObj::onDespawn() {
  if (mMainModel) {
      mMainModel->setEnabled(false);
  }
  if (mShadowModel) {
      mShadowModel->setEnabled(false);
  }
  if (mLightModel) {
      bool lightEnabled = mFlags & LIGHT_RING_ENABLED;
      mLightModel->setEnabled(!lightEnabled);
  }

  u32 drawFlags = mRenderer->drawFlags.uint;
  mRenderer->drawFlags.uint = drawFlags | 0x21212121;

  if (mLightSet) {
    GFX::GFXManager::releaseLightSet(mLightSet);
    mLightSet = NULL;
  }
}

// 0x8079de34 - 0x8079dee0
void ItemObj::loadFromRecvEvent(ItemType itemType, ItemRKNetEventItem *queueItem, bool isTrailHit, bool isDrop, u8 ownerId) {
}

inline f32 min(f32 a, f32 b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

extern "C" void resetCollisionEntries(u32 &flag);
extern void * PlayerHolder;
extern "C" Kart::KartObjectProxy * PlayerHolder_getPlayer(void* playerHolder, u32 playerId);
extern "C" void QuatFromRowVec34(EGG::Quatf * quat, RowVec34 * mtx);
extern "C" void FUN_807bd7b4(Field::DrivableColInfo * colInfo);
extern "C" u32 FUN_807d2ddc(bool);
extern "C" BoxColUnit *  EntityHolder_insertItemObj(BoxColManager * manager, EGG::Vector3f * position, bool, ItemObj * item, f32 size, f32);

// 0x8079dee4 - 0x8079e1ec
void ItemObj::onOnlineShot() {
    (void) 1.0f;
    f32 scaleMax = 0.8f;
    f32 hitboxScale = ItemData::table[mItemId].hitboxScale;
    // if (scaleMax > hitboxScale) {
    //     scaleMax = hitboxScale;
    // }
    // mScaleFactor = scaleMax;
    mScaleFactor = min(scaleMax, ItemData::table[mItemId].hitboxScale);
    fixScale();

    mLastYRotation.x = 0.0f;
    mLastYRotation.y = 1.0f;
    mLastYRotation.z = 0.0f;

    resetCollisionEntries(mCurCollisionFlag);
    mColInfo.bboxLow.setZero();
    mColInfo.bboxHigh.setZero();
    mColInfo.movingFloorDist = -FLT_MIN;
    mColInfo.wallDist = -FLT_MIN;
    mColInfo.floorDist = -FLT_MIN;
    mColInfo.colPerpendicularity = 0.0f;
    mLandCollisionFlag = 0;

    Kart::KartObjectProxy * player = PlayerHolder_getPlayer(PlayerHolder, mOwnerId);
    EGG::Vector3f bodyForward;
    player->getBodyForward(bodyForward);
    mTransform.z = bodyForward;

    mTransform.y.setUp();

    if ((1 << mItemId & 0b00100011) == 0) {
        mTransform.mirrorZ();
    }
    mTransform.normaliseY();

    QuatFromRowVec34(&mQuaternion, &mTransform);
    mTargetYScale = 0;
    if (mDrivableColInfo) {
        FUN_807bd7b4(mDrivableColInfo);
    }

    mSomeCounter = 5;
    mVanishCountdown = 0;

    mActiveTime = FUN_807d2ddc((mFlags >> 0x19) & 1) - 1;

    UpdateFunc trailUpdateFunc = ItemData::table[mItemId].trailingObjUpdateFunc;
    if ((mFlags & (BROKEN | VANISHED)) == 0) {
        mUpdateFunc = trailUpdateFunc;
    }

    if (((mFlags & (LOCAL_BROKEN|LOCAL_VANISHED|LOCAL_DESPAWNED)) == 0) && 
        !ItemData::table[mItemId].disableBoxCol && 
        mBoxColEntity == NULL
    ) {
        registerBoxCol(true);
        mFlags2 |= 0x1000;
    }
}

// 0x8079e1f0 - 0x8079e220
void ItemObj::onOnlineDrop() {
}

// 0x8079e224 - 0x8079e2fc
void ItemObj::init(u16 id, u16 typeIndex, eItemType itemType) {
    mItemId = itemType;
    mId = id;
    mTypeIndex = typeIndex;
    loadResources();
    mUpdateRes = 0;
    mFlags = NONE;
    mFlags2 = 0;

    mSoundActor = new Sound::ItemSnd();
    mSoundActor->_vf0x118(this, mItemId);

    if (ItemData::table[mItemId].canLand || 
        itemType == ITEMOBJ_BANANA || 
        itemType == ITEMOBJ_FAKE_ITEM_BOX || 
        itemType == ITEMOBJ_BOMB
    ) {
        mColInfo.createDrivableColInfo();
    }
    mDrivableColInfo = mColInfo.drivableColInfo;
}

f32 ItemObj::hitboxSize(bool small) {
    f32 radius = small ? ItemData::table[mItemId].hitboxRadius : ItemData::table[mItemId].hitboxSize;
    f32 scale = ItemData::table[mItemId].hitboxScale;
    return scale * radius;
}

// 0x8079ec98 - 0x8079ed18
void ItemObj::scaleHitbox(bool useRadius) {
    f32 scale;
    if (useRadius) {
        u32 itemId = mItemId;
        scale = ItemData::table[mItemId].hitboxRadius;
    } else {
        u32 itemId = mItemId;
        scale = ItemData::table[mItemId].hitboxSize;
    }
    f32 height = ItemData::table[mItemId].hitboxHeight;
    
    mHitboxHeight = mScale.y * height;
    mHitboxRadius = mScale.z * scale;
    
    f32 drawDistBack = ItemData::table[*(volatile int*)&mItemId].drawDistBack;
    mRenderer->drawDistanceBack = mHitboxRadius * drawDistBack;
}

bool ItemObj::raiseScaleFactor(f32 arg1, f32 arg2) {
    if ((mScaleFactor < arg1)) {
        mScaleFactor += arg2;
        if (!(mScaleFactor < arg1)) {
            mScaleFactor = arg1;
        }
        return true;
    }
    return false;
}

// 0x8079ed4c - 0x8079edb0
void ItemObj::fixScale() {
    mUpdateRes |= 0x40;
    mScale.set(mScaleFactor);
    
    u32 itemId = mItemId;
    f32 scale = ItemData::table[mItemId].hitboxSize;
    f32 height = ItemData::table[mItemId].hitboxHeight;
    
    mHitboxHeight = mScale.y * height;
    mHitboxRadius = mScale.z * scale;
    
    f32 drawDistBack = ItemData::table[*(volatile int*)&mItemId].drawDistBack;
    mRenderer->drawDistanceBack = mHitboxRadius * drawDistBack;
}

// 0x8079edb4 - 0x8079ee2c
void ItemObj::setScale(EGG::Vector3f * scale) {
    VECMultiply(&mScale, scale, mScaleFactor);
    mUpdateRes = mUpdateRes | 0x40;
    f32 hitboxScale;
    u32 itemId = mItemId;
    hitboxScale = ItemData::table[mItemId].hitboxRadius;
    f32 height = ItemData::table[mItemId].hitboxHeight;
    
    mHitboxHeight = mScale.y * height;
    mHitboxRadius = mScale.z * hitboxScale;
    
    f32 drawDistBack = ItemData::table[*(volatile int*)&mItemId].drawDistBack;
    mRenderer->drawDistanceBack = mHitboxRadius * drawDistBack;
}

void ItemObj::registerBoxCol(bool small) {
    mBoxColEntity = BoxColManager::spInstance->insertItemObj(
        /* radius */ hitboxSize(small),
        // /* maxSpeed */ 100.0f,
        /* maxSpeed */ MAX_ITEM_SPEED,
        /* pos */ &mTransform.t,
        /* meep */ true,
        /* item */ this
    );
}

void ItemObj::deregisterBoxCol() {
    BoxColManager::spInstance->remove(&mBoxColEntity);
}


//-------------------------------
//Placeholders:
//-------------------------------

// void ItemObj::initDefaultRenderer() {
// }


// TODO: migrate to egg TBitFlag or rkbitfield or another thing
#define ON_BIT(bitfield, mask) ((bitfield & mask) == 0)


void ItemObj::Spawn(ItemType objId, u8 playerId, const EGG::Vector3f &position, bool r7) {
    mTransform.t = position;
    mOwnerId = playerId;
    mTrailOwnerId = MAX_PLAYER_COUNT;
    SetId(objId);
    draw_8079e38c();
    if (!ON_BIT(mFlags, 0x1000000)) {
        drawTransform_807a07b8(0);
    } else {
        drawTransform_807a0cd4(0, 0.0f);
    }
    mNetIdentifier = spawnItemSomething_8079acc0(r7);
}

// forard decls
void NormalizeVector(EGG::Vector3f &out, const EGG::Vector3f &src);
void MultiplyVectorByDotProduct_LLMname(const EGG::Vector3f &lhs, const EGG::Vector3f &rhs, EGG::Vector3f &out);
void Eff_greenShel_FUN_8068d4f8(f64, void *, const EGG::Vector3f &effectPos);
void Eff_redShel_FUN_8068d570(f64, void *, const EGG::Vector3f &effectPos);

struct ObjectCollisionItem {
public:
    struct {
        u32 vt;
        EGG::Vector3f m_translation;
        EGG::Vector3f _0c;
    } base;
    f32 radius;
    EGG::Vector3f itemPos;

    static ObjectCollisionItem *spInstance;
};
static_assert(sizeof(ObjectCollisionItem) == 0x2c);
struct EffectInfo {
    u8 _0[0x74 - 0x00];
    void *_74;
    static EffectInfo* spInstance; // = (EffectInfo*)0x809c21d0;
};
bool checkPrimitiveCollision(Kart::KartAccessor_34* thisptr, ObjectCollisionItem &prim);
// fro debugging
#pragma sym on

// wip
void ItemObj::checkOnlineTargetPlayerCollision() {
    Kart::KartObject *kart = Kart::KartObjectManager::sInstance->getObject(mOnlineTarget);
    f32 boundingRadius = mHitboxRadius + kart->getBoundingRadius();
    f32 kartDist = PSVECSquareDistance(mTransform.t, kart->pos());
    bool isCollide = kartDist < boundingRadius * boundingRadius;
    if (isCollide) {
        ObjectCollisionItem::spInstance->itemPos = mTransform.t;
        ObjectCollisionItem::spInstance->radius = mHitboxRadius;
        // isCollide = kart->objectCollisionKart()->checkPrimitiveCollision(*ObjectCollisionItem::spInstance);
        isCollide = checkPrimitiveCollision(kart->kartAccessor_34(), *ObjectCollisionItem::spInstance);
    }
    if (!isCollide) {
        const EGG::Vector3f &kartPos = kart->pos();
        const EGG::Vector3f &kartPosOld = kart->oldPos();
        f32 distNew = PSVECSquareDistance(kartPos, mTransform.t);
        f32 distOld = PSVECSquareDistance(kartPosOld, mLastPosition);
        if (distNew < distOld) {
            EGG::Vector3f local_38 = kartPos - kartPosOld;
            // todo: we are missing vector headers, probably
            EGG::Vector3f local_44 = mTransform.t - mLastPosition;
            nw4r::math::VEC3Add(&local_38, &kartPos, &local_38);
            nw4r::math::VEC3Add(&local_44, &mTransform.t, &local_44);
            f32 dvar6 = PSVECSquareDistance(local_38, local_44);
            if (dvar6 < distNew) { return; }
        }
    }

    // this is bad
    #define ITEM_OBJ_SHELL (Item::ITEMOBJ_GREEN_SHELL | Item::ITEMOBJ_RED_SHELL | Item::ITEMOBJ_BLUE_SHELL)
    #define then
    if (
        ON_BIT(mItemId, ITEM_OBJ_SHELL)
        && ON_BIT(mFlags, DROPPED)
        && ON_BIT(mFlags, GROUNDED)
        && !isCollide
        && (!ON_BIT(~mRenderer->drawFlags.uint, 0x1010101))
    ) then {
        EGG::Vector3f local_68 = mSpeed;
        NormalizeVector(local_68, mSpeed);
        EGG::Vector3f local_5c = mTransform.t - kart->pos();
        MultiplyVectorByDotProduct_LLMname(local_5c, local_68, local_5c);

        // todo: find PSVecSquaredLength, what is it actually called?
        // if (local_5c.PSVecSquaredLength() >= square(250.0f)) {
        if (local_5c.squaredLength() >= 62500.0f) {
            // local_5c = mTransform.t - local_5c;
            nw4r::math::VEC3Sub(&local_5c, &mTransform.t, &local_5c);

            if (mItemId == Item::ITEMOBJ_GREEN_SHELL) {
                Eff_greenShel_FUN_8068d4f8(mScaleFactor, EffectInfo::spInstance->_74, local_5c);
            }
            if (mItemId == Item::ITEMOBJ_RED_SHELL) {
                Eff_redShel_FUN_8068d570(mScaleFactor, EffectInfo::spInstance->_74, local_5c);
            }
        }
    }

    fallGround_807a6738(kart->pos(), kart->speed(), kart->kartMove()->vel1Dir(), true);
    break_807a6614(false, mOnlineTarget);
}

void ItemObj::handlePlayerCollision(const Kart::KartObject &kart, bool online) {
    const ItemData &data = ItemData::table[mItemId];
    if (data.hasPlayerCollision && !online) {
        if (data.hasPlayerEffect) {
            const EGG::Vector3f &vel1Dir = kart.kartMove()->vel1Dir();
            const EGG::Vector3f &speed = kart.speed();
            const EGG::Vector3f &pos = kart.pos();
            fallGround_807a6738( pos, speed, vel1Dir, true);
            break_807a6614(true, kart.getPlayerIdx());
            kart.rumbleController_80590570();
        } else {
            collideSomething_807a6ec8(true);
        }
    }
}

} // namespace Item
