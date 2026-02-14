#pragma once

#include <array>
#include <vector>

// 三维向量类
class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // 向量运算
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    // 向量长度
    float length() const;
    float lengthSquared() const;

    // 归一化
    Vector3 normalize() const;

    // 点积
    float dot(const Vector3& other) const;

    // 叉积
    Vector3 cross(const Vector3& other) const;
};

// 包围盒类
class BoundingBox {
public:
    Vector3 min;
    Vector3 max;

    BoundingBox() : min(0, 0, 0), max(0, 0, 0) {}
    BoundingBox(const Vector3& min, const Vector3& max) : min(min), max(max) {}

    // 获取中心点
    Vector3 getCenter() const;

    // 获取尺寸
    Vector3 getExtents() const;

    // 检查点是否在包围盒内
    bool contains(const Vector3& point) const;

    // 扩展包围盒以包含点
    void expandBy(const Vector3& point);
};

// OBB 拟合函数：使用 PCA 算法为点云生成最紧凑的旋转包围盒
BoundingBox fitOBB(const std::vector<Vector3>& points);

// 原始聚类结构
struct RawCluster {
    BoundingBox bounds;
    float averageDensity;
    std::vector<Vector3> points;
};

// AME 实体结构
struct AmeEntity {
    std::string aeid_alpha;           // 实体唯一标识符
    BoundingBox bounds;                // 边界框
    float averageDensity;              // 平均密度
    std::vector<Vector3> points;       // 点云数据
    std::string physics_handle;        // 物理引擎句柄
    Vector3 centroid;                  // 中心点
    Vector3 extents;                   // 尺寸
    Vector3 orientation;               // 旋转（欧拉角）
    std::string feature_hash;          // 特征哈希
};

// 扫描结果载荷
struct ScanPayload {
    std::vector<RawCluster> clusters;
    std::vector<AmeEntity> entities;   // 转换后的实体
    BoundingBox globalBounds;
};
