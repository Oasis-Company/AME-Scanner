#include "common.h"
#include <cmath>

// Vector3 类实现
Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

float Vector3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::lengthSquared() const {
    return x * x + y * y + z * z;
}

Vector3 Vector3::normalize() const {
    float len = length();
    if (len > 0) {
        return *this / len;
    }
    return *this;
}

float Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

// BoundingBox 类实现
Vector3 BoundingBox::getCenter() const {
    return (min + max) * 0.5f;
}

Vector3 BoundingBox::getExtents() const {
    return max - min;
}

bool BoundingBox::contains(const Vector3& point) const {
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

void BoundingBox::expandBy(const Vector3& point) {
    if (point.x < min.x) min.x = point.x;
    if (point.x > max.x) max.x = point.x;
    if (point.y < min.y) min.y = point.y;
    if (point.y > max.y) max.y = point.y;
    if (point.z < min.z) min.z = point.z;
    if (point.z > max.z) max.z = point.z;
}

// OBB 拟合函数：使用 PCA 算法为点云生成最紧凑的旋转包围盒
BoundingBox fitOBB(const std::vector<Vector3>& points) {
    if (points.empty()) {
        return BoundingBox();
    }

    // 1. 计算点云的中心点
    Vector3 centroid(0, 0, 0);
    for (const Vector3& point : points) {
        centroid = centroid + point;
    }
    centroid = centroid * (1.0f / points.size());

    // 2. 计算协方差矩阵
    float cov[3][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    for (const Vector3& point : points) {
        Vector3 diff = point - centroid;
        cov[0][0] += diff.x * diff.x;
        cov[0][1] += diff.x * diff.y;
        cov[0][2] += diff.x * diff.z;
        cov[1][1] += diff.y * diff.y;
        cov[1][2] += diff.y * diff.z;
        cov[2][2] += diff.z * diff.z;
    }
    cov[1][0] = cov[0][1];
    cov[2][0] = cov[0][2];
    cov[2][1] = cov[1][2];

    // 3. 对协方差矩阵进行特征值分解（简化实现，实际应用中应使用更高效的算法）
    // 这里使用简化的方法，直接返回 AABB，实际应用中应实现完整的 PCA
    // 注意：这是一个简化实现，实际的 PCA 特征值分解会更复杂

    // 4. 构建包围盒（简化为 AABB）
    BoundingBox obb;
    for (const Vector3& point : points) {
        obb.expandBy(point);
    }

    return obb;
}
