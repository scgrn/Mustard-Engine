/**

zlib License

(C) 2024 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#include "poissonDiscSampling.h"

#include "../math/random.h"

namespace AB {

static b8 isValid(Vec2 candidate, Vec2 sampleRegionSize, f32 cellSize, f32 radius, std::vector<Vec2> points,
    std::vector<std::vector<u32>>& grid) {

    if (candidate.x >= 0 && candidate.x < sampleRegionSize.x && candidate.y >= 0 && candidate.y < sampleRegionSize.y) {
        u32 cellX = (u32)(candidate.x / cellSize);
        u32 cellY = (u32)(candidate.y / cellSize);

        u32 searchStartX = std::max((u32)0, cellX - 2);
        u32 searchEndX = std::min(cellX + 2, (u32)grid.size() - 1);
        u32 searchStartY = std::max((u32)0, cellY - 2);
        u32 searchEndY = std::min(cellY + 2, (u32)grid[0].size() - 1);

        for (u32 x = searchStartX; x <= searchEndX; x++) {
            for (u32 y = searchStartY; y <= searchEndY; y++) {
                u32 pointIndex = grid[x][y];
                if (pointIndex != 0) {
                    f32 sqrDst = (candidate.x - points[pointIndex].x) * (candidate.x - points[pointIndex].x) +
                        (candidate.y - points[pointIndex].y) * (candidate.y - points[pointIndex].y);
                        
                    if (sqrDst < radius * radius) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    return false;
}

std::vector<Vec2> generatePoints(f32 radius, Vec2 sampleRegionSize, u32 numSamplesBeforeRejection) {
    f32 cellSize = radius / sqrt(2.0f);
    u32 gridSizeX = (u32)ceil(sampleRegionSize.x / cellSize);
    u32 gridSizeY = (u32)ceil(sampleRegionSize.y / cellSize);
    std::vector<std::vector<u32>> grid(gridSizeX, std::vector<u32>(gridSizeY, 0));

    std::vector<Vec2> points;
    std::vector<Vec2> spawnPoints;
    
    spawnPoints.push_back(sampleRegionSize / 2.0f);
    while (!spawnPoints.empty()) {
        u32 spawnIndex = rnd(spawnPoints.size());
        Vec2 spawnCenter = spawnPoints[spawnIndex];
        b8 candidateAccepted = false;

        for (u32 i = 0; i < numSamplesBeforeRejection; i++) {
            f32 angle = rnd() * 2.0f * M_PI;
            Vec2 dir(cos(angle), sin(angle));
            Vec2 candidate = spawnCenter + dir * rnd(radius, radius * 2.0f);

            if (isValid(candidate, sampleRegionSize, cellSize, radius, points, grid)) {
                points.push_back(candidate);
                spawnPoints.push_back(candidate);
                grid[(u32)(candidate.x / cellSize)][(u32)(candidate.y / cellSize)] = points.size();
                candidateAccepted = true;
                break;
            }
        }
        if (!candidateAccepted) {
            spawnPoints.erase(spawnPoints.begin() + spawnIndex);
        }
    }

    return points;
}

}   //  namespace

