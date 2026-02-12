#include "ScanProbe.h"
#include <cmath>
#include <vector>

// 设置空间网格
void ScanProbe::setSpatialGrid(const SpatialGrid& grid) {
    this->spatialGrid = grid;
}

// 设置密度阈值
void ScanProbe::setDensityThreshold(float threshold) {
    this->densityThreshold = threshold;
}

// 计算局部密度最大值
Vector3 ScanProbe::findLocalDensityMax(const Vector3& startPosition, float searchRadius) const {
    Vector3 bestPosition = startPosition;
    float bestDensity = spatialGrid.getDensityAt(startPosition);
    
    // 简单的网格搜索
    float stepSize = searchRadius * 0.1f;
    for (float dx = -searchRadius; dx <= searchRadius; dx += stepSize) {
        for (float dy = -searchRadius; dy <= searchRadius; dy += stepSize)