#include "CourseCheckpoints.hpp"

#include "CourseMap.hpp"
#include "system/RaceManager.hpp"

// Symbol: checkSector__Q26System17MapdataCheckPointCFRCQ26System16LinkedCheckpointRCQ23EGG8Vector2fRCQ23EGG8Vector2f
namespace System {
bool MapdataCheckPoint::checkSector(const LinkedCheckpoint& next,
                                    const EGG::Vector2f& p0,
                                    const EGG::Vector2f& p1) const {
  if (-(next.p0diff.y) * p0.x + next.p0diff.x * p0.y < 0.0f)
    return false;

  if (next.p1diff.y * p1.x - next.p1diff.x * p1.y < 0.0f)
    return false;

  return true;
}

f32 MapdataCheckPointAccessor::calculateMeanTotalDistanceRecursive(u16 ckptId) {
    f32 sumDist = 0.0f;
    MapdataCheckPoint *ckpt = get(ckptId);
    u16 n = ckpt->nextCount();
    for (u16 i = 0; i < n; i++) {
        ckpt = ckpt->nextPoint(i);
        const LinkedCheckpoint &linked = ckpt->mNextPoints[i];
        sumDist += linked.distance;
        if (m_finishLineCheckpointId == ckpt->id()) { continue; }
        ckpt = ckpt->nextPoint(i);
        sumDist += calculateMeanTotalDistanceRecursive(ckpt->id());
    }
    return sumDist / n;
}

MapdataCheckPathAccessor::MapdataCheckPathAccessor(const KmpSectionHeader *header)
    : MapdataAccessorBase<MapdataCheckPath, MapdataCheckPath::SData>(header) {
    init(reinterpret_cast<const MapdataCheckPath::SData *>(sectionHeader + 1), sectionHeader->entryCount);
    loadPaths();
}

MapdataCheckPath::MapdataCheckPath(const SData *data) : mpData(data), mDfsDepth(-1), mOneOverCount(1.0f) {
    mOneOverCount = 1.0f / data->size;
}

MapdataCheckPathAccessor *CourseMap::parseCheckPath(u32 sectionName) {
    const KmpSectionHeader *sectionPtr = mpCourse->findSection(sectionName);

    MapdataCheckPathAccessor *accessor = 0;
    if (sectionPtr) { accessor = new MapdataCheckPathAccessor(sectionPtr); }

    return accessor;
}

} // namespace System
