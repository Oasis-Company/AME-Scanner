#pragma once

#include <vector>
#include <Eigen/Core>
#include "gaussian.h"

namespace AmeScanner {

class SurfaceExtractor {
public:
    SurfaceExtractor(float normal_threshold = 0.3f, float curvature_threshold = 0.5f) 
        : normal_threshold_(normal_threshold), curvature_threshold_(curvature_threshold) {}
    
    // Set parameters
    void setNormalThreshold(float threshold) { normal_threshold_ = threshold; }
    void setCurvatureThreshold(float threshold) { curvature_threshold_ = threshold; }
    
    // Extract surface candidates from gaussians
    std::vector<std::vector<size_t>> extractSurfaceCandidates(const std::vector<Gaussian>& gaussians);
    
    // Compute normal for a gaussian
    Eigen::Vector3f computeNormal(const std::vector<Gaussian>& gaussians, size_t gaussian_idx);
    
    // Compute curvature for a gaussian
    float computeCurvature(const std::vector<Gaussian>& gaussians, size_t gaussian_idx);
    
    // Get normals for all gaussians
    const std::vector<Eigen::Vector3f>& getNormals() const { return normals_; }
    
    // Get curvatures for all gaussians
    const std::vector<float>& getCurvatures() const { return curvatures_; }
    
private:
    float normal_threshold_;
    float curvature_threshold_;
    
    std::vector<Eigen::Vector3f> normals_;
    std::vector<float> curvatures_;
    
    // Helper methods
    std::vector<size_t> findNeighbors(const std::vector<Gaussian>& gaussians, size_t gaussian_idx, float radius);
    Eigen::Vector3f estimateNormalFromNeighbors(const std::vector<Gaussian>& gaussians, const std::vector<size_t>& neighbors);
    float estimateCurvatureFromNeighbors(const std::vector<Gaussian>& gaussians, const std::vector<size_t>& neighbors);
};

} // namespace AmeScanner
