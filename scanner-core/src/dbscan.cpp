#include "dbscan.h"
#include <iostream>
#include <chrono>

namespace AmeScanner {

std::vector<std::vector<size_t>> DBSCAN::cluster(const std::vector<Gaussian>& gaussians) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    size_t n = gaussians.size();
    labels_.resize(n, -1); // -1 means unvisited
    num_clusters_ = 0;
    num_noise_ = 0;
    
    for (size_t i = 0; i < n; ++i) {
        if (labels_[i] != -1) {
            continue; // Already visited
        }
        
        std::vector<size_t> neighbors = regionQuery(gaussians, i);
        
        if (neighbors.size() < static_cast<size_t>(min_pts_)) {
            labels_[i] = -2; // Mark as noise
            num_noise_++;
            continue;
        }
        
        // Expand cluster
        labels_[i] = num_clusters_;
        
        // Remove self from neighbors
        for (auto it = neighbors.begin(); it != neighbors.end(); ) {
            if (*it == i) {
                it = neighbors.erase(it);
            } else {
                ++it;
            }
        }
        
        expandCluster(gaussians, i, neighbors, num_clusters_);
        num_clusters_++;
    }
    
    // Generate cluster indices
    std::vector<std::vector<size_t>> clusters(num_clusters_);
    for (size_t i = 0; i < n; ++i) {
        if (labels_[i] >= 0) {
            clusters[labels_[i]].push_back(i);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    float duration_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    std::cout << "DBSCAN clustering completed in " << duration_ms << " ms" << std::endl;
    std::cout << "Found " << num_clusters_ << " clusters" << std::endl;
    std::cout << "Found " << num_noise_ << " noise points" << std::endl;
    
    return clusters;
}

std::vector<size_t> DBSCAN::regionQuery(const std::vector<Gaussian>& gaussians, size_t point_idx) {
    std::vector<size_t> neighbors;
    const Gaussian& point = gaussians[point_idx];
    
    for (size_t i = 0; i < gaussians.size(); ++i) {
        if (i == point_idx) {
            continue;
        }
        
        float dist = computeDistance(point, gaussians[i]);
        if (dist <= eps_) {
            neighbors.push_back(i);
        }
    }
    
    return neighbors;
}

void DBSCAN::expandCluster(const std::vector<Gaussian>& gaussians, size_t point_idx, std::vector<size_t>& neighbors, int cluster_id) {
    while (!neighbors.empty()) {
        size_t current_idx = neighbors.back();
        neighbors.pop_back();
        
        if (labels_[current_idx] == -2) {
            labels_[current_idx] = cluster_id; // Change noise to border point
            num_noise_--;
        }
        
        if (labels_[current_idx] != -1) {
            continue; // Already visited
        }
        
        labels_[current_idx] = cluster_id;
        
        std::vector<size_t> current_neighbors = regionQuery(gaussians, current_idx);
        if (current_neighbors.size() >= static_cast<size_t>(min_pts_)) {
            // Merge neighbors
            for (size_t neighbor_idx : current_neighbors) {
                if (labels_[neighbor_idx] == -1) {
                    neighbors.push_back(neighbor_idx);
                }
            }
        }
    }
}

float DBSCAN::computeDistance(const Gaussian& a, const Gaussian& b) {
    // Use Euclidean distance between positions
    Eigen::Vector3f diff = a.getPosition() - b.getPosition();
    return diff.norm();
}

} // namespace AmeScanner
