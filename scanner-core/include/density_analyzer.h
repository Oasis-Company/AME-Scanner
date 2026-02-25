#pragma once

#include <vector>
#include <Eigen/Core>
#include "gaussian.h"

namespace AmeScanner {

class DensityAnalyzer {
public:
    DensityAnalyzer(float grid_size = 0.1f) : grid_size_(grid_size) {}
    
    // Set grid size for density analysis
    void setGridSize(float grid_size) { grid_size_ = grid_size; }
    
    // Compute density field from gaussians
    std::vector<float> computeDensityField(
        const std::vector<Gaussian>& gaussians,
        Eigen::Vector3f& min_bounds,
        Eigen::Vector3f& max_bounds,
        Eigen::Vector3i& grid_dims
    );
    
    // Compute density at a specific point
    float computeDensityAtPoint(const std::vector<Gaussian>& gaussians, const Eigen::Vector3f& point);
    
    // Find dense regions in the density field
    std::vector<Eigen::Vector3f> findDenseRegions(
        const std::vector<float>& density_field,
        const Eigen::Vector3f& min_bounds,
        const Eigen::Vector3i& grid_dims,
        float density_threshold
    );
    
    // Get density statistics
    struct Statistics {
        float min_density;
        float max_density;
        float mean_density;
        float median_density;
        float std_dev_density;
    };
    
    const Statistics& getStatistics() const { return stats_; }
    
private:
    float grid_size_;
    Statistics stats_;
    
    // Helper methods
    Eigen::Vector3i getGridIndex(const Eigen::Vector3f& point, const Eigen::Vector3f& min_bounds) const;
    size_t getLinearIndex(const Eigen::Vector3i& grid_index, const Eigen::Vector3i& grid_dims) const;
    Eigen::Vector3f getGridCenter(const Eigen::Vector3i& grid_index, const Eigen::Vector3f& min_bounds) const;
    void computeStatistics(const std::vector<float>& density_field);
};

} // namespace AmeScanner
