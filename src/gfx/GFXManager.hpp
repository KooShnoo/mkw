#pragma once

#include <rk_types.h>
#include <nw4r/ut/utList.hpp>

#include "egg/core/eggDisposer.hpp"
#include "render/ModelDirector.hpp"

namespace GFX {
    class GFXManager;

    GFXManager *gfxManager[2];
    

    class LightSet {
    public:
        void init(u8 lightSetIdx, bool unk);
        void copyFrom(LightSet *source);

        u8 unk[0x21];
        u8 index;
    };

    class LightManagerHolder {
    public:
        nw4r::ut::List activeLights;
        nw4r::ut::List inactiveLights;
        
        LightManagerHolder(GFXManager * gfxManager);
        virtual ~LightManagerHolder();
        virtual void loadBlight();
        virtual void _vf_0x10();
        virtual void _vf_0x14();
        virtual void loadLightMap();

    private:
        GFXManager * gfxManager;
        void * lightManager;
        int listSizeB;
    };

    class GFXManager : public EGG::Disposer {
    public:
        int heap;
        int field2_0x14;
        int memAllocator;
        int field4_0x1c;
        int field5_0x20;
        int * scnRootMain;
        int scnRoot[2];
        void * renderer;
        LightManagerHolder * lightManagerHolder;
        int field10_0x38;
        int field11_0x3c;
        short field12_0x40;
        
        nw4r::ut::List field15_0x44;
        int field16_0x50;
        int field17_0x54;
        nw4r::ut::List objects;
        nw4r::ut::List field19_0x64;
        nw4r::ut::List field20_0x70;
        int field21_0x7c;
        int * * field22_0x80;
        int field23_0x84;
        u8 field24_0x88;
        
        int field28_0x8c;
        float field29_0x90;
        int field30_0x94;

        
        static LightSet * acquireLightSet();
        static void releaseLightSet(LightSet *lightSet);
    };
}
