#include "surface_extractor.h"
#include <iostream>
#include <chrono>
#include <Eigen/Eigenvalues>

namespace AmeScanner {

std::vector<std::vector<size_t>> SurfaceExtractor::extractSurfaceCandidates(const std::vector<Gaussian>& gaussians) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    size_t n = gaussians.size();
    normals_.resize(n);
    curvatures_.resize(n);
    
    // Compute normals and curvatures for all gaussians
    for (size_t i = 0; i < n; ++i) {
        normals_[i] = computeNormal(gaussians, i);
        curvatures_[i] = computeCurvature(gaussians, i);
    }
    
    // Mark surface candidates
    std::vector<bool> is_surface_candidate(n, false);
    for (size_t i = 0; i < n; ++i) {
        // Check if curvature is below threshold (indicating smooth surface)
        if (curvatures_[i] < curvature_threshold_) {
            is_surface_candidate[i] = true;
        }
    }
    
    // Group surface candidates into connected regions
    std::vector<std::vector<size_t>> surface_candidates;
    std::vector<bool> visited(n, false);
    
    for (size_t i = 0; i < n; ++i) {
        if (is_surface_candidate[i] && !visited[i]) {
            std::vector<size_t> candidate_region;
            std::vector<size_t> queue = {i};
            visited[i] = true;
            
            while (!queue.empty()) {
                size_t current_idx = queue.back();
                queue.pop_back();
                candidate_region.push_back(current_idx);
                
                // Find neighbors
                std::vector<size_t> neighbors = findNeighbors(gaussians, current_idx, 0.2f);
                for (size_t neighbor_idx : neighbors) {
                    if (is_surface_candidate[neighbor_idx] && !visited[neighbor_idx]) {
                        queue.push_back(neighbor_idx);
                        visited[neighbor_idx] = true;
                    }
                }
            }
            
            // Only add regions with sufficient size
            if (candidate_region.size() >= 5) {
                surface_candidates.push_back(candidate_region);
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    float duration_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    std::cout << "Surface candidate extraction completed in " << duration_ms << " ms" << std::endl;
    std::cout << "Found " << surface_candidates.size() << " surface candidates" << std::endl;
    
    return surface_candidates;
}

Eigen::Vector3f SurfaceExtractor::computeNormal(const std::vector<Gaussian>& gaussians, size_t gaussian_idx) {
    std::vector<size_t> neighbors = findNeighbors(gaussians, gaussian_idx, 0.1f);
    if (neighbors.size() < 3) {
        // Not enough neighbors, return default normal
        return Eigen::Vector3f(0.0f, 1.0f, 0.0f);
    }
    
    return estimateNormalFromNeighbors(gaussians, neighbors);
}

float SurfaceExtractor::computeCurvature(const std::vector<Gaussian>& gaussians, size_t gaussian_idx) {
    std::vector<size_t> neighbors = findNeighbors(gaussians, gaussian_idx, 0.1f);
    if (neighbors.size() < 3) {
        // Not enough neighbors, return high curvature
        return 1.0f;
    }
    
    return estimateCurvatureFromNeighbors(gaussians, neighbors);
}

std::vector<size_t> SurfaceExtractor::findNeighbors(const std::vector<Gaussian>& gaussians, size_t gaussian_idx, float radius) {
    std::vector<size_t> neighbors;
    const auto& pos = gaussians[gaussian_idx].getPosition();
    
    for (size_t i = 0; i < gaussians.size(); ++i) {
        if (i == gaussian_idx) {
            continue;
        }
        
        float dist = (gaussians[i].getPosition() - pos).norm();
        if (dist <= radius) {
            neighbors.push_back(i);
        }
    }
    
    return neighbors;
}

Eigen::Vector3f SurfaceExtractor::estimateNormalFromNeighbors(const std::vector<Gaussian>& gaussians, const std::vector<size_t>& neighbors) {
    // Compute covariance matrix of neighbor positions
    Eigen::Vector3f mean = Eigen::Vector3f::Zero();
    for (size_t neighbor_idx : neighbors) {
        mean += gaussians[neighbor_idx].getPosition();
    }
    mean /= neighbors.size();
    
    Eigen::Matrix3f covariance = Eigen::Matrix3f::Zero();
    for (size_t neighbor_idx : neighbors) {
        Eigen::Vector3f pos = gaussians[neighbor_idx].getPosition() - mean;
        covariance += pos * pos.transpose();
    }
    covariance /= neighbors.size();
    
    // Compute eigenvalues and eigenvectors
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);
    Eigen::Vector3f eigenvalues = solver.eigenvalues();
    Eigen::Matrix3f eigenvectors = solver.eigenvectors();
    
    // Find smallest eigenvalue (corresponding to normal direction)
    size_t min_eigenvalue_idx = 0;
    if (eigenvalues[1] < eigenvalues[min_eigenvalue_idx]) {
        min_eigenvalue_idx = 1;
    }
    if (eigenvalues[2] < eigenvalues[min_eigenvalue_idx]) {
        min_eigenvalue_idx = 2;
    }
    
    // Return corresponding eigenvector as normal
    Eigen::Vector3f normal = eigenvectors.col(min_eigenvalue_idx);
    return normal.normalized();
}

float SurfaceExtractor::estimateCurvatureFromNeighbors(const std::vector<Gaussian>& gaussians, const std::vector<size_t>& neighbors) {
    // Compute covariance matrix of neighbor positions
    Eigen::Vector3f mean = Eigen::Vector3f::Zero();
    for (size_t neighbor_idx : neighbors) {
        mean += gaussians[neighbor_idx].getPosition();
    }
    mean /= neighbors.size();
    
    Eigen::Matrix3f covariance = Eigen::Matrix3f::Zero();
    for (size_t neighbor_idx : neighbors) {
        Eigen::Vector3f pos = gaussians[neighbor_idx].getPosition() - mean;
        covariance += pos * pos.transpose();
    }
    covariance /= neighbors.size();
    
    // Compute eigenvalues
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);
    Eigen::Vector3f eigenvalues = solver.eigenvalues();
    
    // Compute curvature as ratio of smallest to largest eigenvalue
    float max_eigenvalue = eigenvalues.maxCoeff();
    float min_eigenvalue = eigenvalues.minCoeff();
    
    if (max_eigenvalue < 1e-6) {
        return 0.0f; // Flat region
    }
    
    float curvature = min_eigenvalue / max_eigenvalue;
    return 1.0f - curvature; // Higher value indicates more curvature
}

} // namespace AmeScanner
