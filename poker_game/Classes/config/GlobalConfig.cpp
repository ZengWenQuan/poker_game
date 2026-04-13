/**
 * @file GlobalConfig.cpp
 * @brief 全局配置单例实现。
 *
 * 主要功能:
 *   - 读取并缓存 game_config.json、theme.json、strings.json
 *   - 提供运行时配置的查询接口 (UI/布局/难度/动画参数等)
 *   - 支持中英文语言切换 (setLanguage/refreshStrings)
 */
#include "GlobalConfig.h"
#include "JsonConfig.h"

namespace
{
GlobalConfig* s_instance = nullptr;
}

GlobalConfig::GlobalConfig()
{
    JsonConfig game("config/game_config.json");
    JsonConfig theme("config/theme.json");
    JsonConfig strings("config/strings.json");

    // Window
    _windowTitle  = game.getString("window.title", "poker_game");
    _designWidth  = game.getFloat("window.designWidth", 1920.0f);
    _designHeight = game.getFloat("window.designHeight", 1080.0f);
    _windowScale  = game.getFloat("window.scale", 0.45f);
    _fps          = game.getInt("window.fps", 60);
    _showStats    = game.lookup("window.showStats") ? game.lookup("window.showStats")->asBool() : true;

    // Card
    _cardSourceWidth  = game.getFloat("card.sourceWidth", 182.0f);
    _cardSourceHeight = game.getFloat("card.sourceHeight", 282.0f);
    _cardWidthRatioToBackground = game.getFloat("card.widthRatioToBackground", 0.05f);
    _suitTargetWidthRatio       = game.getFloat("card.suitTargetWidthRatio", 0.18f);
    _smallRankTargetHeightRatio = game.getFloat("card.smallRankTargetHeightRatio", 0.14f);
    _bigRankTargetHeightRatio   = game.getFloat("card.bigRankTargetHeightRatio", 0.40f);
    _cornerInsetXRatio          = game.getFloat("card.cornerInsetXRatio", 0.10f);
    _cornerInsetYRatio          = game.getFloat("card.cornerInsetYRatio", 0.06f);

    // Game Rules
    _matchRankDiff         = game.getInt("game.matchRankDiff", 1);
    _rewardCardsPerBonus   = game.getInt("game.rewardCardsPerBonus", 3);
    _standardDeckSize      = game.getInt("game.standardDeckSize", 52);
    _visibleTopCardCountMin = game.getInt("game.visibleTopCardCountMin", 1);
    _visibleTopCardCountMax = game.getInt("game.visibleTopCardCountMax", 3);
    _editorRewardCardCount  = game.getInt("game.editorRewardCardCount", 4);

    // Interaction
    _doubleClickIntervalMs = game.getInt("interaction.doubleClickIntervalMs", 400);
    _dragThresholdPx       = game.getFloat("interaction.dragThresholdPx", 10.0f);
    _dropAreaPaddingRatio  = game.getFloat("interaction.dropAreaPaddingRatio", 0.5f);
    _reserveHitPadding     = game.getInt("interaction.reserveHitPadding", 20);
    _dragZOrder            = game.getInt("interaction.dragZOrder", 1000);
    {
        auto* v = game.lookup("interaction.recycleHitRect");
        if (v && v->getType() == cocos2d::Value::Type::VECTOR && v->asValueVector().size() >= 4)
        {
            const auto& r = v->asValueVector();
            _recycleHitRect = cocos2d::Rect(r[0].asFloat(), r[1].asFloat(), r[2].asFloat(), r[3].asFloat());
        }
    }

    // Animation
    _matchFlyDuration        = game.getFloat("animation.matchFlyDuration", 0.3f);
    _flipDuration            = game.getFloat("animation.flipDuration", 0.2f);
    _bounceBackDuration      = game.getFloat("animation.bounceBackDuration", 0.25f);
    _drawDelayDuration       = game.getFloat("animation.drawDelayDuration", 0.3f);
    _undoFlyDuration         = game.getFloat("animation.undoFlyDuration", 0.3f);
    _topCardScaleUpDuration  = game.getFloat("animation.topCardScaleUpDuration", 0.2f);
    _topCardScaleUpFrom      = game.getFloat("animation.topCardScaleUpFrom", 0.8f);
    _rewardFlyDelay          = game.getFloat("animation.rewardFlyDelay", 0.15f);
    _rewardFlyDuration       = game.getFloat("animation.rewardFlyDuration", 0.25f);
    _flyingCardZOrder        = game.getInt("animation.flyingCardZOrder", 10);

    // UI
    _undoPosition              = game.getVec2("ui.undoPosition", {120, 60});
    _statusPositionRatio       = game.getVec2("ui.statusPositionRatio", {0.104f, 0.954f});
    _levelTitlePositionRatio   = game.getVec2("ui.levelTitlePositionRatio", {0.917f, 0.963f});
    _levelBtnStartRatio        = game.getVec2("ui.levelBtnStartRatio", {0.833f, 0.917f});
    _levelBtnWidth             = game.getFloat("ui.levelBtnWidth", 110.0f);
    _reserveStackStartRatio    = game.getVec2("ui.reserveStackStartRatio", {0.46f, 0.171f});
    _reserveStackStepXRatio    = game.getFloat("ui.reserveStackStepXRatio", 1.0f);
    _reserveLabelPositionRatio = game.getVec2("ui.reserveLabelPositionRatio", {0.46f, 0.037f});
    _recyclePositionRatio      = game.getVec2("ui.recyclePositionRatio", {0.46f, -0.037f});
    _topCardPositionRatio      = game.getVec2("ui.topCardPositionRatio", {0.58f, 0.171f});
    _cardOverlapY              = game.getFloat("ui.cardOverlapY", 9.0f);
    _selectedScale             = game.getFloat("ui.selectedScale", 1.08f);
    _cardSlotExtraHeight       = game.getFloat("ui.cardSlotExtraHeight", 200.0f);

    // Layouts
    _layoutFiles        = game.getStringArray("layouts");
    _defaultLayoutIndex = game.getInt("defaultLayoutIndex", 0);
    _layoutDirs         = game.getStringArray("layoutDirs");
    if (_layoutDirs.empty())
    {
        _layoutDirs = {"config/layouts/", "layouts/"};
    }

    // Theme: Font
    _font = theme.getString("font", "Arial");

    // Theme: FontSizes
    auto* fsMap = theme.lookup("fontSize");
    if (fsMap && fsMap->getType() == cocos2d::Value::Type::MAP)
    {
        for (const auto& pair : fsMap->asValueMap())
            _fontSizes[pair.first] = pair.second.asFloat();
    }

    // Theme: Colors
    auto* colMap = theme.lookup("colors");
    if (colMap && colMap->getType() == cocos2d::Value::Type::MAP)
    {
        for (const auto& pair : colMap->asValueMap())
        {
            if (pair.second.getType() == cocos2d::Value::Type::VECTOR)
            {
                const auto& arr = pair.second.asValueVector();
                if (arr.size() >= 3)
                {
                    int a = (arr.size() >= 4) ? arr[3].asInt() : 255;
                    _colors[pair.first] = cocos2d::Color4B(arr[0].asInt(), arr[1].asInt(), arr[2].asInt(), a);
                }
            }
        }
    }

    // Theme: Dialogs
    auto* dlgMap = theme.lookup("dialogs");
    if (dlgMap && dlgMap->getType() == cocos2d::Value::Type::MAP)
    {
        _dialogs = dlgMap->asValueMap();
    }

    // Theme: Editor
    auto* edMap = theme.lookup("editor");
    if (edMap && edMap->getType() == cocos2d::Value::Type::MAP)
    {
        _editor = edMap->asValueMap();
    }

    // Theme: Images
    auto* imgMap = theme.lookup("images");
    if (imgMap && imgMap->getType() == cocos2d::Value::Type::MAP)
    {
        for (const auto& pair : imgMap->asValueMap())
        {
            if (pair.second.getType() == cocos2d::Value::Type::STRING)
            {
                _images[pair.first] = pair.second.asString();
            }
        }
    }

    // GameConfig: 只覆盖奖励牌资源路径，避免和 theme.json 产生重复来源。
    const std::string rewardFace = game.getString("theme.rewardCardFace", "");
    if (!rewardFace.empty())
    {
        _images["rewardCardFace"] = rewardFace;
    }
    const std::string rewardBack = game.getString("theme.rewardCardBack", "");
    if (!rewardBack.empty())
    {
        _images["rewardCardBack"] = rewardBack;
    }

    // Strings
    _allStrings = strings.getData();
    refreshStrings();
}

GlobalConfig& GlobalConfig::getInstance()
{
    if (!s_instance)
        s_instance = new GlobalConfig();
    return *s_instance;
}

float GlobalConfig::getFontSize(const std::string& key) const
{
    auto it = _fontSizes.find(key);
    return it != _fontSizes.end() ? it->second : 28.0f;
}

cocos2d::Color4B GlobalConfig::getColor4B(const std::string& key) const
{
    auto it = _colors.find(key);
    return it != _colors.end() ? it->second : cocos2d::Color4B::WHITE;
}

cocos2d::Color3B GlobalConfig::getColor3B(const std::string& key) const
{
    auto it = _colors.find(key);
    return it != _colors.end() ? cocos2d::Color3B(it->second.r, it->second.g, it->second.b) : cocos2d::Color3B::WHITE;
}

cocos2d::Vec2 GlobalConfig::getDialogVec2(const std::string& dialogKey, const std::string& fieldKey, cocos2d::Vec2 fb) const
{
    auto dlgIt = _dialogs.find(dialogKey);
    if (dlgIt == _dialogs.end() || dlgIt->second.getType() != cocos2d::Value::Type::MAP)
        return fb;
    const auto& dlg = dlgIt->second.asValueMap();
    auto fieldIt = dlg.find(fieldKey);
    if (fieldIt == dlg.end() || fieldIt->second.getType() != cocos2d::Value::Type::VECTOR)
        return fb;
    const auto& arr = fieldIt->second.asValueVector();
    if (arr.size() < 2) return fb;
    return cocos2d::Vec2(arr[0].asFloat(), arr[1].asFloat());
}

cocos2d::Size GlobalConfig::getDialogSize(const std::string& dialogKey, const std::string& fieldKey, cocos2d::Size fb) const
{
    auto dlgIt = _dialogs.find(dialogKey);
    if (dlgIt == _dialogs.end() || dlgIt->second.getType() != cocos2d::Value::Type::MAP)
        return fb;
    const auto& dlg = dlgIt->second.asValueMap();
    auto fieldIt = dlg.find(fieldKey);
    if (fieldIt == dlg.end() || fieldIt->second.getType() != cocos2d::Value::Type::VECTOR)
        return fb;
    const auto& arr = fieldIt->second.asValueVector();
    if (arr.size() < 2) return fb;
    return cocos2d::Size(arr[0].asFloat(), arr[1].asFloat());
}

int GlobalConfig::getDialogInt(const std::string& dialogKey, const std::string& fieldKey, int fb) const
{
    auto dlgIt = _dialogs.find(dialogKey);
    if (dlgIt == _dialogs.end() || dlgIt->second.getType() != cocos2d::Value::Type::MAP)
        return fb;
    const auto& dlg = dlgIt->second.asValueMap();
    auto fieldIt = dlg.find(fieldKey);
    if (fieldIt == dlg.end())
        return fb;
    return fieldIt->second.asInt();
}

float GlobalConfig::getEditorFloat(const std::string& key, float fb) const
{
    auto it = _editor.find(key);
    if (it == _editor.end())
        return fb;
    return it->second.asFloat();
}

int GlobalConfig::getEditorInt(const std::string& key, int fb) const
{
    auto it = _editor.find(key);
    if (it == _editor.end())
        return fb;
    return it->second.asInt();
}

const std::string& GlobalConfig::getImage(const std::string& key) const
{
    auto it = _images.find(key);
    return it != _images.end() ? it->second : key;
}

std::string GlobalConfig::getImageFormatted(const std::string& key, const std::string& color, const std::string& rank) const
{
    auto it = _images.find(key);
    if (it == _images.end()) return key;
    std::string result = it->second;
    auto replaceAll = [](std::string& s, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::string::npos)
        {
            s.replace(pos, from.size(), to);
            pos += to.size();
        }
    };
    replaceAll(result, "{color}", color);
    replaceAll(result, "{rank}", rank);
    return result;
}

const std::string& GlobalConfig::get(const std::string& key) const
{
    auto it = _strings.find(key);
    return it != _strings.end() ? it->second : key;
}

void GlobalConfig::setLanguage(const std::string& lang)
{
    if (lang == _lang) return;
    _lang = lang;
    refreshStrings();
}

void GlobalConfig::refreshStrings()
{
    _strings.clear();
    auto it = _allStrings.find(_lang);
    if (it != _allStrings.end() && it->second.getType() == cocos2d::Value::Type::MAP)
    {
        for (const auto& pair : it->second.asValueMap())
            _strings[pair.first] = pair.second.asString();
    }
}
