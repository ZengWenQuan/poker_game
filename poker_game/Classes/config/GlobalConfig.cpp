#include "GlobalConfig.h"
#include "JsonConfig.h"

namespace
{
// 单例实例指针。
GlobalConfig* s_instance = nullptr;
}

GlobalConfig::GlobalConfig()
{
    // 配置按职责拆成三类文件，构造时一次性读入内存。
    JsonConfig game("config/game_config.json");
    JsonConfig theme("config/theme.json");
    JsonConfig strings("config/strings.json");

    // Window
    _windowTitle  = game.getString("window.title", "poker_game");
    _designWidth  = game.getFloat("window.designWidth", 1920.0f);
    _designHeight = game.getFloat("window.designHeight", 1080.0f);
    _windowScale  = game.getFloat("window.scale", 0.45f);

    // Card
    _cardSourceWidth  = game.getFloat("card.sourceWidth", 182.0f);
    _cardSourceHeight = game.getFloat("card.sourceHeight", 282.0f);
    _cardWidthRatioToBackground = game.getFloat("card.widthRatioToBackground", 0.05f);

    // Interaction
    _doubleClickIntervalMs = game.getInt("interaction.doubleClickIntervalMs", 400);
    _dragThresholdPx       = game.getFloat("interaction.dragThresholdPx", 10.0f);
    _dropAreaPaddingRatio  = game.getFloat("interaction.dropAreaPaddingRatio", 0.5f);
    _reserveHitPadding     = game.getInt("interaction.reserveHitPadding", 20);
    {
        auto* v = game.lookup("interaction.recycleHitRect");
        if (v && v->getType() == cocos2d::Value::Type::VECTOR && v->asValueVector().size() >= 4)
        {
            // 回收按钮命中区允许比文字本身更大，提升交互容错。
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

    // Layouts
    _layoutFiles        = game.getStringArray("layouts");
    _defaultLayoutIndex = game.getInt("defaultLayoutIndex", 0);

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
                    // 颜色统一用 RGBA 数组表达。
                    _colors[pair.first] = cocos2d::Color4B(arr[0].asInt(), arr[1].asInt(), arr[2].asInt(), a);
                }
            }
        }
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

    // Strings (默认英语)
    auto* langMap = strings.lookup("en");
    if (langMap && langMap->getType() == cocos2d::Value::Type::MAP)
    {
        for (const auto& pair : langMap->asValueMap())
            _strings[pair.first] = pair.second.asString();
    }

}

GlobalConfig& GlobalConfig::getInstance()
{
    // 全局配置只构造一次，后续按单例复用。
    if (!s_instance)
        s_instance = new GlobalConfig();
    return *s_instance;
}

float GlobalConfig::getFontSize(const std::string& key) const
{
    // `key` 用来索引字体尺寸配置；缺失时回退到默认字号。
    auto it = _fontSizes.find(key);
    return it != _fontSizes.end() ? it->second : 28.0f;
}

cocos2d::Color4B GlobalConfig::getColor4B(const std::string& key) const
{
    // `key` 用来索引颜色配置；缺失时回退为白色。
    auto it = _colors.find(key);
    return it != _colors.end() ? it->second : cocos2d::Color4B::WHITE;
}

cocos2d::Color3B GlobalConfig::getColor3B(const std::string& key) const
{
    // `key` 用来索引颜色配置；这里只保留 RGB 分量。
    auto it = _colors.find(key);
    return it != _colors.end() ? cocos2d::Color3B(it->second.r, it->second.g, it->second.b) : cocos2d::Color3B::WHITE;
}

const std::string& GlobalConfig::getImage(const std::string& key) const
{
    // `key` 表示图片资源名称，找不到时返回 key 本身，便于上层继续排查。
    auto it = _images.find(key);
    return it != _images.end() ? it->second : key;
}

std::string GlobalConfig::getImageFormatted(const std::string& key, const std::string& color, const std::string& rank) const
{
    // `key` 表示图片模板名，`color` 和 `rank` 分别替换花色与点数占位符。
    auto it = _images.find(key);
    if (it == _images.end()) return key;
    std::string result = it->second;
    // 支持把 {color} / {rank} 占位符替换成实际资源片段。
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
    // `key` 表示文案标识，找不到时直接返回 key，避免界面出现空串。
    auto it = _strings.find(key);
    return it != _strings.end() ? it->second : key;
}
