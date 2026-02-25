#pragma once

#include <vector>
#include <string>
#include "gaussian.h"

namespace AmeScanner {

class FieldLoader {
public:
    FieldLoader() = default;
    
    // Load 3DGS from .ply file
    bool loadFromPLY(const std::string& file_path, std::vector<Gaussian>& gaussians);
    
    // Load 3DGS from .splat file
    bool loadFromSPLAT(const std::string& file_path, std::vector<Gaussian>& gaussians);
    
    // Save 3DGS to .ply file
    bool saveToPLY(const std::string& file_path, const std::vector<Gaussian>& gaussians) const;
    
    // Save 3DGS to .splat file
    bool saveToSPLAT(const std::string& file_path, const std::vector<Gaussian>& gaussians) const;
    
    // Get loading statistics
    struct Statistics {
        uint32_t num_gaussians;
        float loading_time_ms;
        float min_x, max_x;
        float min_y, max_y;
        float min_z, max_z;
    };
    
    const Statistics& getStatistics() const {
        return stats;
    }
    
private:
    Statistics stats;
    
    // Helper methods
    bool parsePLYHeader(std::ifstream& file, uint32_t& num_vertices);
    bool parsePLYVertex(std::ifstream& file, Gaussian& gaussian);
    bool parseSPLATHeader(std::ifstream& file, uint32_t& num_gaussians);
    bool parseSPLATGaussian(std::ifstream& file, Gaussian& gaussian);
};

} // namespace AmeScanner
