// Adapted from MelgMKW's Pulsar. Modifications have been made.
// Based off of: https://github.com/MelgMKW/Pulsar/blob/4262f79/GameSource/MarioKartWii/Kart/KartManager.hpp
// Copyright (c) 2023 MelgMKW. Licensed under the MIT License.
// See end of file for full license information.

#pragma once

// #include <core/nw4r/ut/List.hpp>
// #include <MarioKartWii/Kart/KartPlayer.hpp>

#include "egg/core/eggDisposer.hpp"
#include "kart/KartObject.hpp"

namespace Kart {

class KartObjectManagerVerify : public EGG::Disposer {};

class KartObjectManager : public EGG::Disposer {
public:
    static KartObjectManager* sInstance;
    static KartObjectManager* CreateInstance();
    static void DestroyInstance();
    /// list of all kart objects offset = 4
    static nw4r::ut::List kartObjects;
    static /*ClipInfo*/ void* curKartClipInfo;
    KartObjectManager();
    ~KartObjectManager() override;

    KartObject* getObject(u32 idx) const;
    void Init();
    void Update();

    KartObjectManagerVerify verifyDisposer;
    KartObject** players;
    u8 playerCount;
    u32 mFlags; ///< some bits are sets depending on the played track
    u8* _2c; ///< contains 1 element per player, set to the player index divided by 4. 0-3: 0, 4-7: 1, 8-11: 2
    u8* _30; ///< contains 1 element per player, set to the player index modulo 4. 0-3: 8x, 4-7: 8(x - 4), 8-11: 8(x - 8)
    u8 unknown_0x34[0x38 - 0x34];
};
static_assert(sizeof(KartObjectManager) == 0x38);

}//namespace kart


// MIT License

// Copyright (c) 2023 MelgMKW

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
