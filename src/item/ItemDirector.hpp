#pragma once

#include <rk_types.h>

#include "egg/core/eggDisposer.hpp"
#include "kart/KartObjectProxy.hpp"
#include "ItemObj.hpp"

namespace Item {
    class ItemDirector;

    ItemDirector * PtrItemDirector = 0;

    class ItemDirectorPlayer : public Kart::KartObjectProxy {
        u8 placeHolder[0x248 - 0xc];
    };

    class ItemDirector : public EGG::Disposer {
    public:
        u8 playerNum;

        ItemDirectorPlayer * playerList;
        ItemDirectorPlayer * players[12];

        u8 itemTable[540];
        u8 field_0x264[0x40];
        ItemObj itemObj;
        int tripleLimit;
    };
}
