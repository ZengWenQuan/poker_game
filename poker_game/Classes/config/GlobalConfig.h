#ifndef POKER_GAME_GLOBAL_CONFIG_H
#define POKER_GAME_GLOBAL_CONFIG_H

#include "cocos2d.h"
#include <string>
#include <vector>
#include <unordered_map>

// 全局运行时配置中心。
// 负责在启动阶段读取游戏配置、主题配置和文案配置，
// 并向其他模块提供统一的只读访问接口。
class GlobalConfig
{
public:
    // 获取全局唯一配置实例。
    static GlobalConfig& getInstance();

    // 运行时聚合配置中心：
    // 从 game/theme/strings 三类 JSON 读取参数，并提供只读访问接口。

    // ---- Window ----
    // 获取窗口标题文本。
    const std::string& getWindowTitle() const { return _windowTitle; }
    // 获取设计分辨率宽度，用于界面布局换算。
    float getDesignWidth() const { return _designWidth; }
    // 获取设计分辨率高度，用于界面布局换算。
    float getDesignHeight() const { return _designHeight; }
    // 获取窗口缩放比例，用于决定实际显示尺寸。
    float getWindowScale() const { return _windowScale; }

    // ---- Card ----
    // 获取原始牌面资源宽度。
    float getCardSourceWidth() const { return _cardSourceWidth; }
    // 获取原始牌面资源高度。
    float getCardSourceHeight() const { return _cardSourceHeight; }
    // 获取牌宽相对背景宽度的比例，用于自适应缩放。
    float getCardWidthRatioToBackground() const { return _cardWidthRatioToBackground; }

    // ---- Interaction ----
    // 获取双击判定间隔，单位毫秒。
    int getDoubleClickIntervalMs() const { return _doubleClickIntervalMs; }
    // 获取拖拽判定最小位移，单位像素。
    float getDragThresholdPx() const { return _dragThresholdPx; }
    // 获取放置区域的额外留白比例，提升拖拽容错。
    float getDropAreaPaddingRatio() const { return _dropAreaPaddingRatio; }
    // 获取暂存区命中区域额外扩展像素。
    int getReserveHitPadding() const { return _reserveHitPadding; }
    // 获取回收区命中矩形，矩形基于回收区锚点定义。
    const cocos2d::Rect& getRecycleHitRect() const { return _recycleHitRect; }

    // ---- Animation ----
    // 获取匹配成功后飞行动画时长。
    float getMatchFlyDuration() const { return _matchFlyDuration; }
    // 获取翻牌动画时长。
    float getFlipDuration() const { return _flipDuration; }
    // 获取拖拽失败后回弹动画时长。
    float getBounceBackDuration() const { return _bounceBackDuration; }
    // 获取发牌或补牌之间的延迟时长。
    float getDrawDelayDuration() const { return _drawDelayDuration; }
    // 获取撤销时牌飞回的动画时长。
    float getUndoFlyDuration() const { return _undoFlyDuration; }
    // 获取顶部牌放大强调动画时长。
    float getTopCardScaleUpDuration() const { return _topCardScaleUpDuration; }

    // ---- UI Layout ----
    // 获取撤销按钮的绝对坐标。
    const cocos2d::Vec2& getUndoPosition() const { return _undoPosition; }
    // 获取状态文本位置比例，基于设计分辨率换算。
    const cocos2d::Vec2& getStatusPositionRatio() const { return _statusPositionRatio; }
    // 获取关卡标题位置比例。
    const cocos2d::Vec2& getLevelTitlePositionRatio() const { return _levelTitlePositionRatio; }
    // 获取关卡按钮起始位置比例。
    const cocos2d::Vec2& getLevelBtnStartRatio() const { return _levelBtnStartRatio; }
    // 获取关卡按钮宽度。
    float getLevelBtnWidth() const { return _levelBtnWidth; }
    // 获取暂存牌堆起始位置比例。
    const cocos2d::Vec2& getReserveStackStartRatio() const { return _reserveStackStartRatio; }
    // 获取暂存牌堆横向步进比例。
    float getReserveStackStepXRatio() const { return _reserveStackStepXRatio; }
    // 获取暂存区标签位置比例。
    const cocos2d::Vec2& getReserveLabelPositionRatio() const { return _reserveLabelPositionRatio; }
    // 获取回收区位置比例。
    const cocos2d::Vec2& getRecyclePositionRatio() const { return _recyclePositionRatio; }
    // 获取顶部牌位置比例。
    const cocos2d::Vec2& getTopCardPositionRatio() const { return _topCardPositionRatio; }
    // 获取牌堆纵向重叠像素值。
    float getCardOverlapY() const { return _cardOverlapY; }

    // ---- Layouts ----
    // 获取布局配置文件列表。
    const std::vector<std::string>& getLayoutFiles() const { return _layoutFiles; }
    // 获取默认布局索引。
    int getDefaultLayoutIndex() const { return _defaultLayoutIndex; }

    // ---- Theme: Font / Colors ----
    // 获取默认字体名称。
    const std::string& getFont() const { return _font; }
    // 根据 key 获取字号。
    // `key` 表示 theme.json 中 fontSize 节点下的字段名。
    float getFontSize(const std::string& key) const;
    // 根据 key 获取带透明度的颜色。
    // `key` 表示 theme.json 中 colors 节点下的字段名。
    cocos2d::Color4B getColor4B(const std::string& key) const;
    // 根据 key 获取不含透明度的颜色。
    // `key` 表示 theme.json 中 colors 节点下的字段名。
    cocos2d::Color3B getColor3B(const std::string& key) const;

    // ---- Images (全部从 theme.json images 段读取) ----
    // 根据图片 key 获取资源路径或路径模板。
    // `key` 表示 theme.json 中 images 节点下的字段名。
    const std::string& getImage(const std::string& key) const;
    // 获取格式化后的图片路径。
    // `key` 表示图片模板名称，`color` 和 `rank` 用来替换模板中的占位符。
    std::string getImageFormatted(const std::string& key, const std::string& color, const std::string& rank) const;

    // ---- Strings ----
    // 根据文案 key 获取当前语言字符串。
    // `key` 表示 strings.json 中语言节点下的字段名。
    const std::string& get(const std::string& key) const;

private:
    // 构造时从多个 JSON 文件读取配置并缓存到内存。
    GlobalConfig();

    // Window
    std::string _windowTitle{"poker_game"};
    float _designWidth{1920.0f};
    float _designHeight{1080.0f};
    float _windowScale{0.45f};

    // Card
    float _cardSourceWidth{182.0f};
    float _cardSourceHeight{282.0f};
    float _cardWidthRatioToBackground{0.05f};

    // Interaction
    int _doubleClickIntervalMs{400};
    float _dragThresholdPx{10.0f};
    float _dropAreaPaddingRatio{0.5f};
    int _reserveHitPadding{20};
    cocos2d::Rect _recycleHitRect{-40, -20, 240, 60};

    // Animation
    float _matchFlyDuration{0.3f};
    float _flipDuration{0.2f};
    float _bounceBackDuration{0.25f};
    float _drawDelayDuration{0.3f};
    float _undoFlyDuration{0.3f};
    float _topCardScaleUpDuration{0.2f};

    // UI
    cocos2d::Vec2 _undoPosition{120, 60};
    cocos2d::Vec2 _statusPositionRatio{0.104f, 0.954f};
    cocos2d::Vec2 _levelTitlePositionRatio{0.917f, 0.963f};
    cocos2d::Vec2 _levelBtnStartRatio{0.833f, 0.917f};
    float _levelBtnWidth{110.0f};
    cocos2d::Vec2 _reserveStackStartRatio{0.46f, 0.171f};
    float _reserveStackStepXRatio{1.0f};
    cocos2d::Vec2 _reserveLabelPositionRatio{0.46f, 0.037f};
    cocos2d::Vec2 _recyclePositionRatio{0.46f, -0.037f};
    cocos2d::Vec2 _topCardPositionRatio{0.58f, 0.171f};
    float _cardOverlapY{9.0f};

    // Layouts
    std::vector<std::string> _layoutFiles; // 布局配置文件路径列表
    int _defaultLayoutIndex{0};            // 默认启用的布局下标

    // Theme: Font / Colors / FontSizes
    std::string _font{"Arial"};
    std::unordered_map<std::string, float> _fontSizes;         // 字号配置表，key 为字号名称
    std::unordered_map<std::string, cocos2d::Color4B> _colors; // 颜色配置表，key 为颜色名称

    // Images: key → path (或路径模板)
    std::unordered_map<std::string, std::string> _images;      // 图片路径或格式模板

    // Strings
    std::unordered_map<std::string, std::string> _strings;     // 当前语言的文案表
};

#endif
