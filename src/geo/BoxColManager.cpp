#include "BoxColManager.hpp"
#include "geo/BoxColUnit.hpp"
#include "item/ItemObj.hpp"
#include "rk_types.h"
#include <math.h>


BoxColUnit *BoxColManager::insertItemObj(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos, bool meep, const Item::ItemObj *item) {
  u32 flags = BOXCOL_FLAG_ITEM;

  if (meep) {
    flags |= BOXCOL_FLAG_100;
  }

  return insert(radius, maxSpeed, pos, flags, item);
}

bool BoxColManager::isSphereInSpatialCache(const EGG::Vector3f& pos, f32 radius, u32 flag) const {
  if (flt_808A47EC != someDist && someFlag == (someFlag | flag)) {
    float v4 = someDist - radius;
    if (fabs((pos.x - someX)) <= v4 && fabs((pos.z - someZ)) <= v4)
      return true;
  }

  return false;
}
