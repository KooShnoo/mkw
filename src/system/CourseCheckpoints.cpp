#pragma sym on

#include "CourseCheckpoints.hpp"
#include "CourseMap.hpp"
#include "system/RaceManager.hpp"

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

bool MapdataCheckPoint::checkDistanceRatio(
    const LinkedCheckpoint& next, 
    const EGG::Vector2f& p0,
    const EGG::Vector2f& p1, 
    f32* distanceRatio) const
{
  float dot = EGG::Vector2f::dot(this->mDir, p1);
  float dot2 = -EGG::Vector2f::dot(next.checkpoint->mDir, p0);
  float ratio = (dot) / (dot + dot2);
  
  *distanceRatio = ratio;
  
  return (ratio >= 0.0f) && (ratio <= 1.0f);
}

MapdataCheckPoint::SectorOccupancy MapdataCheckPoint::checkSectorAndDistanceRatio_(
    const LinkedCheckpoint& next,
    const EGG::Vector2f& p0,
    const EGG::Vector2f& p1,
    f32* distanceRatio
) const {   
    if (!this->checkSector(next, p0, p1))
        return OutsideSector;
    
    if (!this->checkDistanceRatio(next, p0, p1, distanceRatio))
        return BetweenSides;

    return InsideSector;
}

MapdataCheckPoint::SectorOccupancy MapdataCheckPoint::checkSectorAndDistanceRatio(
    const EGG::Vector3f& pos,
    f32* distanceRatio
) const {   
    EGG::Vector2f temp1((this->mpData->right).x, (this->mpData->right).y);
    temp1.y = pos.z - temp1.y;
    temp1.x = pos.x - temp1.x;
 
    bool betweenSides = false;
    for (int i = 0; i < this->mNextCount; i++) {
        EGG::Vector2f temp2((this->mNextPoints[i].checkpoint->mpData->left).x, (this->mNextPoints[i].checkpoint->mpData->left).y);
        temp2.y = pos.z - temp2.y;
        temp2.x = pos.x - temp2.x;

        switch(this->checkSectorAndDistanceRatio_(this->mNextPoints[i], temp2, temp1, distanceRatio)) {
            case InsideSector: 
                return InsideSector;
            case BetweenSides: 
                betweenSides = true;
                break;
            default:
                break;
        }
    }

    return betweenSides ? BetweenSides : OutsideSector;
}

/* searchNextCheckpoint(s)
 * Helper function that increments the depth counter and continues the recursive search on the next checkpoint. If there are multiple next checkpoints (split path), 
 * it will continue the search on each branch and stop if the player is found.
 */
s16 CourseMap::searchNextCheckpoint(s32 playerIdx, const EGG::Vector3f &pos, s16 depth, const MapdataCheckPoint &checkpoint, float *completion, u32 params, const bool param_8) const {
    s16 id = -1;
    // increment depth counter unless it's -1
    s16 depth_ = depth >= 0 ? depth + 1 : -1;

    // iterate through each next checkpoint
    for (u16 i = 0; i < checkpoint.nextCount(); i++) {
        MapdataCheckPoint *checkpoint_ = (s32)i < checkpoint.nextCount() ? checkpoint.nextPoint(i) : nullptr;
        // if checkpoint hasn't been searched yet OR param_8 is false
        if (!param_8 || !checkpoint_->isPlayerFlagged(playerIdx)) {
            // call recursive function to continue the search forwards
            id = findRecursiveSector(playerIdx, pos, depth_, 0, *checkpoint_, completion, params);
            if (id != -1)
                // stop if player's checkpoint has been found
                break;
        }
    }
    return id;
}

/* searchPrevCheckpoint(s)
 * Helper function that increments the depth counter and continues the recursive search on the previous checkpoint. If there are multiple previous checkpoints (split path), 
 * it will continue the search on each branch and stop if the player is found.
 */
s16 CourseMap::searchPrevCheckpoint(s32 playerIdx, const EGG::Vector3f &pos, s16 depth, const MapdataCheckPoint &checkpoint, float *completion, u32 params, const bool param_8) const {
    s16 id = -1;
    // increment depth counter unless it's -1
    s16 depth_ = depth >= 0 ? depth + 1 : -1;

    // iterate through each previous checkpoint
    for (u16 i = 0; i < checkpoint.prevCount(); i++) {
        MapdataCheckPoint *checkpoint_ = (s32)i < checkpoint.prevCount() ? checkpoint.prevPoint(i) : nullptr;
        // if checkpoint hasn't been searched yet OR param_8 is false
        if (!param_8 || !checkpoint_->isPlayerFlagged(playerIdx)) {
            // call recursive function to continue the search backwards
            id = findRecursiveSector(playerIdx, pos, depth_, 1, *checkpoint_, completion, params);
            if (id != -1)
                // stop if player's checkpoint has been found
                break;
        }
    }
    return id;
}

/* findRecursiveSector
 * Main recursive function for the checkpoint algorithm
 */
// TODO: param_5 is "eSearchType"
s16 CourseMap::findRecursiveSector(s32 playerIdx, const EGG::Vector3f &pos, s16 depth, int param_5, MapdataCheckPoint &checkpoint, float *completion, u32 params) const {
    s16 maxDepth = params & 4 ? 12 : 6;        // set depth limit to 12 for online players, otherwise 6
    if (depth >= 0 && depth > maxDepth) {      // return immediately if max depth is reached
        return -1;
    }

    // if this checkpoint has been searched already, force set completion type to OutsideSector (why?)
    bool flagged = checkpoint.isPlayerFlagged(playerIdx);                            
    MapdataCheckPoint::SectorOccupancy completion_ = MapdataCheckPoint::OutsideSector;     
    if (!flagged) {
        completion_ = checkpoint.checkSectorAndDistanceRatio(pos, completion);
    }
    checkpoint.setPlayerFlags(playerIdx);

    // if player is inside current checkpoint, return current checkpoint
    if (completion_ == MapdataCheckPoint::InsideSector)        
        return checkpoint.id();

    // Search type 0: Search forwards first, then backwards
    if (param_5 == 0) {

        // If "player is forwards" flag is true but completion < 0, force completion to 0 and return current checkpoint (GHOST CHECKPOINT!)
        if (params & 1 && completion_ == MapdataCheckPoint::BetweenSides && *completion < 0.0f) {
            *completion = 0.0f;
            return checkpoint.id();
        }

        // Stop if current checkpoint is a KCP, unless this is an online player
        if (!(params & 2) && checkpoint.data()->checkArea >= 0)        
            return -1;

        // If player is between the sides of the quad but NOT between this checkpoint and next, AND completion > 0, then set "player is forwards" flag
        u32 params_;
        if (completion_ == MapdataCheckPoint::BetweenSides && *completion > 0.0f)
            params_ = params | 1;
        else
            params_ = params &~ 1;
        
        s16 id = searchNextCheckpoint(playerIdx, pos, depth, checkpoint, completion, params_, false);                // search forwards INCLUDING checkpoints already searched
        return id == -1 ? searchPrevCheckpoint(playerIdx, pos, depth, checkpoint, completion, params_, true) : id;   // if that fails, search backwards EXCLUDING checkpoints already searched
    }

    // Search type 1: Search backwards first, then forwards

    // If "player is backwards" flag is true but completion > 1, force completion to 1 and return current checkpoint (GHOST CHECKPOINT!)
    if (params & 1 && completion_ == MapdataCheckPoint::BetweenSides && *completion > 1.0f) {
        *completion = 1.0f;
        return checkpoint.id();
    }

    // Stop if current checkpoint is a KCP, unless this is an online player
    if (!(params & 2) && checkpoint.data()->checkArea >= 0)
        return -1;

    // If player is between the sides of the quad but NOT between this checkpoint and next, AND completion < 0, then set "player is backwards" flag
    u32 params_;
    if (completion_ == MapdataCheckPoint::BetweenSides && *completion < 0.0f)
        params_ = params | 1;
    else
        params_ = params &~ 1;

    s16 id = searchPrevCheckpoint(playerIdx, pos, depth, checkpoint, completion, params_, false);                // search backwards INCLUDING checkpoints already searched
    return id == -1 ? searchNextCheckpoint(playerIdx, pos, depth, checkpoint, completion, params_, true) : id;   // if that fails, search forwards EXCLUDING checkpoints already searched
}

/* findSector
 * The starting point for the checkpoint search algorithm; called every frame (I think)
 * Searches nearby checkpoints first (local search), then all "loaded" checkpoints (global search)
 * Returns the checkpoint ID that the player is in, or -1 if the player isn't found
 */
s16 CourseMap::findSector(s32 playerIdx, const EGG::Vector3f& pos, u16 checkpointIdx, f32* distanceRatio, bool isRemote) {
    clearSectorChecked();
    MapdataCheckPoint *checkpoint = getCheckPoint(checkpointIdx);
    s16 id = -1;
    MapdataCheckPoint::SectorOccupancy completion = checkpoint->checkSectorAndDistanceRatio(pos, distanceRatio);  // check if player is in starting checkpoint
    checkpoint->setPlayerFlags(playerIdx);    // flag starting checkpoint as searched
    u32 params = 0;
    if (isRemote)
        params = 6;

    // LOCAL SEARCH
    // Searches through checkpoints recursively with a depth limit of 6 (or 12 for online players).
    // Search stops if the player is found, the depth limit is reached, or a key checkpoint is encountered.
    switch (completion) {
        
    // Case 1: the player is fully inside the current checkpoint, so just set to current checkpoint
    case MapdataCheckPoint::InsideSector:    
        id = checkpoint->id();
        break;

    // Case 2: the player is between the sides of the quad, but NOT between this checkpoint and next; player is likely in the same checkpoint group
    case MapdataCheckPoint::BetweenSides:
        
        // Case 2a: the player is closer to the next checkpoint than the current checkpoint; player is most likely to be in NEXT checkpoints
        if (*distanceRatio > 0.5f) {    
            
            // Step 1: Starting at current checkpoint, search forwards
            for (s32 i = 0; i < checkpoint->nextCount(); i++) {
                MapdataCheckPoint *checkpoint_ = i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                id = findRecursiveSector(playerIdx, pos, 1, 0, *checkpoint_, distanceRatio, params);
                if (id != -1)
                    break;
            }
            // Step 2: If step 1 fails, start at next checkpoint(s) and search backwards
            if (id == -1) {
                for (s32 i = 0; i < checkpoint->nextCount(); i++) {
                    MapdataCheckPoint *next = i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                    for (s32 j = 0; j < next->prevCount(); j++) {
                        MapdataCheckPoint *prev = j < next->prevCount() ? next->prevPoint(j) : nullptr;
                        if (prev == checkpoint)
                            continue;

                        id = findRecursiveSector(playerIdx, pos, 1, 1, *prev, distanceRatio, params);
                        if (id != -1)
                            break;
                    }
                }
                // Step 3: If step 2 fails, start at previous checkpoint(s) and search forwards
                if (id == -1) {
                    for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                        MapdataCheckPoint *prev = (s32)i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                        for (s32 j = 0; j < prev->nextCount(); j++) {
                            MapdataCheckPoint *next = (s32)j < prev->nextCount() ? prev->nextPoint(j) : nullptr;
                            if (next == checkpoint)
                                continue;

                            id = findRecursiveSector(playerIdx, pos, 1, 0, *next, distanceRatio, params);
                            if (id != -1)
                                break;
                        }
                    }
                }
            }
            // Step 4: If step 3 fails, start at current checkpoint and search backwards
            if (id == -1) {
                for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                    MapdataCheckPoint *checkpoint_ = i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                    id = findRecursiveSector(playerIdx, pos, 1, 1, *checkpoint_, distanceRatio, params);
                    if (id != -1)
                        break;
                }
            }
            
        // Case 2b: the player is closer to the current checkpoint than the next checkpoint; player is most likely to be in PREVIOUS checkpoints
        } else {    
            // (do all the steps from 2a in reverse)
            // Search prev
            for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *checkpoint_ = i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                id = findRecursiveSector(playerIdx, pos, 1, 1, *checkpoint_, distanceRatio, params);
                if (id != -1)
                    break;
            }
            // If that fails, search prev -> next
            if (id == -1) {
                for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                    MapdataCheckPoint *prev = (s32)i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                    for (s32 j = 0; j < prev->nextCount(); j++) {
                        MapdataCheckPoint *next = (s32)j < prev->nextCount() ? prev->nextPoint(j) : nullptr;
                        if (next == checkpoint)
                            continue;

                        id = findRecursiveSector(playerIdx, pos, 1, 0, *next, distanceRatio, params);
                        if (id != -1)
                            break;
                    }
                }

                // If that fails, search next -> prev
                if (id == -1) {
                    for (s32 i = 0; i < checkpoint->nextCount(); i++) {
                        MapdataCheckPoint *next = (s32)i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                        for (s32 j = 0; j < next->prevCount(); j++) {
                            MapdataCheckPoint *prev = (s32)j < next->prevCount() ? next->prevPoint(j) : nullptr;
                            if (prev == checkpoint)
                                continue;

                            id = findRecursiveSector(playerIdx, pos, 1, 1, *prev, distanceRatio, params);
                            if (id != -1)
                                break;
                        }
                    }
                }
            }
            // If that fails, search next
            if (id == -1) {
                for (s32 i = 0; i < checkpoint->nextCount(); i++) {
                    MapdataCheckPoint *checkpoint_ = i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                    id = findRecursiveSector(playerIdx, pos, 1, 0, *checkpoint_, distanceRatio, params);
                    if (id != -1)
                        break;
                }
            }
        }
        break;

    // Case 3: the player is not between the sides of the quad (may still be between this checkpoint and next); player is likely in a different checkpoint group
    case MapdataCheckPoint::OutsideSector:    
        // Search next -> prev
        for (s32 i = 0; i < checkpoint->nextCount(); i++) {
            MapdataCheckPoint *next = (s32)i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
            for (s32 j = 0; j < next->prevCount(); j++) {
                MapdataCheckPoint *prev = (s32)j < next->prevCount() ? next->prevPoint(j) : nullptr;
                if (prev == checkpoint)
                    continue;

                id = findRecursiveSector(playerIdx, pos, 1, 1, *prev, distanceRatio, params);
                if (id != -1)
                    break;
            }
        }

        // If that fails, search prev -> next
        if (id == -1) {
            for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *prev = i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                for (s32 j = 0; j < prev->nextCount(); j++) {
                    MapdataCheckPoint *next = j < prev->nextCount() ? prev->nextPoint(j) : nullptr;
                    if (next == checkpoint)
                        continue;

                    id = findRecursiveSector(playerIdx, pos, 1, 0, *next, distanceRatio, params);
                    if (id != -1)
                        break;
                }
            }
        }

        // If that fails, search next
        if (id == -1) {
            for (s32 i = 0; i < checkpoint->nextCount(); i++) {
                MapdataCheckPoint *checkpoint_ = i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                id = findRecursiveSector(playerIdx, pos, 1, 0, *checkpoint_, distanceRatio, params);
                if (id != -1)
                    break;
            }
        }

        // If that fails, search prev
        if (id == -1) {
            for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *checkpoint_ = i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                id = findRecursiveSector(playerIdx, pos, 1, 1, *checkpoint_, distanceRatio, params);
                if (id != -1)
                    break;
            }
        }
        
    default:
        break;
    }

    // GLOBAL SEARCH
    // if local search fails, remove depth limit and search all "loaded" checkpoints

    // Step 1: Search all next checkpoints until player or key checkpoint is found 
    if (id == -1) {
        for (s32 i = 0; i < checkpoint->nextCount(); i++) {
            MapdataCheckPoint *checkpoint_ = i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
            id = findRecursiveSector(playerIdx, pos, -1, 0, *checkpoint_, distanceRatio, 0);
            if (id != -1)
                break;
        }
        // Step 2: Search all previous checkpoints until player or key checkpoint is found 
        if (id == -1) {
            for (s32 i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *checkpoint_ = i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                id = findRecursiveSector(playerIdx, pos, -1, 1, *checkpoint_, distanceRatio, 0);
                if (id != -1)
                    break;
            }
        }
    }

    // ONLINE PLAYERS ONLY
    // if both local and global searches fail, just iterate through ALL checkpoints (ignoring kcps) until player is found or last checkpoint reached
    if (isRemote && id == -1) {
        for (u16 i = 0; i < getCheckPointCount(); i++) {
            MapdataCheckPoint *checkpoint_ = getCheckPoint(i);
            if (!checkpoint_->isPlayerFlagged(playerIdx)) {        // search all unsearched checkpoints
                MapdataCheckPoint::SectorOccupancy completion = checkpoint_->checkSectorAndDistanceRatio(pos, distanceRatio);
                checkpoint_->setPlayerFlags(playerIdx);
                if (completion == MapdataCheckPoint::InsideSector) {
                    id = i;
                    break;
                }
            }
        }
    }

    return id;
}

void CourseMap::clearSectorChecked() {
    for (u16 i = 0; i < getCheckPointCount(); i++) {
        getCheckPoint(i)->resetFlags();
    }
}

u16 CourseMap::computeExactFinishingTime(u16 checkpointIdx, const EGG::Vector3f &pos, const EGG::Vector3f &lastPos) {
    EGG::Vector2f lastPos2D = EGG::Vector2f(lastPos.x, lastPos.z);
    EGG::Vector2f pos2D = EGG::Vector2f(pos.x, pos.z);

    return getCheckPoint(checkpointIdx)->computeExactFinishingTime(lastPos2D, pos2D);
}

const u16 MapdataCheckPoint::computeExactFinishingTime(const EGG::Vector2f &lastPos, const EGG::Vector2f &pos) {
    EGG::Vector2f vel;    
    EGG::Vector2f displacement;
    EGG::Vector2f overshoot;
    
    vel.sub(pos, lastPos);    
    vel *= 1.0f / SystemManager::ref().getFrameTime();

    overshoot.sub(lastPos, mMidpoint);   
    displacement.add(overshoot, vel);

    u16 ms;
    for(ms = 1; ms < SystemManager::ref().getFrameTime(); ms++) {
        if(displacement.dot(mDir) >= 0.0f) {
            break;
        }

        EGG::Vector2f newDisplacement;
        newDisplacement.add(displacement, vel);
        displacement = newDisplacement;
    }    
    return ms;
}

// WIP
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

f32 MapdataCheckPointAccessor::calculateMeanTotalDistance() {
    return calculateMeanTotalDistanceRecursive(m_finishLineCheckpointId);
}

} // namespace System
