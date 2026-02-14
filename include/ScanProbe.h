#pragma once

#include "common.h"
#include "SpatialGrid.h"
#include <string>

class ScanProbe {
public:
    // 执行全局扫描：寻找所有潜在的实体簇
    std::vector<RawCluster> performGlobalSurvey();

    // 针对特定 AEID 执行局部精扫
    void refineEntity(const std::string& aeid, float resolution);

    // 状态汇报：将探测结果准备好，发送给协议引擎
    ScanPayload capturePayload();

    // 设置空间网格
    void setSpatialGrid(const SpatialGrid& grid);

    // 设置密度阈值
    void setDensityThreshold(float threshold);

    // 设置几何降噪参数
    void setOutlierRemovalParams(float radius, int minNeighbors);

private:
    SpatialGrid spatialGrid;
    float densityThreshold = 0.01f;
    float outlierRadius = 0.1f;
    int minNeighbors = 5;
    std::vector<RawCluster> detectedClusters;

    // 聚类算法：从密度场中提取实体
    void clusterDensityField();

    // 计算局部密度最大值
    Vector3 findLocalDensityMax(const Vector3& startPosition, float searchRadius) const;

    // 将 RawCluster 转换为 AmeEntity
    AmeEntity convertToEntity(const RawCluster& cluster, int entityId) const;
};
