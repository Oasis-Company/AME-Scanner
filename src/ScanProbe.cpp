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
        for (float dy = -searchRadius; dy <= searchRadius; dy += stepSize) {
            for (float dz = -searchRadius; dz <= searchRadius; dz += stepSize) {
                Vector3 testPosition = startPosition + Vector3(dx, dy, dz);
                float testDensity = spatialGrid.getDensityAt(testPosition);
                if (testDensity > bestDensity) {
                    bestDensity = testDensity;
                    bestPosition = testPosition;
                }
            }
        }
    }
    
    return bestPosition;
}

// 聚类算法：从密度场中提取实体
void ScanProbe::clusterDensityField() {
    // 这里实现一个简单的聚类算法
    // 1. 找到密度高于阈值的点
    // 2. 对这些点进行聚类
    // 3. 为每个聚类创建 RawCluster
    
    // 由于我们没有直接访问原始点数据的方法，这里使用模拟数据
    // 实际实现中应该使用更复杂的聚类算法
    
    // 模拟一些聚类
    detectedClusters.clear();
    
    // 示例：创建一个简单的聚类
    BoundingBox clusterBounds(Vector3(-1, -1, -1), Vector3(1, 1, 1));
    RawCluster cluster;
    cluster.bounds = clusterBounds;
    cluster.averageDensity = 0.5f;
    cluster.points.push_back(Vector3(0, 0, 0));
    
    detectedClusters.push_back(cluster);
}

// 执行全局扫描：寻找所有