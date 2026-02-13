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

    // 读取顶点数据 - 按需加载，只提取 x, y, z 和 opacity
    xPositions.reserve(vertexCount);
    yPositions.reserve(vertexCount);
    zPositions.reserve(vertexCount);
    opacities.reserve(vertexCount);

    // 初始化全局边界
    globalBounds = BoundingBox(Vector3(1e9, 1e9, 1e9), Vector3(-1e9, -1e9, -1e9));

    while (std::getline(file, line) && vertexCount > 0) {
        std::istringstream iss(line);
        float x, y, z, opacity;
        
        // 假设 PLY 文件格式为：x y z r g b opacity scaleX scaleY scaleZ rotX rotY rotZ rotW
        iss >> x >> y >> z;
        
        // 跳过颜色信息
        float r, g, b;
        iss >> r >> g >> b;
        
        // 读取不透明度
        iss >> opacity;
        
        // 跳过缩放和旋转信息（按需加载）
        float scaleX, scaleY, scaleZ, rotX, rotY, rotZ, rotW;
        iss >> scaleX >> scaleY >> scaleZ >> rotX >> rotY >> rotZ >> rotW;

        // 存储数据到 SoA 结构
        xPositions.push_back(x);
        yPositions.push_back(y);
        zPositions.push_back(z);
        opacities.push_back(opacity);

        // 更新全局边界
        globalBounds.expandBy(Vector3(x, y, z));

        vertexCount--;
    }

    file.close();
    return true;
}

// 显存管理：释放不需要的颜色信息，只保留几何与不透明度
void FieldLoader::optimizeMemory() {
    // 实现动态阈值过滤，去除虚假密度
    std::vector<float> optimizedXPositions;
    std::vector<float> optimizedYPositions;
    std::vector<float> optimizedZPositions;
    std::vector<float> optimizedOpacities;

    const float opacityThreshold = 0.01f;
    for (size_t i = 0; i < opacities.size(); i++) {
        if (opacities[i] > opacityThreshold) {
            optimizedXPositions.push_back(xPositions[i]);
            optimizedYPositions.push_back(yPositions[i]);
            optimizedZPositions.push_back(zPositions[i]);
            optimizedOpacities.push_back(opacities[i]);
        }
    }

    // 替换原始数据
    xPositions.swap(optimizedXPositions);
    yPositions.swap(optimizedYPositions);
    zPositions.swap(optimizedZPositions);
    opacities.swap(optimizedOpacities);

    // 重新计算全局边界
    globalBounds = BoundingBox(Vector3(1e9, 1e9, 1e9), Vector3(-1e9, -1e9, -1e9));
    for (size_t i = 0; i < xPositions.size(); i++) {
        globalBounds.expandBy(Vector3(xPositions[i], yPositions[i], zPositions[i]));
    }
}

// 获取当前场域的边界信息
BoundingBox FieldLoader::getGlobalBounds() const {
    return globalBounds;
}
