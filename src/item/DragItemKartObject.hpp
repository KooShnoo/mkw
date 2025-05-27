#pragma once

#include <rk_types.h>

#include "BaseItemData.hpp"
#include "BaseItemMath.hpp"
#include "ItemObj.hpp"
#include "kart/KartObjectProxy.hpp"
#include "ItemDirector.hpp"

namespace Item {
    class ItemObj;
    struct RowVec34;
    
    class DragItemKartObject : public Kart::KartObjectProxy {
    public:
        DragItemKartObject();
        ~DragItemKartObject();

        void init(ItemDirectorPlayer * player);
        void equip(bool onlinePlayer);
        void itemsBind();
        void itemsUpdate();
        void scaleItem(ItemObj * item);
        void itemsScale();

        void syncPlayerPos();

        void dropItem(ItemObj * item, u32 dragIdx);
        void calcTimedDrop();
        void update();
        void updateDragPosition();

        void firableUse();
        void firableUpdate();
        void activateItem(ItemObj * item);
        void releaseItem(ItemObj * item);
        void onFirableHit(u32 dragIdx, ItemObj * item);

        void trailableUse();
        void trailableUpdate();
        static f32 vecNormalize(Vec * dest, Vec * src);
        void onTrailableHit(u32 dragIdx, ItemObj * item);

        void circlableCalc();
        void circableUse();
        void circableUpdate();
        void circableHit(u32 dragIdx, ItemObj * item);

        void itemDespawn();
        void itemsDespawn();

        void calcThrowVector(bool fixYSpeed, Vec * launchVector, Vec * direction);
        void calcForwardsThrow(bool fixYSpeed, f32 throwSpeed, f32 momentumMultiplier, f32 startRadius, Vec * direction);
        void calcBackwardsThrow(bool fixYSpeed, f32 throwSpeed, f32 momentumMultiplier, f32 startRadius, Vec * direction);

        void onDragItemhit(ItemObj * item);
        void disposeItem(ItemObj * item);
        void dropAll(u32 playerIdx);
        void callDragItemCollision(u32 dragIdx);

        void init();
        void forceRelease();
        void netItemsDespawn();
        void netCalcItemDespawn(ItemObj * item, u32 targetDragNum);
        void netItemsDespawnAll();

    private:
        ItemDirectorPlayer * player;
        u8 playerId;
        u32 curItemId;
        u32 curItemboxItemId;
        u32 itemBehaviour;

        ItemObj * dragItems[3];
        Vec collisionVecs[3];

        u32 dragNum;
        u32 dropTimer;

        RowVec34 transform;
        Vec speed;

        f32 field_0x94;
        f32 speedRatioCapped;
        Vec movement;
        f32 field_0xa8;
        bool inMovement;

        Vec scale;
        u8 field_0xbc[0x154-0xbc];

        bool mForceRelease;

        u8 field_0x155[0x17a-0x155];
        s16 axisRotation;
        u32 field_0x17c;
    };
}
