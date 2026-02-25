#pragma once

#include <vector>
#include <string>
#include <Eigen/Core>
#include "spatial_structure_package.h"

namespace AmeScanner {

class SpatialGraph {
public:
    SpatialGraph() = default;
    
    // Build spatial relationship graph from entities
    void buildGraph(const std::vector<AmeEntity>& entities);
    
    // Get spatial relationships
    const std::vector<SpatialRelationship>& getRelationships() const {
        return relationships_;
    }
    
    // Get relationship types
    static std::vector<std::string> getRelationshipTypes() {
        return {
            "contains",
            "contained_by",
            "above",
            "below",
            "left_of",
            "right_of",
            "front_of",
            "behind",
            "adjacent_to"
        };
    }
    
    // Compute relationship between two entities
    SpatialRelationship computeRelationship(const AmeEntity& entity1, const AmeEntity& entity2);
    
    // Visualize the graph (optional)
    bool visualize(const std::string& output_file) const;
    
private:
    std::vector<SpatialRelationship> relationships_;
    
    // Helper methods
    bool contains(const AmeEntity& container, const AmeEntity& contained);
    bool isAbove(const AmeEntity& entity1, const AmeEntity& entity2);
    bool isLeftOf(const AmeEntity& entity1, const AmeEntity& entity2);
    bool isFrontOf(const AmeEntity& entity1, const AmeEntity& entity2);
    bool isAdjacent(const AmeEntity& entity1, const AmeEntity& entity2);
    
    // Compute distance between two OBBs
    float computeDistance(const OBB& obb1, const OBB& obb2);
    
    // Check if two OBBs intersect
    bool intersects(const OBB& obb1, const OBB& obb2);
};

} // namespace AmeScanner
