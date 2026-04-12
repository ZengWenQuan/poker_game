#ifndef POKER_GAME_JSON_CONFIG_H
#define POKER_GAME_JSON_CONFIG_H

#include "cocos2d.h"
#include <string>
#include <vector>

class JsonConfig
{
public:
    explicit JsonConfig(const std::string& filePath);

    bool isLoaded() const { return loaded; }
    const cocos2d::ValueMap& getData() const { return data; }

    // 点分路径取值
    const cocos2d::Value* lookup(const std::string& path) const;

    std::string getString(const std::string& path, const std::string& fb = "") const;
    int getInt(const std::string& path, int fb = 0) const;
    float getFloat(const std::string& path, float fb = 0.0f) const;
    cocos2d::Vec2 getVec2(const std::string& path, cocos2d::Vec2 fb = cocos2d::Vec2::ZERO) const;
    std::vector<std::string> getStringArray(const std::string& path) const;

    bool loaded = false;
    cocos2d::ValueMap data;
};

#endif
