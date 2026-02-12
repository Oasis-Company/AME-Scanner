#include "FieldLoader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// 加载原始 GS 数据并转化为内部密度场
bool FieldLoader::loadSplattingField(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    bool headerEnd = false;
    size_t vertexCount = 0;

    // 解析 PLY 头
    while (std::getline(file, line) && !headerEnd) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "element" && !headerEnd) {
            std::string type;
            iss >> type;
            if (type == "vertex") {
                iss >> vertexCount;
            }
        } else if (token == "end_header") {
            headerEnd = true;
        }
    }

    // 读取顶点数据
    positions.reserve(vertexCount);
    opacities.reserve(vertexCount);
    scales.reserve(vertexCount * 3);
    rotations.reserve(vertexCount * 4);

    // 初始化全局边界
    globalBounds = BoundingBox(Vector3(1e9, 1e9, 1e9), Vector3(-1e9, -1e9, -1e9));

    while (std::getline(file, line) && vertexCount > 0) {
        std::istringstream iss(line);
        float x, y, z, opacity, scaleX, scaleY, scaleZ, rotX, rotY, rotZ, rotW;
        
        // 假设 PLY 文件格式为：x y z r g b opacity scaleX scaleY scaleZ rotX rotY rotZ rotW
        iss >> x >> y >> z;
        // 跳过颜色信息
        float r, g, b;
        iss >> r >> g >> b;
        // 读取不透明度、缩放和旋转
        iss >> opacity >> scaleX >> scaleY >> scaleZ >> rotX >> rotY >> rotZ >> rotW;

        Vector3 position(x, y, z);
        positions.push_back(position);
        opacities.push_back(opacity);
        scales.push_back(scaleX);
        scales.push_back(scaleY);
        scales.push_back(scaleZ);
        rotations.push_back(rotX);
        rotations.push_back(rotY);
        rotations.push_back(rotZ);
        rotations.push_back(rotW);

        // 更新全局边界
        globalBounds.expandBy(position);

        vertexCount--;
    }

    file.close();
    return true;
}

// 显存管理：释放不需要的颜色信息，只保留几何与不透明度
void FieldLoader::optimizeMemory() {
    // 这里可以实现更复杂的内存优化策略
    // 例如：只保留不透明度大于阈值的点
    std::vector<Vector3> optimizedPositions;
    std::vector<float> optimizedOpacities;
    std::vector<float> optimizedScales;
    std::vector<float> optimizedRotations;

    const float opacityThreshold = 0.01f;
    for (size_t i = 0; i < opacities.size(); i++) {
        if (opacities[i] > opacityThreshold) {
            optimizedPositions.push_back(positions[i]);
            optimizedOpacities.push_back(opacities[i]);
            optimizedScales.push_back(scales[i * 3]);
            optimizedScales.push_back(scales[i * 3 + 1]);
            optimizedScales.push_back(scales[i * 3 + 2]);
            optimizedRotations.push_back(rotations[i * 4]);
            optimizedRotations.push_back(rotations[i * 4 + 1]);
            optimizedRotations.push_back(rotations[i * 4 + 2]);
            optimizedRotations.push_back(rotations[i * 4 + 3]);
        }
    }

    // 替换原始数据
    positions.swap(optimizedPositions);
    opacities.swap(optimizedOpacities);
    scales.swap(optimizedScales);
    rotations.swap(optimizedRotations);

    // 重新计算全局边界
    globalBounds = BoundingBox(Vector3(1e9, 1e9, 1e9), Vector3(-1e9, -1e9, -1e9));
    for (const auto& pos : positions) {
        globalBounds.expandBy(pos);
    }
}

// 获取当前场域的边界信息
BoundingBox FieldLoader::getGlobalBounds() const {
    return globalBounds;
}
