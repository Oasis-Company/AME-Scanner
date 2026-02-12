#pragma once

#include "common.h"
#include <string>

class FieldLoader {
public:
    // 加载原始 GS 数据并转化为内部密度场
    bool loadSplattingField(const std::string& filePath);

    // 显存管理：释放不需要的颜色信息，只保留几何与不透明度
    void optimizeMemory();

    // 获取当前场域的边界信息
    BoundingBox getGlobalBounds() const;

private:
    BoundingBox globalBounds;
    std::vector<Vector3> positions;
    std::vector<float> opacities;
    std::vector<float> scales;
    std::vector<float> rotations;
};
