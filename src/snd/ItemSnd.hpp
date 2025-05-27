#pragma once

#include <rk_types.h>

namespace Item {
    class ItemObj;
}

namespace Sound {
    class ItemSnd {
    public:
        ItemSnd();
        virtual ~ItemSnd();
        virtual void _vf0x8();
        virtual void SetupSound();
        virtual void detail_SetupSoundWithAmbientInfo();

        virtual void _vf0x18();
        virtual void _vf0x1c();
        virtual void _vf0x20();
        virtual void _vf0x24();
        virtual void _vf0x28();
        virtual void _vf0x2c();
        virtual void _vf0x30();
        virtual void _vf0x34();
        virtual void _vf0x38();
        virtual void _vf0x3c();
        virtual void _vf0x40();
        virtual void _vf0x44();
        virtual void _vf0x48();
        virtual void _vf0x4c();
        virtual void _vf0x50();
        virtual void _vf0x54();
        virtual void _vf0x58();
        virtual void _vf0x5c();
        virtual void _vf0x60();
        virtual void _vf0x64();
        virtual void _vf0x68();
        virtual void _vf0x6c();
        virtual void _vf0x70();
        virtual void _vf0x74();
        virtual void _vf0x78();
        virtual void _vf0x7c();
        virtual void _vf0x80();
        virtual void _vf0x84();
        virtual void _vf0x88();
        virtual void _vf0x8c();
        virtual void _vf0x90();
        virtual void _vf0x94();
        virtual void _vf0x98();
        virtual void _vf0x9c();
        virtual void _vf0xa0();
        virtual void _vf0xa4();
        virtual void _vf0xa8();
        virtual void _vf0xac();
        virtual void _vf0xb0();
        virtual void _vf0xb4();
        virtual void _vf0xb8();
        virtual void _vf0xbc();
        virtual void _vf0xc0();
        virtual void _vf0xc4();
        virtual void _vf0xc8();
        virtual void _vf0xcc();
        virtual void _vf0xd0();
        virtual void _vf0xd4();
        virtual void _vf0xd8();
        virtual void _vf0xdc();
        virtual void _vf0xe0();
        virtual void _vf0xe4();
        virtual void _vf0xe8();
        virtual void _vf0xec();
        virtual void _vf0xf0();
        virtual void _vf0xf4();
        virtual void _vf0xf8();
        virtual void _vf0xfc();
        virtual void _vf0x100();
        virtual void _vf0x104();
        virtual void _vf0x108();
        virtual void _vf0x10c();
        virtual void _vf0x110();
        virtual void _vf0x114();
        virtual void _vf0x118(Item::ItemObj * item, u16 itemId);
    
    private:
        u8 unk[0xac - 0x4];
    };
}
