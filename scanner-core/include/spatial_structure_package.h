#pragma once

#include <vector>
#include <string>
#include <Eigen/Core>

namespace AmeScanner {

struct OBB {
    Eigen::Vector3f center;
    Eigen::Matrix3f rotation;
    Eigen::Vector3f extents;
};

struct AmeEntity {
    uint32_t id;
    OBB obb;
    std::string mesh_path;
    std::string metaclass;
    uint32_t physics_handle;
};

struct SpatialRelationship {
    uint32_t source_id;
    uint32_t target_id;
    std::string relationship_type;
    float confidence;
};

struct SpatialStructurePackage {
    OBB scene_bbox;
    std::vector<AmeEntity> entities;
    std::vector<SpatialRelationship> relationships;
    
    struct Metadata {
        std::string version;
        std::string timestamp;
        uint32_t num_entities;
        uint32_t num_relationships;
        float processing_time_ms;
    } metadata;
    
    // Serialization methods
    bool serialize(const std::string& file_path) const;
    bool deserialize(const std::string& file_path);
};

} // namespace AmeScanner
