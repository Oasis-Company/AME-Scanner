#include "field_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <Eigen/Geometry>

namespace AmeScanner {

bool FieldLoader::loadFromPLY(const std::string& file_path, std::vector<Gaussian>& gaussians) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open PLY file: " << file_path << std::endl;
        return false;
    }
    
    uint32_t num_vertices = 0;
    if (!parsePLYHeader(file, num_vertices)) {
        std::cerr << "Failed to parse PLY header" << std::endl;
        return false;
    }
    
    gaussians.reserve(num_vertices);
    
    // Initialize statistics
    stats.num_gaussians = 0;
    stats.min_x = stats.min_y = stats.min_z = std::numeric_limits<float>::max();
    stats.max_x = stats.max_y = stats.max_z = std::numeric_limits<float>::lowest();
    
    for (uint32_t i = 0; i < num_vertices; ++i) {
        Gaussian gaussian;
        if (!parsePLYVertex(file, gaussian)) {
            std::cerr << "Failed to parse PLY vertex " << i << std::endl;
            return false;
        }
        gaussians.push_back(gaussian);
        
        // Update statistics
        const auto& pos = gaussian.getPosition();
        stats.min_x = std::min(stats.min_x, pos.x());
        stats.max_x = std::max(stats.max_x, pos.x());
        stats.min_y = std::min(stats.min_y, pos.y());
        stats.max_y = std::max(stats.max_y, pos.y());
        stats.min_z = std::min(stats.min_z, pos.z());
        stats.max_z = std::max(stats.max_z, pos.z());
        stats.num_gaussians++;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats.loading_time_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    std::cout << "Loaded " << stats.num_gaussians << " gaussians from PLY file in " << stats.loading_time_ms << " ms" << std::endl;
    return true;
}

bool FieldLoader::loadFromSPLAT(const std::string& file_path, std::vector<Gaussian>& gaussians) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open SPLAT file: " << file_path << std::endl;
        return false;
    }
    
    uint32_t num_gaussians = 0;
    if (!parseSPLATHeader(file, num_gaussians)) {
        std::cerr << "Failed to parse SPLAT header" << std::endl;
        return false;
    }
    
    gaussians.reserve(num_gaussians);
    
    // Initialize statistics
    stats.num_gaussians = 0;
    stats.min_x = stats.min_y = stats.min_z = std::numeric_limits<float>::max();
    stats.max_x = stats.max_y = stats.max_z = std::numeric_limits<float>::lowest();
    
    for (uint32_t i = 0; i < num_gaussians; ++i) {
        Gaussian gaussian;
        if (!parseSPLATGaussian(file, gaussian)) {
            std::cerr << "Failed to parse SPLAT gaussian " << i << std::endl;
            return false;
        }
        gaussians.push_back(gaussian);
        
        // Update statistics
        const auto& pos = gaussian.getPosition();
        stats.min_x = std::min(stats.min_x, pos.x());
        stats.max_x = std::max(stats.max_x, pos.x());
        stats.min_y = std::min(stats.min_y, pos.y());
        stats.max_y = std::max(stats.max_y, pos.y());
        stats.min_z = std::min(stats.min_z, pos.z());
        stats.max_z = std::max(stats.max_z, pos.z());
        stats.num_gaussians++;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats.loading_time_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    std::cout << "Loaded " << stats.num_gaussians << " gaussians from SPLAT file in " << stats.loading_time_ms << " ms" << std::endl;
    return true;
}

bool FieldLoader::saveToPLY(const std::string& file_path, const std::vector<Gaussian>& gaussians) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open PLY file for writing: " << file_path << std::endl;
        return false;
    }
    
    // Write header
    file << "ply" << std::endl;
    file << "format ascii 1.0" << std::endl;
    file << "element vertex " << gaussians.size() << std::endl;
    file << "property float x" << std::endl;
    file << "property float y" << std::endl;
    file << "property float z" << std::endl;
    file << "property float nx" << std::endl;
    file << "property float ny" << std::endl;
    file << "property float nz" << std::endl;
    file << "property uchar red" << std::endl;
    file << "property uchar green" << std::endl;
    file << "property uchar blue" << std::endl;
    file << "property float scale" << std::endl;
    file << "property float rotation_x" << std::endl;
    file << "property float rotation_y" << std::endl;
    file << "property float rotation_z" << std::endl;
    file << "property float opacity" << std::endl;
    file << "end_header" << std::endl;
    
    // Write vertices
    for (const auto& gaussian : gaussians) {
        const auto& pos = gaussian.getPosition();
        const auto& color = gaussian.getColor();
        const auto& scale = gaussian.getScale();
        const auto& rotation = gaussian.getRotation();
        float opacity = gaussian.getOpacity();
        
        file << pos.x() << " " << pos.y() << " " << pos.z() << " ";
        file << "0 0 0 ";
        file << static_cast<int>(color.x() * 255) << " " << static_cast<int>(color.y() * 255) << " " << static_cast<int>(color.z() * 255) << " ";
        file << scale.x() << " ";
        file << rotation.x() << " " << rotation.y() << " " << rotation.z() << " ";
        file << opacity << std::endl;
    }
    
    return true;
}

bool FieldLoader::saveToSPLAT(const std::string& file_path, const std::vector<Gaussian>& gaussians) const {
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open SPLAT file for writing: " << file_path << std::endl;
        return false;
    }
    
    // Write header
    uint32_t num_gaussians = gaussians.size();
    file.write(reinterpret_cast<const char*>(&num_gaussians), sizeof(num_gaussians));
    
    // Write gaussians
    for (const auto& gaussian : gaussians) {
        const auto& pos = gaussian.getPosition();
        const auto& color = gaussian.getColor();
        const auto& scale = gaussian.getScale();
        const auto& rotation = gaussian.getRotation();
        float opacity = gaussian.getOpacity();
        
        file.write(reinterpret_cast<const char*>(pos.data()), sizeof(float) * 3);
        file.write(reinterpret_cast<const char*>(color.data()), sizeof(float) * 3);
        file.write(reinterpret_cast<const char*>(&opacity), sizeof(float));
        file.write(reinterpret_cast<const char*>(scale.data()), sizeof(float) * 3);
        file.write(reinterpret_cast<const char*>(rotation.coeffs().data()), sizeof(float) * 4);
    }
    
    return true;
}

bool FieldLoader::parsePLYHeader(std::ifstream& file, uint32_t& num_vertices) {
    std::string line;
    
    // Check magic number
    if (!std::getline(file, line) || line != "ply") {
        return false;
    }
    
    // Read format
    if (!std::getline(file, line) || line != "format ascii 1.0") {
        return false;
    }
    
    // Find element vertex
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        if (token == "element" && iss >> token && token == "vertex") {
            iss >> num_vertices;
            break;
        }
        
        if (token == "end_header") {
            break;
        }
    }
    
    // Skip to end of header
    while (std::getline(file, line)) {
        if (line == "end_header") {
            break;
        }
    }
    
    return true;
}

bool FieldLoader::parsePLYVertex(std::ifstream& file, Gaussian& gaussian) {
    std::string line;
    if (!std::getline(file, line)) {
        return false;
    }
    
    std::istringstream iss(line);
    float x, y, z, nx, ny, nz, scale, rx, ry, rz, opacity;
    int r, g, b;
    
    if (!(iss >> x >> y >> z >> nx >> ny >> nz >> r >> g >> b >> scale >> rx >> ry >> rz >> opacity)) {
        return false;
    }
    
    Eigen::Vector3f position(x, y, z);
    Eigen::Vector3f color(r / 255.0f, g / 255.0f, b / 255.0f);
    Eigen::Vector3f scale_vec(scale, scale, scale);
    Eigen::Quaternionf rotation = Eigen::Quaternionf::UnitRandom();
    
    gaussian = Gaussian(position, color, opacity, scale_vec, rotation);
    return true;
}

bool FieldLoader::parseSPLATHeader(std::ifstream& file, uint32_t& num_gaussians) {
    file.read(reinterpret_cast<char*>(&num_gaussians), sizeof(num_gaussians));
    return file.good();
}

bool FieldLoader::parseSPLATGaussian(std::ifstream& file, Gaussian& gaussian) {
    float pos[3], color[3], opacity, scale[3], rotation[4];
    
    file.read(reinterpret_cast<char*>(pos), sizeof(float) * 3);
    file.read(reinterpret_cast<char*>(color), sizeof(float) * 3);
    file.read(reinterpret_cast<char*>(&opacity), sizeof(float));
    file.read(reinterpret_cast<char*>(scale), sizeof(float) * 3);
    file.read(reinterpret_cast<char*>(rotation), sizeof(float) * 4);
    
    if (!file.good()) {
        return false;
    }
    
    Eigen::Vector3f position(pos[0], pos[1], pos[2]);
    Eigen::Vector3f color_vec(color[0], color[1], color[2]);
    Eigen::Vector3f scale_vec(scale[0], scale[1], scale[2]);
    Eigen::Quaternionf rotation_quat(rotation[0], rotation[1], rotation[2], rotation[3]);
    
    gaussian = Gaussian(position, color_vec, opacity, scale_vec, rotation_quat);
    return true;
}

} // namespace AmeScanner
