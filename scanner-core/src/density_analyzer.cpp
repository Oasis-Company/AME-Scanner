#include "density_analyzer.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace AmeScanner {

std::vector<float> DensityAnalyzer::computeDensityField(
    const std::vector<Gaussian>& gaussians,
    Eigen::Vector3f& min_bounds,
    Eigen::Vector3f& max_bounds,
    Eigen::Vector3i& grid_dims
) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Compute scene bounds
    min_bounds = Eigen::Vector3f::Constant(std::numeric_limits<float>::max());
    max_bounds = Eigen::Vector3f::Constant(std::numeric_limits<float>::lowest());
    
    for (const auto& gaussian : gaussians) {
        const auto& pos = gaussian.getPosition();
        min_bounds = min_bounds.cwiseMin(pos);
        max_bounds = max_bounds.cwiseMax(pos);
    }
    
    // Expand bounds slightly to include all points
    min_bounds -= Eigen::Vector3f::Constant(grid_size_);
    max_bounds += Eigen::Vector3f::Constant(grid_size_);
    
    // Compute grid dimensions
    Eigen::Vector3f extent = max_bounds - min_bounds;
    grid_dims = (extent / grid_size_).cast<int>() + Eigen::Vector3i::Ones();
    
    // Initialize density field
    size_t num_voxels = grid_dims.x() * grid_dims.y() * grid_dims.z();
    std::vector<float> density_field(num_voxels, 0.0f);
    
    // Compute density for each voxel
    for (int x = 0; x < grid_dims.x(); ++x) {
        for (int y = 0; y < grid_dims.y(); ++y) {
            for (int z = 0; z < grid_dims.z(); ++z) {
                Eigen::Vector3i grid_index(x, y, z);
                Eigen::Vector3f point = getGridCenter(grid_index, min_bounds);
                
                float density = computeDensityAtPoint(gaussians, point);
                size_t linear_index = getLinearIndex(grid_index, grid_dims);
                density_field[linear_index] = density;
            }
        }
    }
    
    // Compute statistics
    computeStatistics(density_field);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    float duration_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    std::cout << "Density field computed in " << duration_ms << " ms" << std::endl;
    std::cout << "Grid dimensions: " << grid_dims.x() << " x " << grid_dims.y() << " x " << grid_dims.z() << std::endl;
    std::cout << "Number of voxels: " << num_voxels << std::endl;
    
    return density_field;
}

float DensityAnalyzer::computeDensityAtPoint(const std::vector<Gaussian>& gaussians, const Eigen::Vector3f& point) {
    float density = 0.0f;
    
    for (const auto& gaussian : gaussians) {
        const auto& pos = gaussian.getPosition();
        float opacity = gaussian.getOpacity();
        
        // Compute distance from point to gaussian center
        float dist = (point - pos).norm();
        
        // Compute density contribution using Gaussian falloff
        float scale = gaussian.getScale().mean();
        float falloff = std::exp(-0.5f * (dist * dist) / (scale * scale));
        density += opacity * falloff;
    }
    
    return density;
}

std::vector<Eigen::Vector3f> DensityAnalyzer::findDenseRegions(
    const std::vector<float>& density_field,
    const Eigen::Vector3f& min_bounds,
    const Eigen::Vector3i& grid_dims,
    float density_threshold
) {
    std::vector<Eigen::Vector3f> dense_regions;
    
    for (int x = 0; x < grid_dims.x(); ++x) {
        for (int y = 0; y < grid_dims.y(); ++y) {
            for (int z = 0; z < grid_dims.z(); ++z) {
                Eigen::Vector3i grid_index(x, y, z);
                size_t linear_index = getLinearIndex(grid_index, grid_dims);
                
                if (density_field[linear_index] >= density_threshold) {
                    Eigen::Vector3f region_center = getGridCenter(grid_index, min_bounds);
                    dense_regions.push_back(region_center);
                }
            }
        }
    }
    
    std::cout << "Found " << dense_regions.size() << " dense regions above threshold " << density_threshold << std::endl;
    
    return dense_regions;
}

Eigen::Vector3i DensityAnalyzer::getGridIndex(const Eigen::Vector3f& point, const Eigen::Vector3f& min_bounds) const {
    Eigen::Vector3f relative_pos = point - min_bounds;
    return (relative_pos / grid_size_).cast<int>();
}

size_t DensityAnalyzer::getLinearIndex(const Eigen::Vector3i& grid_index, const Eigen::Vector3i& grid_dims) const {
    return grid_index.x() + grid_index.y() * grid_dims.x() + grid_index.z() * grid_dims.x() * grid_dims.y();
}

Eigen::Vector3f DensityAnalyzer::getGridCenter(const Eigen::Vector3i& grid_index, const Eigen::Vector3f& min_bounds) const {
    return min_bounds + (grid_index.cast<float>() + Eigen::Vector3f::Constant(0.5f)) * grid_size_;
}

void DensityAnalyzer::computeStatistics(const std::vector<float>& density_field) {
    if (density_field.empty()) {
        stats_.min_density = 0.0f;
        stats_.max_density = 0.0f;
        stats_.mean_density = 0.0f;
        stats_.median_density = 0.0f;
        stats_.std_dev_density = 0.0f;
        return;
    }
    
    // Compute min, max, mean
    stats_.min_density = *std::min_element(density_field.begin(), density_field.end());
    stats_.max_density = *std::max_element(density_field.begin(), density_field.end());
    
    float sum = 0.0f;
    for (float density : density_field) {
        sum += density;
    }
    stats_.mean_density = sum / density_field.size();
    
    // Compute median
    std::vector<float> sorted_densities = density_field;
    std::sort(sorted_densities.begin(), sorted_densities.end());
    size_t mid = sorted_densities.size() / 2;
    if (sorted_densities.size() % 2 == 0) {
        stats_.median_density = (sorted_densities[mid - 1] + sorted_densities[mid]) / 2.0f;
    } else {
        stats_.median_density = sorted_densities[mid];
    }
    
    // Compute standard deviation
    float sum_sq = 0.0f;
    for (float density : density_field) {
        float diff = density - stats_.mean_density;
        sum_sq += diff * diff;
    }
    stats_.std_dev_density = std::sqrt(sum_sq / density_field.size());
    
    std::cout << "Density statistics:" << std::endl;
    std::cout << "  Min density: " << stats_.min_density << std::endl;
    std::cout << "  Max density: " << stats_.max_density << std::endl;
    std::cout << "  Mean density: " << stats_.mean_density << std::endl;
    std::cout << "  Median density: " << stats_.median_density << std::endl;
    std::cout << "  Std dev density: " << stats_.std_dev_density << std::endl;
}

} // namespace AmeScanner
