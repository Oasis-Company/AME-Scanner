#pragma once

#include "common.h"
#include <unordered_map>

class SpatialGrid {
public:
    // 核心查询接口：扫描机探测密度的唯一手段
    // 输入空间坐标，返回 0.0 ~ 1.0 的不透明度
    float getDensityAt(const Vector3& position) const;

    // 梯度探测：返回该点密度变化最剧烈的方向
    // 用于后期确定物体表面边缘
    Vector3 getDensityGradient(const Vector3& position) const;

    // 建立加速结构（如 Hash-grid 或 Octree）
    void buildAccelerationStructure();

    // 从 FieldLoader 加载数据
    void loadData(const std::vector<Vector3>& positions, const std::vector<float>& opacities);

private:
    // 空间哈希网格实现
    struct Voxel {
        std::vector<size_t> pointIndices;
        float averageDensity;
    };

    std::vector<Vector3> positions;
    std::vector<float> opacities;
    std::unordered_map<size_t, Voxel> voxelGrid;
    float voxelSize = 0.1f; // 体素大小

    // 计算空间哈希键
    size_t hashVoxel(const Vector3& position) const;

    // 获取体素内的所有点
    std::vector<size_t> getPointsInVoxel(const Vector3& position) const;

    // 密度采样：在给定空间范围内采样密度超过阈值的点
    std::vector<Vector3> sampleDensityPoints(const BoundingBox& bounds, float densityThreshold, float sampleStep) const;
};
