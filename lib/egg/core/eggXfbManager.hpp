#pragma once

#include <rk_types.h>

#include <decomp.h>

// Source:
// https://github.com/doldecomp/ogws/blob/master/include/egg/core/eggXfbManager.h

namespace EGG {
class Heap;
class Xfb;
class XfbManager {
public:
  inline XfbManager(Heap* heap)
    : mListHead(nullptr), mListTail(nullptr),
      mCopiedXfb(nullptr), mShowXfb(nullptr) {}
  ~XfbManager() {}

  bool isReadytoCopy() const {
    return mListTail != mShowXfb && mListTail != mListHead;
  }

  bool attach(Xfb*);
  void copyEFB(bool);
  void setNextFrameBuffer();
  void postVRetrace();

private:
  Xfb* mListHead;  // at 0x0
  Xfb* mListTail;  // at 0x4
  Xfb* mCopiedXfb; // at 0x8
  Xfb* mShowXfb;   // at 0xC
};
} // namespace EGG
