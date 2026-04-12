#ifndef POKER_GAME_LAYOUT_CONFIG_H
#define POKER_GAME_LAYOUT_CONFIG_H

#include "cocos2d.h"
#include <string>
#include <vector>

struct SlotLayout
{
    int id;
    float x;
    float y;
    int pileIndex = 0;         // 所属主牌堆序号，基于 0
    int layer = 0;             // 所在层级，越小越靠上
    float rotation = 0.0f;  // 旋转角度（度），以左下角为圆心，顺时针为正
    std::vector<int> covers;  // 压住当前 slot 的上一层牌（parents）
};

class LayoutConfig
{
public:
    // 从 JSON 文件加载布局
    bool loadFromFile(const std::string& filePath);

    const std::string& getName() const { return _name; }
    int getTotalSlots() const { return _totalSlots; }
    int getMainPileCount() const { return _mainPileCount; }
    bool usePileCenterOrigin() const { return _usePileCenterOrigin; }
    const std::vector<SlotLayout>& getSlots() const { return _slots; }

    // 获取所有可用布局的文件路径（扫描 layouts/ 目录下的 json 文件）
    struct LayoutInfo
    {
        std::string filePath;  // 如 "layouts/fan.json"
        std::string name;      // 如 "Fan"
    };
    static std::vector<LayoutInfo> getAvailableLayouts();
    static void rebuildDynamicCoversForSlots(std::vector<SlotLayout>& slots,
                                             bool usePileCenterOrigin,
                                             int mainPileCount);
    static cocos2d::Vec2 resolveSlotPosition(const SlotLayout& slot,
                                             bool usePileCenterOrigin,
                                             int mainPileCount,
                                             float areaWidth,
                                             float areaHeight);

private:
    std::string _name;
    int _totalSlots = 0;
    int _mainPileCount = 1;
    bool _usePileCenterOrigin = false;
    std::vector<SlotLayout> _slots;
};

#endif
