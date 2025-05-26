#pragma once

#include <rk_types.h>
#include <egg/math/eggVector.hpp>

#include "system/CourseMap.hpp"
#include "ItemObj.hpp"

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

    struct RowVec34 {
        EGG::Vector3f x;
        EGG::Vector3f y;
        EGG::Vector3f z;
        EGG::Vector3f t;

        void normalizeZY();
        void normalizeXZ();
        void normalizeYX();
        void normalizeZX();
        void normalizeXY();
        void normalizeYZ();

        void calcDecomposedQuat(Quaternion *mtxQuat, Quaternion *decompQuat);
        void calcQuatAndRotation(Quaternion *mtxQuat, float angle);
        void rotate(RowVec34 *out, s16 angle);

        inline void mirrorZ() {
            f32 scale = -1;
            nw4r::math::VEC3Scale(&this->z, &this->z, scale);
        }

        inline void normaliseY() {
            bool test = this->z.y * this->z.y < (f32)0.99800104;
            if (test) {
                this->z.y = 0;
                EGG::Vector3f input = this->z;
                getNormVector(&this->z, &input);
                PSVECCrossProduct(this->y, this->z, this->x);
            } else {
                this->z.x = 0;
                this->z.y = 0;
                this->z.z = 1;
                this->x.x = 1;
                this->x.y = 0;
                this->x.z = 0;
            }
        }
    };
    static_assert(sizeof(RowVec34) == 48);
}
