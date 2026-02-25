#pragma once

#include <Eigen/Core>

namespace AmeScanner {

class Gaussian {
public:
    Gaussian() = default;
    Gaussian(
        const Eigen::Vector3f& position,
        const Eigen::Vector3f& color,
        float opacity,
        const Eigen::Vector3f& scale,
        const Eigen::Quaternionf& rotation
    ) : position(position), color(color), opacity(opacity), scale(scale), rotation(rotation) {}
    
    Eigen::Vector3f getPosition() const { return position; }
    void setPosition(const Eigen::Vector3f& pos) { position = pos; }
    
    Eigen::Vector3f getColor() const { return color; }
    void setColor(const Eigen::Vector3f& col) { color = col; }
    
    float getOpacity() const { return opacity; }
    void setOpacity(float opa) { opacity = opa; }
    
    Eigen::Vector3f getScale() const { return scale; }
    void setScale(const Eigen::Vector3f& s) { scale = s; }
    
    Eigen::Quaternionf getRotation() const { return rotation; }
    void setRotation(const Eigen::Quaternionf& rot) { rotation = rot; }
    
    // Compute covariance matrix from scale and rotation
    Eigen::Matrix3f computeCovariance() const {
        Eigen::Matrix3f R = rotation.toRotationMatrix();
        Eigen::Matrix3f S = Eigen::DiagonalMatrix<float, 3>(scale.x() * scale.x(), scale.y() * scale.y(), scale.z() * scale.z());
        return R * S * R.transpose();
    }
    
    // Compute inverse covariance matrix
    Eigen::Matrix3f computeInverseCovariance() const {
        Eigen::Matrix3f cov = computeCovariance();
        return cov.inverse();
    }
    
    // Compute Mahalanobis distance to a point
    float computeMahalanobisDistance(const Eigen::Vector3f& point) const {
        Eigen::Vector3f diff = point - position;
        Eigen::Matrix3f inv_cov = computeInverseCovariance();
        return std::sqrt(diff.transpose() * inv_cov * diff);
    }
    
private:
    Eigen::Vector3f position;    // 3D position
    Eigen::Vector3f color;       // RGB color
    float opacity;              // Opacity
    Eigen::Vector3f scale;       // Scale factors
    Eigen::Quaternionf rotation; // Rotation
};

} // namespace AmeScanner
