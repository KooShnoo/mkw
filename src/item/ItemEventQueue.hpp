#pragma once

#include <rk_types.h>

#include "egg/core/eggDisposer.hpp"

namespace Item {
    struct ItemRKNetEventItem {
        u8 data[0x18];
        u32 objectId;
        u32 action;
        u32 recvTime;
    };
    static_assert(sizeof(ItemRKNetEventItem) == 36);

    class ItemRKNetEventQueue : public EGG::Disposer {
        u8 queueStartId;
        u8 queueLength;

         void * queueInfos;
        ItemRKNetEventItem * queueItems;

        unk32 unk;

        u8 queueRecvStartId;
        u8 queueRecvLength;

        void * queueRecvInfos;
        ItemRKNetEventItem * queueRecvItems;
    };
    static_assert(sizeof(ItemRKNetEventQueue) == 44);
}