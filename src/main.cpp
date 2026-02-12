#include "FieldLoader.h"
#include "SpatialGrid.h"
#include "ScanProbe.h"
#include <iostream>
#include <string>

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
    // 注意：这里需要访问 FieldLoader 的私有成员，实际实现中应该提供相应的接口
    // 这里我们使用模拟数据
    std::vector<Vector3> positions;
    std::vector<float> opacities;
    positions.push_back(Vector3(0, 0, 0));
    opacities.push_back(0.5f);
    grid.loadData(positions, opacities);

    // 5. 执行扫描
    ScanProbe probe;
    probe.setSpatialGrid(grid);
    probe.setDensityThreshold(0.01f);

    std::cout << "Performing global survey..." << std::endl;
    std::vector<RawCluster> clusters = probe.performGlobalSurvey();

    std::cout << "Detected " << clusters.size() << " clusters" << std::endl;

    // 6. 生成扫描结果
    ScanPayload payload = probe.capturePayload();
    std::cout << "Scan completed." << std::endl;

    return 0;
}
