#include "common.h"
#include "FieldLoader.h"
#include "SpatialGrid.h"
#include "ScanProbe.h"
#include <iostream>
#include <chrono>

// 测试 Vector3 类
void testVector3() {
    std::cout << "=== Testing Vector3 ===" << std::endl;
    
    Vector3 v1(1, 2, 3);
    Vector3 v2(4, 5, 6);
    
    // 测试加法
    Vector3 sum = v1 + v2;
    std::cout << "v1 + v2 = (" << sum.x << ", " << sum.y << ", " << sum.z << ")" << std::endl;
    
    // 测试减法
    Vector3 diff = v2 - v1;
    std::cout << "v2 - v1 = (" << diff.x << ", " << diff.y << ", " << diff.z << ")" << std::endl;
    
    // 测试点积
    float dot = v1.dot(v2);
    std::cout << "v1 . v2 = " << dot << std::endl;
    
    // 测试长度
    float len = v1.length();
    std::cout << "|v1| = " << len << std::endl;
    
    std::cout << "Vector3 tests completed." << std::endl;
}

// 测试 BoundingBox 类
void testBoundingBox() {
    std::cout << "\n=== Testing BoundingBox ===" << std::endl;
    
    BoundingBox box(Vector3(0, 0, 0), Vector3(2, 2, 2));
    
    // 测试中心点
    Vector3 center = box.getCenter();
    std::cout << "Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
    
    // 测试尺寸
    Vector3 extents = box.getExtents();
    std::cout << "Extents: (" << extents.x << ", " << extents.y << ", " << extents.z << ")" << std::endl;
    
    // 测试包含点
    Vector3 inside(1, 1, 1);
    Vector3 outside(3, 1, 1);
    std::cout << "Contains (1,1,1): " << (box.contains(inside) ? "true" : "false") << std::endl;
    std::cout << "Contains (3,1,1): " << (box.contains(outside) ? "true" : "false") << std::endl;
    
    std::cout << "BoundingBox tests completed." << std::endl;
}

// 测试 SpatialGrid 类
void testSpatialGrid() {
    std::cout << "\n=== Testing SpatialGrid ===" << std::endl;
    
    // 创建测试数据
    std::vector<Vector3> positions;
    std::vector<float> opacities;
    
    positions.push_back(Vector3(0, 0, 0));
    opacities.push_back(0.5f);
    positions.push_back(Vector3(1, 1, 1));
    opacities.push_back(0.8f);
    positions.push_back(Vector3(-1, -1, -1));
    opacities.push_back(0.3f);
    
    // 构建空间网格
    SpatialGrid grid;
    grid.loadData(positions, opacities);
    
    // 测试密度查询
    float density = grid.getDensityAt(Vector3(0, 0, 0));
    std::cout << "Density at (0,0,0): " << density << std::endl;
    
    // 测试密度梯度
    Vector3 gradient = grid.getDensityGradient(Vector3(0, 0, 0));
    std::cout << "Gradient at (0,0,0): (" << gradient.x << ", " << gradient.y << ", " << gradient.z << ")" << std::endl;
    
    // 测试密度采样
    BoundingBox bounds(Vector3(-2, -2, -2), Vector3(2, 2, 2));
    std::vector<Vector3> samples = grid.sampleDensityPoints(bounds, 0.1f, 0.5f);
    std::cout << "Density samples found: " << samples.size() << std::endl;
    
    std::cout << "SpatialGrid tests completed." << std::endl;
}

// 测试性能
void testPerformance() {
    std::cout << "\n=== Testing Performance ===" << std::endl;
    
    // 创建大量测试数据
    std::vector<Vector3> positions;
    std::vector<float> opacities;
    
    const int numPoints = 10000;
    for (int i = 0; i < numPoints; i++) {
        float x = (rand() % 1000) / 100.0f - 5.0f;
        float y = (rand() % 1000) / 100.0f - 5.0f;
        float z = (rand() % 1000) / 100.0f - 5.0f;
        positions.push_back(Vector3(x, y, z));
        opacities.push_back((rand() % 100) / 100.0f);
    }
    
    // 测试构建时间
    auto start = std::chrono::high_resolution_clock::now();
    SpatialGrid grid;
    grid.loadData(positions, opacities);
    auto end = std::chrono::high_resolution_clock::now();
    auto buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Build time for " << numPoints << " points: " << buildTime << " ms" << std::endl;
    
    // 测试查询时间
    start = std::chrono::high_resolution_clock::now();
    const int numQueries = 10000;
    for (int i = 0; i < numQueries; i++) {
        float x = (rand() % 1000) / 100.0f - 5.0f;
        float y = (rand() % 1000) / 100.0f - 5.0f;
        float z = (rand() % 1000) / 100.0f - 5.0f;
        grid.getDensityAt(Vector3(x, y, z));
    }
    end = std::chrono::high_resolution_clock::now();
    auto queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Query time for " << numQueries << " queries: " << queryTime << " ms" << std::endl;
    std::cout << "Average query time: " << (queryTime * 1000.0f / numQueries) << " us" << std::endl;
    
    std::cout << "Performance tests completed." << std::endl;
}

int main() {
    // 运行所有测试
    testVector3();
    testBoundingBox();
    testSpatialGrid();
    testPerformance();
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}
