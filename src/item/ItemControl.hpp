#pragma once

#include <rk_types.h>
#include <rvl/mtx/mtx.h>

#include "ItemObj.hpp"

namespace Item {
    class ItemControl {
        ItemControl();
        void init(ItemType itemType, u32 &startId);
        void initModels();

        void spawn(u32 num, ItemObj ** res, u8 ownerId, Vec * position, bool fixedNetId);
        void despawnItem(ItemObj * item);
        void activateItem(ItemObj * item);

        void sortGroundList();
        void update();

        void vanish();
        void despawn();
        void despawnAll();

        void drawUpdate();

        void groundItem(ItemObj * item);
        void breakAll();

        // 5 net functions

        u32 getTotalNum();
        u16 upCurSpawnId();

    private:
        ItemType itemType;
        ItemObj ** itemList;
        u32 listLength;
        u32 hardLimit; //Held + spawned
        u32 spawnNum; //Active + ground
        u32 activeNum;
        u32 groundNum;
        u32 boxLimit;

        u16 curSpawnId;
    };
    static_assert(sizeof(ItemControl) == 0x24);
}
