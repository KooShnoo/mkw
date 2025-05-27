#pragma once

#include <rk_types.h>
#include <egg/math/eggVector.hpp>

extern "C" void getNormVector(EGG::Vector3f * out, EGG::Vector3f * in);
namespace Item {
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

    inline nw4r::math::VEC3* VECMultiply(register nw4r::math::VEC3* pOut, const register nw4r::math::VEC3* p, register f32 scale)
    {
    #if defined(NW4R_MATH_BROADWAY)
        register f32 a, b;
        asm
        {
            psq_l    a, 0(p), 0, 0;
            ps_muls0 b, a, scale;
            psq_l    a, 8(p), 1, 0;
            psq_st   b, 0(pOut), 0, 0;
            ps_muls0 b, a, scale;
            psq_st   b, 8(pOut), 1, 0;
        }
    #else
        pOut->x = p->x * scale;
        pOut->y = p->y * scale;
        pOut->z = p->z * scale;
    #endif
        return pOut;
    }
}
