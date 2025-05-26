#pragma once

#include <rk_types.h>
#include <rvl/mtx/mtx.h>

#include "ItemObj.hpp"
#include "egg/core/eggDisposer.hpp"
#include "kart/KartObjectProxy.hpp"
#include "system/CourseMap.hpp"

namespace Item {
    class ItemDirectorPlayer : public Kart::KartObjectProxy {
        u8 placeHolder[0x248 - 0xc];
    };

    class ItemDirector : public EGG::Disposer {
    public:
        static ItemDirector * spInstance;
        u8 playerNum;

        ItemDirectorPlayer * playerList;
        ItemDirectorPlayer * players[12];

        u8 itemTable[540];
        u8 field_0x264[0x40];
        u8 itemObj[0x42c - 0x2a4]; //TODO: base ItemObj class
        int tripleLimit;
    };
    static_assert(sizeof(ItemDirector) == 0x430);
}
