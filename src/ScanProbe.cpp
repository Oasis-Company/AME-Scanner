#include "ScanProbe.h"
#include <cmath>
#include <vector>
#include <sstream>

// 设置空间网格
void ScanProbe::setSpatialGrid(const SpatialGrid& grid) {
    this->spatialGrid = grid;
}

// 设置密度阈值
void ScanProbe::setDensityThreshold(float threshold) {
    this->densityThreshold = threshold;
}

// 设置几何降噪参数
void ScanProbe::setOutlierRemovalParams(float radius, int minNeighbors) {
    this->outlierRadius = radius;
    this->minNeighbors = minNeighbors;
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

// 将 RawCluster 转换为 AmeEntity
AmeEntity ScanProbe::convertToEntity(const RawCluster& cluster, int entityId) const {
    AmeEntity entity;
    
    // 生成实体唯一标识符
    std::stringstream ss;
    ss << "entity_" << entityId;
    entity.aeid_alpha = ss.str();
    
    // 复制基本属性
    entity.averageDensity = cluster.averageDensity;
    entity.points = cluster.points;
    
    // 应用几何降噪
    if (!entity.points.empty()) {
        entity.points = spatialGrid.removeOutliers(entity.points, outlierRadius, minNeighbors);
    }
    
    // 拟合 OBB
    if (!entity.points.empty()) {
        entity.bounds = fitOBB(entity.points);
    } else {
        entity.bounds = cluster.bounds;
    }
    
    // 计算中心点和尺寸
    entity.centroid = entity.bounds.getCenter();
    entity.extents = entity.bounds.getExtents();
    
    // 初始化旋转（默认为零旋转）
    entity.orientation = Vector3(0, 0, 0);
    
    // 生成物理引擎句柄
    std::stringstream phandle;
    phandle << "physics_" << entityId;
    entity.physics_handle = phandle.str();
    
    // 生成特征哈希（简化实现）
    std::stringstream fhash;
    fhash << "hash_" << entityId;
    entity.feature_hash = fhash.str();
    
    return entity;
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
    cluster.points.push_back(Vector3(0.1, 0.1, 0.1));
    cluster.points.push_back(Vector3(-0.1, -0.1, -0.1));
    cluster.points.push_back(Vector3(0.1, -0.1, 0.1));
    cluster.points.push_back(Vector3(-0.1, 0.1, -0.1));
    
    detectedClusters.push_back(cluster);
}

// 执行全局扫描：寻找所有潜在的实体簇
std::vector<RawCluster> ScanProbe::performGlobalSurvey() {
    clusterDensityField();
    return detectedClusters;
}

// 针对特定 AEID 执行局部精扫
void ScanProbe::refineEntity(const std::string& aeid, float resolution) {
    // 这里实现局部精扫逻辑
    // 实际实现中应该根据 AEID 找到对应的实体，然后在其周围进行更精细的扫描
    
    // 示例：简单打印信息
    // std::cout << "Refining entity " << aeid << " with resolution " << resolution << std::endl;
}

// 状态汇报：将探测结果准备好，发送给协议引擎
ScanPayload ScanProbe::capturePayload() {
    ScanPayload payload;
    payload.clusters = detectedClusters;
    
    // 计算全局边界
    if (!detectedClusters.empty()) {
        BoundingBox globalBounds = detectedClusters[0].bounds;
        for (const auto& cluster : detectedClusters) {
            globalBounds.expandBy(cluster.bounds.min);
            globalBounds.expandBy(cluster.bounds.max);
        }
        payload.globalBounds = globalBounds;
    }
    
    // 转换 RawCluster 为 AmeEntity
    for (int i = 0; i < detectedClusters.size(); i++) {
        AmeEntity entity = convertToEntity(detectedClusters[i], i);
        payload.entities.push_back(entity);
    }
    
    return payload;
}
