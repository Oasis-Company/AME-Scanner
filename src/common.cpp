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
