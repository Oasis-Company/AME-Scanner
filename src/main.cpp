#include "FieldLoader.h"
#include "SpatialGrid.h"
#include "ScanProbe.h"
#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        std::cout << "Usage: ame-scanner <path_to_ply_file>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    
    // 1. 加载高斯点云数据
    FieldLoader loader;
    std::cout << "Loading splatting field from " << filePath << "..." << std::endl;
    if (!loader.loadSplattingField(filePath)) {
        std::cerr << "Failed to load splatting field" << std::endl;
        return 1;
    }

    // 2. 优化内存使用
    std::cout << "Optimizing memory..." << std::endl;
    loader.optimizeMemory();

    // 3. 获取全局边界
    BoundingBox bounds = loader.getGlobalBounds();
    std::cout << "Global bounds: min(" << bounds.min.x << ", " << bounds.min.y << ", " << bounds.min.z << "), "
              << "max(" << bounds.max.x << ", " << bounds.max.y << ", " << bounds.max.z << ")" << std::endl;

    // 4. 构建空间网格
    SpatialGrid grid;
    // 使用 SoA 数据格式加载
    grid.loadData(loader.getXPositions(), loader.getYPositions(), loader.getZPositions(), loader.getOpacities());
    std::cout << "Loaded " << loader.getPointCount() << " points" << std::endl;

    // 5. 测试核心查询函数性能
    std::cout << "Testing query performance..." << std::endl;
    Vector3 testPos(0, 0, 0);
    float searchRadius = 0.1f;
    
    // 计时查询
    auto start = std::chrono::high_resolution_clock::now();
    float density = grid.queryDensity(testPos, searchRadius);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "Density at (0,0,0): " << density << std::endl;
    std::cout << "Query time: " << duration << " microseconds" << std::endl;

    // 6. 执行扫描
    ScanProbe probe;
    probe.setSpatialGrid(grid);
    probe.setDensityThreshold(0.01f);

    std::cout << "Performing global survey..." << std::endl;
    std::vector<RawCluster> clusters = probe.performGlobalSurvey();

    std::cout << "Detected " << clusters.size() << " clusters" << std::endl;

    // 7. 生成扫描结果
    ScanPayload payload = probe.capturePayload();
    std::cout << "Scan completed." << std::endl;

    return 0;
}
