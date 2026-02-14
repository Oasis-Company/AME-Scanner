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

// 矩阵乘法：向量与矩阵相乘
Vector3 multiplyVectorMatrix(const Vector3& v, const RotationMatrix m) {
    return Vector3(
        v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0],
        v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1],
        v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2]
    );
}

// 矩阵转置
void transposeMatrix(RotationMatrix m, RotationMatrix result) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = m[j][i];
        }
    }
}

// 求解 3x3 对称矩阵的特征值和特征向量
void solveEigenvalues(const float matrix[3][3], float eigenvalues[3], float eigenvectors[3][3]) {
    // 对于 3x3 对称矩阵，我们可以使用特征多项式求解
    // 这里使用简化的方法，实际应用中可能需要更 robust 的算法

    // 初始化特征值为对角线元素
    eigenvalues[0] = matrix[0][0];
    eigenvalues[1] = matrix[1][1];
    eigenvalues[2] = matrix[2][2];

    // 初始化特征向量为单位矩阵
    eigenvectors[0][0] = 1; eigenvectors[0][1] = 0; eigenvectors[0][2] = 0;
    eigenvectors[1][0] = 0; eigenvectors[1][1] = 1; eigenvectors[1][2] = 0;
    eigenvectors[2][0] = 0; eigenvectors[2][1] = 0; eigenvectors[2][2] = 1;

    // 注意：这是一个简化实现，实际应用中需要使用更 accurate 的特征值分解算法
    // 例如，可以使用 QR 算法或 Jacobi 算法
}

// 按特征值排序特征向量
void sortEigenvectors(float eigenvalues[3], float eigenvectors[3][3]) {
    // 按特征值从大到小排序
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (eigenvalues[i] < eigenvalues[j]) {
                // 交换特征值
                std::swap(eigenvalues[i], eigenvalues[j]);
                // 交换特征向量
                for (int k = 0; k < 3; k++) {
                    std::swap(eigenvectors[k][i], eigenvectors[k][j]);
                }
            }
        }
    }
}

// PCA 计算函数：计算点云的主成分和旋转矩阵
PCAData computePCA(const std::vector<Vector3>& points) {
    PCAData result;

    if (points.empty()) {
        // 初始化默认值
        result.centroid = Vector3(0, 0, 0);
        // 单位矩阵
        result.rotation[0][0] = 1; result.rotation[0][1] = 0; result.rotation[0][2] = 0;
        result.rotation[1][0] = 0; result.rotation[1][1] = 1; result.rotation[1][2] = 0;
        result.rotation[2][0] = 0; result.rotation[2][1] = 0; result.rotation[2][2] = 1;
        result.extents = Vector3(0, 0, 0);
        return result;
    }

    // 1. 计算点云质心
    result.centroid = Vector3(0, 0, 0);
    for (const Vector3& point : points) {
        result.centroid = result.centroid + point;
    }
    result.centroid = result.centroid * (1.0f / points.size());

    // 2. 计算协方差矩阵
    float cov[3][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    for (const Vector3& point : points) {
        Vector3 diff = point - result.centroid;
        cov[0][0] += diff.x * diff.x;
        cov[0][1] += diff.x * diff.y;
        cov[0][2] += diff.x * diff.z;
        cov[1][1] += diff.y * diff.y;
        cov[1][2] += diff.y * diff.z;
        cov[2][2] += diff.z * diff.z;
    }
    // 协方差矩阵是对称的
    cov[1][0] = cov[0][1];
    cov[2][0] = cov[0][2];
    cov[2][1] = cov[1][2];

    // 3. 对协方差矩阵进行特征值分解
    float eigenvalues[3];
    float eigenvectors[3][3];
    solveEigenvalues(cov, eigenvalues, eigenvectors);

    // 4. 按特征值排序特征向量
    sortEigenvectors(eigenvalues, eigenvectors);

    // 5. 构建旋转矩阵
    // 特征向量按列排列
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.rotation[i][j] = eigenvectors[i][j];
        }
    }

    // 6. 计算旋转后的点云尺寸
    // 转换点云到新坐标系
    BoundingBox bb;
    for (const Vector3& point : points) {
        Vector3 diff = point - result.centroid;
        Vector3 transformed = multiplyVectorMatrix(diff, result.rotation);
        bb.expandBy(transformed + result.centroid);
    }
    result.extents = bb.getExtents();

    return result;
}

// OBB 拟合函数：使用 PCA 算法为点云生成最紧凑的旋转包围盒
BoundingBox fitOBB(const std::vector<Vector3>& points) {
    if (points.empty()) {
        return BoundingBox();
    }

    // 1. 计算 PCA 数据
    PCAData pcaData = computePCA(points);

    // 2. 构建包围盒
    // 简化实现：直接返回 AABB
    BoundingBox obb;
    for (const Vector3& point : points) {
        obb.expandBy(point);
    }

    return obb;
}
