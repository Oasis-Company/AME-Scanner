#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include "field_loader.h"
#include "dbscan.h"
#include "spatial_structure_package.h"

void printHelp() {
    std::cout << "AME Scanner CLI Tool" << std::endl;
    std::cout << "Usage: scanner-cli [options] input_file output_file" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -e, --epsilon FLOAT     DBSCAN epsilon parameter (default: 0.1)" << std::endl;
    std::cout << "  -m, --min-pts INT       DBSCAN min points parameter (default: 5)" << std::endl;
    std::cout << "  -f, --format FORMAT     Output format (default: ssp)" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << std::endl;
    std::cout << "Input formats supported: .ply, .splat" << std::endl;
    std::cout << "Output formats supported: .ssp (Spatial Structure Package)" << std::endl;
}

int main(int argc, char** argv) {
    float epsilon = 0.1f;
    int min_pts = 5;
    std::string format = "ssp";
    bool verbose = false;
    
    // Parse command line arguments
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"epsilon", required_argument, 0, 'e'},
        {"min-pts", required_argument, 0, 'm'},
        {"format", required_argument, 0, 'f'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "he:m:f:v", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'h':
            printHelp();
            return 0;
        case 'e':
            epsilon = std::stof(optarg);
            break;
        case 'm':
            min_pts = std::stoi(optarg);
            break;
        case 'f':
            format = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        default:
            printHelp();
            return 1;
        }
    }
    
    // Check if input and output files are provided
    if (optind + 2 > argc) {
        std::cerr << "Error: Missing input or output file" << std::endl;
        printHelp();
        return 1;
    }
    
    std::string input_file = argv[optind];
    std::string output_file = argv[optind + 1];
    
    if (verbose) {
        std::cout << "AME Scanner CLI Tool" << std::endl;
        std::cout << "Input file: " << input_file << std::endl;
        std::cout << "Output file: " << output_file << std::endl;
        std::cout << "DBSCAN epsilon: " << epsilon << std::endl;
        std::cout << "DBSCAN min points: " << min_pts << std::endl;
        std::cout << "Output format: " << format << std::endl;
        std::cout << std::endl;
    }
    
    // Load 3DGS data
    AmeScanner::FieldLoader loader;
    std::vector<AmeScanner::Gaussian> gaussians;
    
    if (input_file.substr(input_file.size() - 4) == ".ply") {
        if (!loader.loadFromPLY(input_file, gaussians)) {
            std::cerr << "Error: Failed to load PLY file" << std::endl;
            return 1;
        }
    } else if (input_file.substr(input_file.size() - 6) == ".splat") {
        if (!loader.loadFromSPLAT(input_file, gaussians)) {
            std::cerr << "Error: Failed to load SPLAT file" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Unsupported input file format" << std::endl;
        return 1;
    }
    
    if (verbose) {
        const auto& stats = loader.getStatistics();
        std::cout << "Loaded " << stats.num_gaussians << " gaussians" << std::endl;
        std::cout << "Scene bounds: " << std::endl;
        std::cout << "  X: [" << stats.min_x << ", " << stats.max_x << "]" << std::endl;
        std::cout << "  Y: [" << stats.min_y << ", " << stats.max_y << "]" << std::endl;
        std::cout << "  Z: [" << stats.min_z << ", " << stats.max_z << "]" << std::endl;
        std::cout << std::endl;
    }
    
    // Cluster gaussians using DBSCAN
    AmeScanner::DBSCAN dbscan(epsilon, min_pts);
    auto clusters = dbscan.cluster(gaussians);
    
    if (verbose) {
        std::cout << "Found " << clusters.size() << " clusters" << std::endl;
        std::cout << "Cluster sizes:" << std::endl;
        for (size_t i = 0; i < clusters.size(); ++i) {
            std::cout << "  Cluster " << i << ": " << clusters[i].size() << " points" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Create SpatialStructurePackage
    AmeScanner::SpatialStructurePackage ssp;
    
    // Set metadata
    ssp.metadata.version = "1.0";
    ssp.metadata.timestamp = "2024-01-01T00:00:00";
    ssp.metadata.num_entities = clusters.size();
    ssp.metadata.num_relationships = 0;
    ssp.metadata.processing_time_ms = 0.0f;
    
    // Create entities from clusters
    for (size_t i = 0; i < clusters.size(); ++i) {
        AmeScanner::AmeEntity entity;
        entity.id = i;
        entity.physics_handle = i;
        entity.metaclass = "unknown";
        
        // Compute OBB for cluster
        // TODO: Implement PCA-based OBB fitting
        AmeScanner::OBB obb;
        obb.center = Eigen::Vector3f::Zero();
        obb.rotation = Eigen::Matrix3f::Identity();
        obb.extents = Eigen::Vector3f::Ones();
        entity.obb = obb;
        
        ssp.entities.push_back(entity);
    }
    
    // Serialize SpatialStructurePackage
    if (!ssp.serialize(output_file)) {
        std::cerr << "Error: Failed to save Spatial Structure Package" << std::endl;
        return 1;
    }
    
    if (verbose) {
        std::cout << "Successfully saved Spatial Structure Package to " << output_file << std::endl;
        std::cout << "Number of entities: " << ssp.entities.size() << std::endl;
        std::cout << "Number of relationships: " << ssp.relationships.size() << std::endl;
    }
    
    std::cout << "AME Scanner CLI Tool completed successfully!" << std::endl;
    
    return 0;
}
