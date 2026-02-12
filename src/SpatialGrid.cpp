#include "SpatialGrid.h"
#include <cmath>
#include <vector>

// 计算空间哈希键
size_t SpatialGrid::hashVoxel(const Vector3& position) const {
    int ix = static_cast<int>(std::floor(position.x / voxelSize));
    int iy = static_cast<int>(std::floor(position.y / voxelSize));
    int iz = static_cast<int>(std::floor(position.z / voxelSize));
    
    // 简单的哈希函数
    return static_cast<size_t>((ix * 73856093) ^ (iy * 19349663) ^ (iz * 83492791));
}

// 获取体素内的所有点
std::vector<size_t> SpatialGrid::getPointsInVoxel(const Vector3& position) const {
    size_t hash = hashVoxel(position);
    auto it = voxelGrid.find(hash);
    if (it != voxelGrid.end()) {
        return it->second.pointIndices;
    }
    return std::vector<size_t>();
}

// 从 FieldLoader 加载数据
void SpatialGrid::loadData(const std::vector<Vector3>& positions, const std::vector<float>& opacities) {
    this->positions = positions;
    this->opacities = opacities;
    buildAccelerationStructure();
}

// 建立加速结构（如 Hash-grid 或 Octree）
void SpatialGrid::buildAccelerationStructure() {
    voxelGrid.clear();
    
    // 构建空间哈希网格
    for (size_t i = 0; i < positions.size(); i++) {
        size_t hash = hashVoxel(positions[i]);
        voxelGrid[hash].pointIndices.push_back(i);
    }
    
    // 计算每个体素的平均密度
    for (auto& [hash, voxel] : voxelGrid) {
        float totalDensity = 0.0f;
        for (size_t idx : voxel.pointIndices) {
            totalDensity += opacities[idx];
        }
        voxel.averageDensity = totalDensity / voxel.pointIndices.size();
    }
}

// 核心查询接口：扫描机探测密度的唯一手段
// 输入空间坐标，返回 0.0 ~ 1.0 的不透明度
float SpatialGrid::getDensityAt(const Vector3& position) const {
    std::vector<size_t> pointIndices = getPointsInVoxel(position);
    
    if (pointIndices.empty()) {
        return 0.0f;
    }
    
    // 计算该点的密度（简单平均）
    float totalDensity = 0.0f;
    int count = 0;
    
    // 考虑周围几个体素以获得更平滑的结果
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dz = -1; dz <= 1; dz++) {
                Vector3 offsetPosition = position + Vector3(dx * voxelSize, dy * voxelSize, dz * voxelSize);
                std::vector<size_t> nearbyPoints = getPointsInVoxel(offsetPosition);
                
                for (size_t idx : nearbyPoints) {
                    // 计算点到查询位置的距离
                    float distance = (positions[idx] - position).length();
                    // 距离衰减
                    float weight = std::max(0.0f, 1.0f - distance / (voxelSize * 2.0f));
                    totalDensity += opacities[idx] * weight;
                    count++;
                }
            }
        }
    }
    
    if (count > 0) {
        return std::min(1.0f, totalDensity / count);
    }
    
    return 0.0f;
}

// 梯度探测：返回该点密度变化最剧烈的方向
// 用于后期确定物体表面边缘
Vector3 SpatialGrid::getDensityGradient(const Vector3& position) const {
    // 计算三个方向的密度差
    float densityCenter = getDensityAt(position);
    float densityXPlus = getDensityAt(position + Vector3(voxelSize * 0.1f, 0, 0));
    float densityXMinus = getDensityAt(position - Vector3(voxelSize * 0.1f, 0, 0));
    float densityYPlus = getDensityAt(position + Vector3(0, voxelSize * 0.1f, 0));
    float densityYMinus = getDensityAt(position - Vector3(0, voxelSize * 0.1f, 0));
    float densityZPlus = getDensityAt(position + Vector3(0, 0, voxelSize * 0.1f));
    float densityZMinus = getDensityAt(position - Vector3(0, 0, voxelSize * 0.1f));
    
    // 计算梯度
    float gradientX = (densityXPlus - densityXMinus) / (voxelSize * 0.2f);
    float gradientY = (densityYPlus - densityYMinus) / (voxelSize * 0.2f);
    float gradientZ = (densityZPlus - densityZMinus) / (voxelSize * 0.2f);
    
    return Vector3(gradientX, gradientY, gradientZ);
}

// 密度采样：在给定空间范围内采样密度超过阈值的点
std::vector<Vector3> SpatialGrid::sampleDensityPoints(const BoundingBox& bounds, float densityThreshold, float sampleStep) const {
    std::vector<Vector3> densityPoints;
    
    // 在边界范围内按照指定步长进行采样
    for (float x = bounds.min.x; x <= bounds.max.x; x += sampleStep) {
        for (float y = bounds.min.y; y <= bounds.max.y; y += sampleStep) {
            for (float z = bounds.min.z; z <= bounds.max.z; z += sampleStep) {
                Vector3 samplePosition(x, y, z);
                float density = getDensityAt(samplePosition);
                
                if (density > densityThreshold) {
                    densityPoints.push_back(samplePosition);
                }
            }
        }
    }
    
    return densityPoints;
}
