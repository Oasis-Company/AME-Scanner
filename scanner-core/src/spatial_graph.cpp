#include "spatial_graph.h"
#include <iostream>
#include <chrono>
#include <fstream>

namespace AmeScanner {

void SpatialGraph::buildGraph(const std::vector<AmeEntity>& entities) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    relationships_.clear();
    
    size_t n = entities.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == j) {
                continue;
            }
            
            SpatialRelationship relationship = computeRelationship(entities[i], entities[j]);
            if (!relationship.relationship_type.empty()) {
                relationships_.push_back(relationship);
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    float duration_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    std::cout << "Spatial graph built in " << duration_ms << " ms" << std::endl;
    std::cout << "Found " << relationships_.size() << " spatial relationships" << std::endl;
}

SpatialRelationship SpatialGraph::computeRelationship(const AmeEntity& entity1, const AmeEntity& entity2) {
    SpatialRelationship relationship;
    relationship.source_id = entity1.id;
    relationship.target_id = entity2.id;
    relationship.confidence = 0.0f;
    
    // Check containment
    if (contains(entity1, entity2)) {
        relationship.relationship_type = "contains";
        relationship.confidence = 0.9f;
        return relationship;
    }
    
    if (contains(entity2, entity1)) {
        relationship.relationship_type = "contained_by";
        relationship.confidence = 0.9f;
        return relationship;
    }
    
    // Check adjacency
    if (isAdjacent(entity1, entity2)) {
        relationship.relationship_type = "adjacent_to";
        relationship.confidence = 0.8f;
        return relationship;
    }
    
    // Check relative positions
    if (isAbove(entity1, entity2)) {
        relationship.relationship_type = "above";
        relationship.confidence = 0.7f;
        return relationship;
    }
    
    if (isAbove(entity2, entity1)) {
        relationship.relationship_type = "below";
        relationship.confidence = 0.7f;
        return relationship;
    }
    
    if (isLeftOf(entity1, entity2)) {
        relationship.relationship_type = "left_of";
        relationship.confidence = 0.6f;
        return relationship;
    }
    
    if (isLeftOf(entity2, entity1)) {
        relationship.relationship_type = "right_of";
        relationship.confidence = 0.6f;
        return relationship;
    }
    
    if (isFrontOf(entity1, entity2)) {
        relationship.relationship_type = "front_of";
        relationship.confidence = 0.6f;
        return relationship;
    }
    
    if (isFrontOf(entity2, entity1)) {
        relationship.relationship_type = "behind";
        relationship.confidence = 0.6f;
        return relationship;
    }
    
    // No significant relationship found
    return relationship;
}

bool SpatialGraph::contains(const AmeEntity& container, const AmeEntity& contained) {
    // Simplified containment check: check if contained OBB center is inside container OBB
    const OBB& container_obb = container.obb;
    const OBB& contained_obb = contained.obb;
    
    // Transform contained center to container space
    Eigen::Vector3f relative_center = container_obb.rotation.transpose() * (contained_obb.center - container_obb.center);
    
    // Check if relative center is within container extents
    return (std::abs(relative_center.x()) < container_obb.extents.x()) &&
           (std::abs(relative_center.y()) < container_obb.extents.y()) &&
           (std::abs(relative_center.z()) < container_obb.extents.z());
}

bool SpatialGraph::isAbove(const AmeEntity& entity1, const AmeEntity& entity2) {
    // Check if entity1 is above entity2 (y-axis)
    return entity1.obb.center.y() > entity2.obb.center.y() + 0.1f;
}

bool SpatialGraph::isLeftOf(const AmeEntity& entity1, const AmeEntity& entity2) {
    // Check if entity1 is left of entity2 (x-axis)
    return entity1.obb.center.x() < entity2.obb.center.x() - 0.1f;
}

bool SpatialGraph::isFrontOf(const AmeEntity& entity1, const AmeEntity& entity2) {
    // Check if entity1 is front of entity2 (z-axis)
    return entity1.obb.center.z() > entity2.obb.center.z() + 0.1f;
}

bool SpatialGraph::isAdjacent(const AmeEntity& entity1, const AmeEntity& entity2) {
    float distance = computeDistance(entity1.obb, entity2.obb);
    return distance < 0.1f;
}

float SpatialGraph::computeDistance(const OBB& obb1, const OBB& obb2) {
    // Simplified distance computation: distance between centers
    return (obb1.center - obb2.center).norm();
}

bool SpatialGraph::intersects(const OBB& obb1, const OBB& obb2) {
    // Simplified intersection check: bounding sphere check
    float distance = (obb1.center - obb2.center).norm();
    float radius_sum = obb1.extents.norm() + obb2.extents.norm();
    return distance < radius_sum;
}

bool SpatialGraph::visualize(const std::string& output_file) const {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Failed to open output file for visualization" << std::endl;
        return false;
    }
    
    file << "digraph SpatialGraph {" << std::endl;
    file << "  node [shape=box];" << std::endl;
    
    for (const auto& relationship : relationships_) {
        file << "  " << relationship.source_id << " -> " << relationship.target_id 
             << " [label=\"" << relationship.relationship_type 
             << " (" << relationship.confidence << ")\"];" << std::endl;
    }
    
    file << "}" << std::endl;
    file.close();
    
    std::cout << "Spatial graph visualized to " << output_file << std::endl;
    return true;
}

} // namespace AmeScanner
