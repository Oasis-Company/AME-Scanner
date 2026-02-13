#pragma once

#include "common.h"
#include <string>
#include <vector>

class FieldLoader {
public:
    // 加载原始 GS 数据并转化为内部密度场
    bool loadSplattingField(const std::string& filePath);

    // 显存管理：释放不需要的颜色信息，只保留几何与不透明度
    void optimizeMemory();

    // 获取当前场域的边界信息
    BoundingBox getGlobalBounds() const;

    // SoA 数据访问接口
    const std::vector<float>& getXPositions() const { return xPositions; }
    const std::vector<float>& getYPositions() const { return yPositions; }
    const std::vector<float>& getZPositions() const { return zPositions; }
    const std::vector<float>& getOpacities() const { return opacities; }
    size_t getPointCount() const { return xPositions.size(); }

private:
    BoundingBox globalBounds;
    
    // SoA (Structure of Arrays) 存储模式
    std::vector<float> xPositions;
    std::vector<float> yPositions;
    std::vector<float> zPositions;
    std::vector<float> opacities;
};
