#include <iostream>
#include <vector>
#include "field_loader.h"

void test_ply_loading() {
    std::cout << "Testing .ply file loading..." << std::endl;
    
    AmeScanner::FieldLoader loader;
    std::vector<AmeScanner::Gaussian> gaussians;
    
    // Test random_gaussians.ply
    std::cout << "Loading random_gaussians.ply..." << std::endl;
    bool success = loader.loadFromPLY("test-preparation/test-data/random_gaussians.ply", gaussians);
    if (success) {
        std::cout << "✓ Successfully loaded " << gaussians.size() << " gaussians from random_gaussians.ply" << std::endl;
    } else {
        std::cout << "✗ Failed to load random_gaussians.ply" << std::endl;
        return;
    }
    
    // Test clustered_gaussians.ply
    gaussians.clear();
    std::cout << "Loading clustered_gaussians.ply..." << std::endl;
    success = loader.loadFromPLY("test-preparation/test-data/clustered_gaussians.ply", gaussians);
    if (success) {
        std::cout << "✓ Successfully loaded " << gaussians.size() << " gaussians from clustered_gaussians.ply" << std::endl;
    } else {
        std::cout << "✗ Failed to load clustered_gaussians.ply" << std::endl;
        return;
    }
    
    // Test large_gaussians.ply
    gaussians.clear();
    std::cout << "Loading large_gaussians.ply..." << std::endl;
    success = loader.loadFromPLY("test-preparation/test-data/large_gaussians.ply", gaussians);
    if (success) {
        std::cout << "✓ Successfully loaded " << gaussians.size() << " gaussians from large_gaussians.ply" << std::endl;
    } else {
        std::cout << "✗ Failed to load large_gaussians.ply" << std::endl;
        return;
    }
}

void test_gaussian_parameters() {
    std::cout << "\nTesting Gaussian parameters..." << std::endl;
    
    AmeScanner::FieldLoader loader;
    std::vector<AmeScanner::Gaussian> gaussians;
    
    bool success = loader.loadFromPLY("test-preparation/test-data/random_gaussians.ply", gaussians);
    if (!success) {
        std::cout << "✗ Failed to load random_gaussians.ply for parameter testing" << std::endl;
        return;
    }
    
    // Check that we have gaussians
    if (gaussians.empty()) {
        std::cout << "✗ No gaussians loaded" << std::endl;
        return;
    }
    
    // Check parameters of the first Gaussian
    const auto& gaussian = gaussians[0];
    auto position = gaussian.getPosition();
    auto color = gaussian.getColor();
    float opacity = gaussian.getOpacity();
    auto scale = gaussian.getScale();
    auto rotation = gaussian.getRotation();
    
    std::cout << "✓ First Gaussian parameters:" << std::endl;
    std::cout << "  Position: (" << position.x() << ", " << position.y() << ", " << position.z() << ")" << std::endl;
    std::cout << "  Color: (" << color.x() << ", " << color.y() << ", " << color.z() << ")" << std::endl;
    std::cout << "  Opacity: " << opacity << std::endl;
    std::cout << "  Scale: (" << scale.x() << ", " << scale.y() << ", " << scale.z() << ")" << std::endl;
    std::cout << "  Rotation: (" << rotation.x() << ", " << rotation.y() << ", " << rotation.z() << ", " << rotation.w() << ")" << std::endl;
    
    // Check that parameters are within reasonable ranges
    bool position_valid = (-5.0f <= position.x() && position.x() <= 5.0f) &&
                         (-5.0f <= position.y() && position.y() <= 5.0f) &&
                         (-5.0f <= position.z() && position.z() <= 5.0f);
    
    bool color_valid = (0.0f <= color.x() && color.x() <= 1.0f) &&
                       (0.0f <= color.y() && color.y() <= 1.0f) &&
                       (0.0f <= color.z() && color.z() <= 1.0f);
    
    bool opacity_valid = (0.2f <= opacity && opacity <= 1.0f);
    
    bool scale_valid = (0.1f <= scale.x() && scale.x() <= 0.6f) &&
                       (0.1f <= scale.y() && scale.y() <= 0.6f) &&
                       (0.1f <= scale.z() && scale.z() <= 0.6f);
    
    if (position_valid && color_valid && opacity_valid && scale_valid) {
        std::cout << "✓ All Gaussian parameters within expected ranges" << std::endl;
    } else {
        std::cout << "✗ Some parameters out of expected ranges" << std::endl;
        if (!position_valid) std::cout << "  - Position out of range" << std::endl;
        if (!color_valid) std::cout << "  - Color out of range" << std::endl;
        if (!opacity_valid) std::cout << "  - Opacity out of range" << std::endl;
        if (!scale_valid) std::cout << "  - Scale out of range" << std::endl;
    }
}

int main() {
    std::cout << "=== 3DGS Data Loading Test ===" << std::endl;
    
    // Test PLY loading
    test_ply_loading();
    
    // Test Gaussian parameters
    test_gaussian_parameters();
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}
