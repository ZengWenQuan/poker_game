#ifndef POKER_GAME_LAYOUT_CONFIG_H
#define POKER_GAME_LAYOUT_CONFIG_H

#include "cocos2d.h"
#include <string>
#include <vector>

struct SlotLayout
{
    int id;                    // 槽位唯一编号，运行时默认与主牌区下标一一对应
    float x;                   // 布局坐标 X，是否为绝对坐标由 usePileCenterOrigin 决定
    float y;                   // 布局坐标 Y，是否为绝对坐标由 usePileCenterOrigin 决定
    int pileIndex = 0;         // 所属主牌堆序号，基于 0，用于多主牌堆模板复用
    int layer = 0;             // 层级，数值越小越靠上；动态计算遮挡关系时依赖该值
    float rotation = 0.0f;     // 旋转角度（度），以左下角为圆心，顺时针为正
    std::vector<int> covers;   // 压住当前槽位的父节点列表，用于判断当前牌是否可翻开
};

// 布局配置：负责解析布局 JSON，提供槽位数据，并生成动态遮挡关系。
class LayoutConfig
{
public:
    // 从 JSON 文件加载布局定义。
    // 该过程会解析几何数据，并在满足条件时重建动态 covers，
    // 使布局资源尽量只描述结构而不重复维护遮挡关系。
    // filePath: Resources 相对路径，例如 "config/layouts/fan.json"。
    bool loadFromFile(const std::string& filePath);

    const std::string& getName() const { return _name; }
    int getTotalSlots() const { return _totalSlots; }
    int getMainPileCount() const { return _mainPileCount; }
    bool usePileCenterOrigin() const { return _usePileCenterOrigin; }
    const std::vector<SlotLayout>& getSlots() const { return _slots; }

    // 获取所有可用布局的文件路径（扫描约定目录下的 json 文件）。
    struct LayoutInfo
    {
        std::string filePath;  // 如 "layouts/fan.json"
        std::string name;      // 如 "Fan"
    };
    static std::vector<LayoutInfo> getAvailableLayouts();

    // 根据层级与几何重叠关系重建 covers。
    // 编辑器和运行时都应复用这套逻辑，避免多处维护同一规则。
    // slots: 待处理槽位数组；usePileCenterOrigin: 是否按牌堆中心定位；
    // mainPileCount: 主牌堆数量，用于换算相对坐标。
    static void rebuildDynamicCoversForSlots(std::vector<SlotLayout>& slots,
                                             bool usePileCenterOrigin,
                                             int mainPileCount);

    // 把布局中的局部坐标解析成设计分辨率下的场景坐标。
    // 启用主牌堆中心原点时，x/y 相对各牌堆中心；否则直接视为绝对坐标。
    // slot: 单个槽位定义；areaWidth/areaHeight: 主牌区尺寸。
    static cocos2d::Vec2 resolveSlotPosition(const SlotLayout& slot,
                                             bool usePileCenterOrigin,
                                             int mainPileCount,
                                             float areaWidth,
                                             float areaHeight);

private:
    std::string _name;                 // 布局名称，主要用于 UI 和日志
    int _totalSlots = 0;               // 槽位总数，决定主牌区初始发牌量
    int _mainPileCount = 1;            // 主牌区逻辑牌堆数量
    bool _usePileCenterOrigin = false; // 是否启用“按各主牌堆中心定位”的布局模式
    std::vector<SlotLayout> _slots;    // 解析后的全部槽位定义
};

#endif
