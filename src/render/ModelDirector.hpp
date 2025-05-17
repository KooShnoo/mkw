#pragma once

#include <rk_types.h>

#include "gfx/GFXManager.hpp"

namespace Render {
    class ModelDirector {
    public:
        ModelDirector();
        virtual ~ModelDirector();
        virtual void setEnabled(bool enable);

        void setLightSetIdx(u8 lightSetIdx);
        void setLightSet(GFX::LightSet* lightSet);
    };
}
