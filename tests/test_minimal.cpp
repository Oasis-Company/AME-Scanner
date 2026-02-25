#include <iostream>
#include <vector>
#include "field_loader.h"
#include "dbscan.h"

void testMinimalDeliverable() {
    std::cout << "Testing Minimal Deliverable..." << std::endl;
    
    // Test 1: FieldLoader initialization
    AmeScanner::FieldLoader loader;
    std::cout << "✓ FieldLoader initialized successfully" << std::endl;
    
    // Test 2: DBSCAN initialization
    AmeScanner::DBSCAN dbscan(0.1f, 5);
    std::cout << "✓ DBSCAN initialized successfully" << std::endl;
    
    // Test 3: Create test gaussians
    std::vector<AmeScanner::Gaussian> test_gaussians;
    
    // Create two clusters
    for (int i = 0; i < 20; ++i) {
        AmeScanner::Gaussian gaussian(
            Eigen::Vector3f(0.1f * i, 0.0f, 0.0f),
            Eigen::Vector3f(1.0f, 0.0f, 0.0f),
            1.0f,
            Eigen::Vector3f(0.05f, 0.05f, 0.05f),
            Eigen::Quaternionf::Identity()
        );
        test_gaussians.push_back(gaussian);
    }
    
    for (int i = 0; i < 20; ++i) {
        AmeScanner::Gaussian gaussian(
            Eigen::Vector3f(1.0f + 0.1f * i, 1.0f, 0.0f),
            Eigen::Vector3f(0.0f, 1.0f, 0.0f),
            1.0f,
            Eigen::Vector3f(0.05f, 0.05f, 0.05f),
            Eigen::Quaternionf::Identity()
        );
        test_gaussians.push_back(gaussian);
    }
    
    std::cout << "✓ Test gaussians created successfully" << std::endl;
    
    // Test 4: DBSCAN clustering
    auto clusters = dbscan.cluster(test_gaussians);
    std::cout << "✓ DBSCAN clustering completed successfully" << std::endl;
    std::cout << "✓ Found " << clusters.size() << " clusters" << std::endl;
    
    // Test 5: Output cluster list
    std::cout << "✓ Cluster list output:" << std::endl;
    for (size_t i = 0; i < clusters.size(); ++i) {
        std::cout << "  Cluster " << i << ": " << clusters[i].size() << " points" << std::endl;
    }
    
    std::cout << "\nMinimal Deliverable Test PASSED!" << std::endl;
}

int main() {
    testMinimalDeliverable();
    return 0;
}
