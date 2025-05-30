#pragma once

#include <rk_types.h>

#include "BaseItemMath.hpp"
#include "ItemEventQueue.hpp"
#include "render/ModelDirector.hpp"
#include "field/CollisionInfo.hpp"
#include "geo/BoxColManager.hpp"
#include "snd/ItemSnd.hpp"
#include <egg/math/eggVector.hpp>

namespace Render {
    struct ObjectRenderer {
        union RenderFlags {
            int uint;
            u8 bytes[4];
        };

        float * position;
        float drawDistanceBack;
        float drawDistanceFwd;
        float playerDistances[4];
        short areaGroups;
        u8 unk;
        RenderFlags drawFlags; /* 1 = disable, 20 = disable draw dist calc */
    };
};

extern bool isDKJP;
namespace Item {
    typedef enum eItemHitResult {
        HIT_NONE = 0, 
        HIT_BOUNCE = 1, 
        HIT_BREAK = 2
    } ItemHitResult;

    typedef enum eItemType {
        ITEMOBJ_GREEN_SHELL = 0,
        ITEMOBJ_RED_SHELL = 1,
        ITEMOBJ_BANANA = 2,
        ITEMOBJ_MUSHROOM = 3,
        ITEMOBJ_STAR = 4,
        ITEMOBJ_BLUE_SHELL = 5,
        ITEMOBJ_LIGHTNING = 6,
        ITEMOBJ_FAKE_ITEM_BOX = 7,
        ITEMOBJ_MEGA_MUSHROOM = 8,
        ITEMOBJ_BOMB = 9,
        ITEMOBJ_BLOOPER = 10,
        ITEMOBJ_POW = 11,
        ITEMOBJ_GOLDEN_MUSHROOM = 12,
        ITEMOBJ_BULLET = 13,
        ITEMOBJ_CLOUD = 14
    } ItemType;

    typedef enum ItemObjFlags1 {
        NONE = 0,
        TRAILED = 0x1 /* 00 */,
        EXISTS = 0x2 /* 01 */,
        DROPPED = 0x4 /* 02 */,
        GROUNDED = 0x8 /* 03 */,
        x10 = 0x10 /* 04 */,
        x20 = 0x20 /* 05 */,
        BROKEN = 0x40 /* 06 */,
        VANISHED = 0x80 /* 07 */,
        x100 = 0x100 /* 08 */,
        x200 = 0x200 /* 09 */,
        BELOW_DEAD_PLANE = 0x400 /* 10 */,
        x800 = 0x800 /* 11 */,
        x1000 = 0x1000 /* 12 */,
        x2000 = 0x2000 /* 13 */,
        x4000 = 0x4000 /* 14 */,
        FROM_DROP = 0x8000 /* 15 */,
        LIGHT_RING_ENABLED = 0x10000 /* 16 */,
        x20000 = 0x20000 /* 17 */,
        x40000 = 0x40000 /* 18 */,
        x80000 = 0x80000 /* 19 */,
        x100000 = 0x100000 /* 20 */,
        LOCAL_BROKEN = 0x200000 /* 21 */,
        LOCAL_VANISHED = 0x400000 /* 22 */,
        LOCAL_DESPAWNED = 0x800000 /* 23 */,
        x1000000 = 0x1000000 /* 24 */,
        POS_PREV_OVERRIDDEN = 0x2000000 /* 25 */,
        x4000000 = 0x4000000 /* 26 */,
        TRAIL_HIT_1 = 0x8000000 /* 27 */,
        TRAIL_HIT_2 = 0x10000000 /* 28 */,
        TRAIL_HIT_3 = 0x20000000 /* 29 */,
        x40000000 = 0x40000000 /* 30 */
    } ItemObjFlags1;

    class ItemObj {
    public:
        virtual void initModel();
        virtual void loadResources();

        ItemObj();

        void modelSetVisible(bool visible, bool disable);
        void modelEnable();

        virtual void onActivate();

        void setLightSet(GFX::LightSet *lightSet);
        void setLightSetIdx(u8 lightSetIdx);

        virtual bool onUse();
        virtual ItemHitResult onHit();
        virtual void onDrop();
        virtual void onDespawn();

        void loadFromRecvEvent(ItemType itemType, ItemRKNetEventItem *queueItem, bool isTrailHit, bool isDrop, u8 ownerId);

        virtual void onOnlineShot();
        virtual void onOnlineDrop();

        void init(u16 id, u16 typeIndex, eItemType itemType);

        void scaleHitbox(bool useRadius);
        void fixScale();
        void setScale(EGG::Vector3f * scale);

        // Placeholders:
        void initDefaultRenderer();
        void initRenderer(char *fileName, char *resName, char *shadowResName, int unk1, char **anims, int unk2, void* unk3, int unk4);
    
        typedef void (ItemObj::*UpdateFunc)();
        typedef void (ItemObj::*BounceHitFunc)();
    private:
        ItemType mItemId;
        u16 mId; ///< Global
        u16 mTypeIndex; ///< Id of type
        u16 mNetIdentifier; ///< u8[0] = owner, u8[1] = counter
        
        //TODO: change to vectors
        EGG::Quatf mQuaternion;
        RowVec34 mTransform;
        EGG::Vector3f mSpeed;
        EGG::Vector3f mScale;
        float mScaleFactor;
        
        u8 mOwnerId;
        u8 mTrailOwnerId;
        u16 mField15_0x6e;
        u16 mField16_0x70;
        
        u32 mUpdateRes; ///< 0x4 = despawned, 0x8 = made stationary
        ItemObjFlags1 mFlags; 
        unk32 mFlags2;
        EGG::Vector3f mPosDraw; ///< Used by blue shell while high in the air
        EGG::Vector3f mPosStart; ///< Used during throw if flags2&8
        float * mPositionPtr; ///< Points to h20 mtx
        Render::ModelDirector * mMainModel;
        Render::ModelDirector * mShadowModel;
        Render::ObjectRenderer * mRenderer;
        Render::ModelDirector * mLightModel;
        GFX::LightSet * mLightSet;
        BoxColUnit * mBoxColEntity;
        float mHitboxHeight;
        float mHitboxRadius;
        Vec mLastPosition;
        Vec mLastYRotation;

        u32 mCurCollisionFlag;
        Field::ColInfo mColInfo;
        Field::DrivableColInfo * mDrivableColInfo;
        u32 mLandCollisionFlag;
        u8 mOnlineTarget;
        
        float mTargetYScale;
        EGG::Vector3f mField43_0x144;
        EGG::Vector3f mSomeRotationVec;
        short mSomeCounter;
        short mVanishCountdown;
        
        u32 mActiveTime;
        u32 mCounter;

        UpdateFunc mUpdateFunc;
        BounceHitFunc mBounceHitFunc;

        Sound::ItemSnd * mSoundActor;
        u8 fe;
        bool mField52_0x184;
        u8 f;
        bool mBelowDeadPlane;
    };
    #ifndef __INTELLISENSE__
    static_assert(sizeof(ItemObj) == 0x188);
    #endif
    
}
