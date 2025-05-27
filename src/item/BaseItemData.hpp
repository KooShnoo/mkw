#pragma once

#include <rk_types.h>

#include "ItemObj.hpp"
#include "system/CourseMap.hpp"

extern "C" void getNormVector(EGG::Vector3f * out, EGG::Vector3f * in);
namespace Item {
    class ItemObj;
    struct ItemData {
        static ItemData table[15];

        static void setup();
        static void createTable();
        static bool isTimeTrial();
        static bool unkPlayerFlag();
        static void* getUnkPlayerInstance();

        static u16 getGeoObjNum();
        static u16 getGeoObjType(System::MapdataGeoObj* geoObj);
        static System::MapdataGeoObj* getGeoObjById(u32 id);

        static void calcItemFlowLimits();

        static f32 getHitboxHeight(u32 item) {
            return table[item].hitboxHeight;
        }
        static f32 getHitboxSize(u32 item) {
            ItemData * tableItem = &table[item];
            return tableItem->getHitboxSize();
        }
        static f32 getDrawDistBack(u32 item) {
            return table[item].drawDistBack;
        }

        f32 getHitboxSize() {
            return this->hitboxSize;
        }

        
        void * initializeItemArrayFunc;
        u32 boxLimit;
        u32 competitionLimit;
        u32 itemArraySize;
        u32 hardLimit;
        bool overrideVanishFunc;
        bool overrideBreakFunc;
        bool overrideUpdateFunc;
        
        typedef void (ItemObj::*TrailingObjUpdateFunc)(u32);
        typedef void (ItemObj::*RecvItemInitFunc)(void *, bool);
        ItemObj::UpdateFunc trailingObjUpdateFunc;
        void (* recvItemInitFunc)(void *, void *, bool);
        u32 eventPacketSize;
        bool disableBoxCol;
        
        float drawDistFwd;
        float drawDistBack;
        float hitboxHeight;
        float hitboxRadius;
        float hitboxSize;
        float scaleFactor;
        float hitboxScale;
        float field23_0x4c;
        float field24_0x50;
        float field25_0x54;
        bool canLand;
        bool hasPlayerEffect;
        bool hasPlayerCollision;
        
        int tripleLimitWeight;
        u32 breakSound;
        u32 hitSound;
        u32 landSound;
        float shadowScale;
        bool useLight;
    };
}
