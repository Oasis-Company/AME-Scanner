#pragma once

#include <vector>
#include <Eigen/Core>
#include "gaussian.h"

namespace AmeScanner {

class DBSCAN {
public:
    DBSCAN(float eps = 0.1f, int min_pts = 5) : eps_(eps), min_pts_(min_pts) {}
    
    // Set parameters
    void setEpsilon(float eps) { eps_ = eps; }
    void setMinPoints(int min_pts) { min_pts_ = min_pts; }
    
    // Cluster gaussians
    std::vector<std::vector<size_t>> cluster(const std::vector<Gaussian>& gaussians);
    
    // Get cluster labels
    const std::vector<int>& getLabels() const { return labels_; }
    
    // Get number of clusters
    int getNumClusters() const { return num_clusters_; }
    
    // Get number of noise points
    int getNumNoise() const { return num_noise_; }
    
private:
    float eps_;          // Maximum distance between two points to be considered neighbors
    int min_pts_;        // Minimum number of points required to form a cluster
    
    std::vector<int> labels_;    // Cluster labels for each point
    int num_clusters_;           // Number of clusters found
    int num_noise_;              // Number of noise points
    
    // Helper methods
    std::vector<size_t> regionQuery(const std::vector<Gaussian>& gaussians, size_t point_idx);
    void expandCluster(const std::vector<Gaussian>& gaussians, size_t point_idx, std::vector<size_t>& neighbors, int cluster_id);
    float computeDistance(const Gaussian& a, const Gaussian& b);
};

} // namespace AmeScanner
