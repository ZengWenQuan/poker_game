/**
 * @file GlobalConfig.h
 * @brief 全局配置单例头文件。
 *
 * 主要功能:
 *   - Singleton 模式: GlobalConfig::getInstance()
 *   - UI  参数: 字体/字号/颜色/按钮尺寸
 *   - 布局参数: 主牌区数量/底牌堆步进
 *   - 难度参数: 各难度下可见牌数量
 *   - 动画参数: 翻牌/移动动画时长
 *   - 语言  : setLanguage() / get() / getLanguage()
 */
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
    const std::string& getWindowTitle() const { return _windowTitle; }
    float getDesignWidth() const { return _designWidth; }
    float getDesignHeight() const { return _designHeight; }
    float getWindowScale() const { return _windowScale; }
    int getFps() const { return _fps; }
    bool getShowStats() const { return _showStats; }

    // ---- Card ----
    float getCardSourceWidth() const { return _cardSourceWidth; }
    float getCardSourceHeight() const { return _cardSourceHeight; }
    float getCardWidthRatioToBackground() const { return _cardWidthRatioToBackground; }
    float getSuitTargetWidthRatio() const { return _suitTargetWidthRatio; }
    float getSmallRankTargetHeightRatio() const { return _smallRankTargetHeightRatio; }
    float getBigRankTargetHeightRatio() const { return _bigRankTargetHeightRatio; }
    float getCornerInsetXRatio() const { return _cornerInsetXRatio; }
    float getCornerInsetYRatio() const { return _cornerInsetYRatio; }

    // ---- Game Rules ----
    int getMatchRankDiff() const { return _matchRankDiff; }
    int getRewardCardsPerBonus() const { return _rewardCardsPerBonus; }
    int getStandardDeckSize() const { return _standardDeckSize; }
    int getVisibleTopCardCountMin() const { return _visibleTopCardCountMin; }
    int getVisibleTopCardCountMax() const { return _visibleTopCardCountMax; }
    int getEditorRewardCardCount() const { return _editorRewardCardCount; }

    // ---- Interaction ----
    int getDoubleClickIntervalMs() const { return _doubleClickIntervalMs; }
    float getDragThresholdPx() const { return _dragThresholdPx; }
    float getDropAreaPaddingRatio() const { return _dropAreaPaddingRatio; }
    int getReserveHitPadding() const { return _reserveHitPadding; }
    const cocos2d::Rect& getRecycleHitRect() const { return _recycleHitRect; }
    int getDragZOrder() const { return _dragZOrder; }

    // ---- Animation ----
    float getMatchFlyDuration() const { return _matchFlyDuration; }
    float getFlipDuration() const { return _flipDuration; }
    float getBounceBackDuration() const { return _bounceBackDuration; }
    float getDrawDelayDuration() const { return _drawDelayDuration; }
    float getUndoFlyDuration() const { return _undoFlyDuration; }
    float getTopCardScaleUpDuration() const { return _topCardScaleUpDuration; }
    float getTopCardScaleUpFrom() const { return _topCardScaleUpFrom; }
    float getRewardFlyDelay() const { return _rewardFlyDelay; }
    float getRewardFlyDuration() const { return _rewardFlyDuration; }
    int getFlyingCardZOrder() const { return _flyingCardZOrder; }

    // ---- UI Layout ----
    const cocos2d::Vec2& getUndoPosition() const { return _undoPosition; }
    const cocos2d::Vec2& getStatusPositionRatio() const { return _statusPositionRatio; }
    const cocos2d::Vec2& getLevelTitlePositionRatio() const { return _levelTitlePositionRatio; }
    const cocos2d::Vec2& getLevelBtnStartRatio() const { return _levelBtnStartRatio; }
    float getLevelBtnWidth() const { return _levelBtnWidth; }
    const cocos2d::Vec2& getReserveStackStartRatio() const { return _reserveStackStartRatio; }
    float getReserveStackStepXRatio() const { return _reserveStackStepXRatio; }
    const cocos2d::Vec2& getReserveLabelPositionRatio() const { return _reserveLabelPositionRatio; }
    const cocos2d::Vec2& getRecyclePositionRatio() const { return _recyclePositionRatio; }
    const cocos2d::Vec2& getTopCardPositionRatio() const { return _topCardPositionRatio; }
    float getCardOverlapY() const { return _cardOverlapY; }
    float getSelectedScale() const { return _selectedScale; }
    float getCardSlotExtraHeight() const { return _cardSlotExtraHeight; }

    // ---- Layouts ----
    const std::vector<std::string>& getLayoutFiles() const { return _layoutFiles; }
    int getDefaultLayoutIndex() const { return _defaultLayoutIndex; }
    const std::vector<std::string>& getLayoutDirs() const { return _layoutDirs; }

    // ---- Theme: Font / Colors ----
    const std::string& getFont() const { return _font; }
    float getFontSize(const std::string& key) const;
    cocos2d::Color4B getColor4B(const std::string& key) const;
    cocos2d::Color3B getColor3B(const std::string& key) const;

    // ---- Theme: Dialogs ----
    cocos2d::Vec2 getDialogVec2(const std::string& dialogKey, const std::string& fieldKey, cocos2d::Vec2 fb = cocos2d::Vec2::ZERO) const;
    cocos2d::Size getDialogSize(const std::string& dialogKey, const std::string& fieldKey, cocos2d::Size fb = cocos2d::Size::ZERO) const;
    int getDialogInt(const std::string& dialogKey, const std::string& fieldKey, int fb = 0) const;

    // ---- Theme: Editor ----
    float getEditorFloat(const std::string& key, float fb = 0.0f) const;
    int getEditorInt(const std::string& key, int fb = 0) const;

    // ---- Images ----
    const std::string& getImage(const std::string& key) const;
    std::string getImageFormatted(const std::string& key, const std::string& color, const std::string& rank) const;

    // ---- Strings ----
    const std::string& get(const std::string& key) const;
    const std::string& getLanguage() const { return _lang; }
    void setLanguage(const std::string& lang);

private:
    GlobalConfig();
    void refreshStrings();

    // Window
    std::string _windowTitle{"poker_game"};
    float _designWidth{1920.0f};
    float _designHeight{1080.0f};
    float _windowScale{0.45f};
    int _fps{60};
    bool _showStats{true};

    // Card
    float _cardSourceWidth{182.0f};
    float _cardSourceHeight{282.0f};
    float _cardWidthRatioToBackground{0.05f};
    float _suitTargetWidthRatio{0.18f};
    float _smallRankTargetHeightRatio{0.14f};
    float _bigRankTargetHeightRatio{0.40f};
    float _cornerInsetXRatio{0.10f};
    float _cornerInsetYRatio{0.06f};

    // Game Rules
    int _matchRankDiff{1};
    int _rewardCardsPerBonus{3};
    int _standardDeckSize{52};
    int _visibleTopCardCountMin{1};
    int _visibleTopCardCountMax{3};
    int _editorRewardCardCount{4};

    // Interaction
    int _doubleClickIntervalMs{400};
    float _dragThresholdPx{10.0f};
    float _dropAreaPaddingRatio{0.5f};
    int _reserveHitPadding{20};
    cocos2d::Rect _recycleHitRect{-40, -20, 240, 60};
    int _dragZOrder{1000};

    // Animation
    float _matchFlyDuration{0.3f};
    float _flipDuration{0.2f};
    float _bounceBackDuration{0.25f};
    float _drawDelayDuration{0.3f};
    float _undoFlyDuration{0.3f};
    float _topCardScaleUpDuration{0.2f};
    float _topCardScaleUpFrom{0.8f};
    float _rewardFlyDelay{0.15f};
    float _rewardFlyDuration{0.25f};
    int _flyingCardZOrder{10};

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
    float _selectedScale{1.08f};
    float _cardSlotExtraHeight{200.0f};

    // Layouts
    std::vector<std::string> _layoutFiles;
    int _defaultLayoutIndex{0};
    std::vector<std::string> _layoutDirs;

    // Theme: Font / Colors / FontSizes
    std::string _font{"Arial"};
    std::unordered_map<std::string, float> _fontSizes;
    std::unordered_map<std::string, cocos2d::Color4B> _colors;

    // Theme: Dialogs
    cocos2d::ValueMap _dialogs;

    // Theme: Editor
    cocos2d::ValueMap _editor;

    // Images
    std::unordered_map<std::string, std::string> _images;

    // Strings
    std::string _lang{"en"};
    cocos2d::ValueMap _allStrings;
    std::unordered_map<std::string, std::string> _strings;
};

#endif
