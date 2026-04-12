#ifndef POKER_GAME_JSON_CONFIG_H
#define POKER_GAME_JSON_CONFIG_H

#include "cocos2d.h"
#include <string>
#include <vector>

// JSON 配置读取器。
// 负责把 JSON 文件解析成 cocos2d::ValueMap，并提供按路径访问的轻量接口。
class JsonConfig
{
public:
    // 构造并加载指定 JSON 文件。
    // `filePath` 是相对 Resources 的配置文件路径。
    explicit JsonConfig(const std::string& filePath);

    // 返回配置文件是否已成功加载。
    bool isLoaded() const { return loaded; }
    // 返回解析后的根对象，适合需要直接遍历配置的场景。
    const cocos2d::ValueMap& getData() const { return data; }

    // 点分路径取值，例如 "ui.statusPositionRatio"。
    // `path` 是以 '.' 分隔的层级路径。
    const cocos2d::Value* lookup(const std::string& path) const;

    // 一组轻量访问器，统一做默认值回退。
    // `path` 是配置路径，`fb` 是找不到或类型不符时使用的回退值。
    std::string getString(const std::string& path, const std::string& fb = "") const;
    // `path` 是配置路径，`fb` 是找不到或类型不符时使用的回退值。
    int getInt(const std::string& path, int fb = 0) const;
    // `path` 是配置路径，`fb` 是找不到或类型不符时使用的回退值。
    float getFloat(const std::string& path, float fb = 0.0f) const;
    // `path` 是配置路径，`fb` 是找不到或格式不对时使用的回退坐标。
    cocos2d::Vec2 getVec2(const std::string& path, cocos2d::Vec2 fb = cocos2d::Vec2::ZERO) const;
    // `path` 是配置路径，返回字符串数组；找不到时返回空数组。
    std::vector<std::string> getStringArray(const std::string& path) const;

    bool loaded = false;    // 文件是否成功解析
    cocos2d::ValueMap data; // 解析后的根对象
};

#endif
