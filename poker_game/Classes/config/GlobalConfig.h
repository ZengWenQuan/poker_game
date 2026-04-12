#ifndef POKER_GAME_GLOBAL_CONFIG_H
#define POKER_GAME_GLOBAL_CONFIG_H

#include "cocos2d.h"
#include <string>
#include <vector>
#include <unordered_map>

class GlobalConfig
{
public:
    static GlobalConfig& getInstance();

    // ---- Window ----
    const std::string& getWindowTitle() const { return _windowTitle; }
    float getDesignWidth() const { return _designWidth; }
    float getDesignHeight() const { return _designHeight; }
    float getWindowScale() const { return _windowScale; }

    // ---- Card ----
    float getCardSourceWidth() const { return _cardSourceWidth; }
    float getCardSourceHeight() const { return _cardSourceHeight; }
    float getCardWidthRatioToBackground() const { return _cardWidthRatioToBackground; }

    // ---- Interaction ----
    int getDoubleClickIntervalMs() const { return _doubleClickIntervalMs; }
    float getDragThresholdPx() const { return _dragThresholdPx; }
    float getDropAreaPaddingRatio() const { return _dropAreaPaddingRatio; }
    int getReserveHitPadding() const { return _reserveHitPadding; }
    const cocos2d::Rect& getRecycleHitRect() const { return _recycleHitRect; }

    // ---- Animation ----
    float getMatchFlyDuration() const { return _matchFlyDuration; }
    float getFlipDuration() const { return _flipDuration; }
    float getBounceBackDuration() const { return _bounceBackDuration; }
    float getDrawDelayDuration() const { return _drawDelayDuration; }
    float getUndoFlyDuration() const { return _undoFlyDuration; }
    float getTopCardScaleUpDuration() const { return _topCardScaleUpDuration; }

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

    // ---- Layouts ----
    const std::vector<std::string>& getLayoutFiles() const { return _layoutFiles; }
    int getDefaultLayoutIndex() const { return _defaultLayoutIndex; }

    // ---- Theme: Font / Colors ----
    const std::string& getFont() const { return _font; }
    float getFontSize(const std::string& key) const;
    cocos2d::Color4B getColor4B(const std::string& key) const;
    cocos2d::Color3B getColor3B(const std::string& key) const;

    // ---- Images (全部从 theme.json images 段读取) ----
    const std::string& getImage(const std::string& key) const;
    std::string getImageFormatted(const std::string& key, const std::string& color, const std::string& rank) const;

    // ---- Strings ----
    const std::string& get(const std::string& key) const;

private:
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
    std::vector<std::string> _layoutFiles;
    int _defaultLayoutIndex{0};

    // Theme: Font / Colors / FontSizes
    std::string _font{"Arial"};
    std::unordered_map<std::string, float> _fontSizes;
    std::unordered_map<std::string, cocos2d::Color4B> _colors;

    // Images: key → path (或路径模板)
    std::unordered_map<std::string, std::string> _images;

    // Strings
    std::unordered_map<std::string, std::string> _strings;
};

#endif
