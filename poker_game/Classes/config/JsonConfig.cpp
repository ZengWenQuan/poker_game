#include "JsonConfig.h"
#include "logging/GameLogger.h"
#include "json/document.h"
#include "json/istreamwrapper.h"
#include "platform/CCFileUtils.h"
#include <fstream>

namespace
{
cocos2d::Value convertJsonValue(const rapidjson::Value& value)
{
    using cocos2d::Value;
    using cocos2d::ValueMap;
    using cocos2d::ValueVector;

    if (value.IsObject())
    {
        ValueMap mapValue;
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
            mapValue[it->name.GetString()] = convertJsonValue(it->value);
        return Value(mapValue);
    }

    if (value.IsArray())
    {
        ValueVector vectorValue;
        vectorValue.reserve(value.Size());
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
            vectorValue.push_back(convertJsonValue(value[i]));
        return Value(vectorValue);
    }

    if (value.IsString()) return Value(value.GetString());
    if (value.IsBool())   return Value(value.GetBool());
    if (value.IsInt())    return Value(value.GetInt());
    if (value.IsUint())   return Value(static_cast<int>(value.GetUint()));
    if (value.IsNumber()) return Value(static_cast<float>(value.GetDouble()));
    return Value();
}
}

JsonConfig::JsonConfig(const std::string& filePath)
{
    const std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filePath);
    if (fullPath.empty())
    {
        GAME_LOG_ERROR("JsonConfig file not found: %s", filePath.c_str());
        return;
    }

    std::ifstream ifs(fullPath);
    if (!ifs.is_open())
    {
        GAME_LOG_ERROR("JsonConfig cannot open file: %s", fullPath.c_str());
        return;
    }

    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (doc.HasParseError() || !doc.IsObject())
    {
        GAME_LOG_ERROR("JsonConfig invalid json object: %s", filePath.c_str());
        return;
    }

    data = convertJsonValue(doc).asValueMap();
    loaded = true;
    GAME_LOG_INFO("JsonConfig loaded: %s", filePath.c_str());
}

const cocos2d::Value* JsonConfig::lookup(const std::string& path) const
{
    const cocos2d::ValueMap* cur = &data;
    const cocos2d::Value* val = nullptr;
    size_t start = 0;
    while (start < path.size())
    {
        size_t dot = path.find('.', start);
        std::string key = (dot == std::string::npos) ? path.substr(start) : path.substr(start, dot - start);
        auto it = cur->find(key);
        if (it == cur->end()) return nullptr;
        val = &it->second;
        if (dot == std::string::npos) return val;
        if (val->getType() != cocos2d::Value::Type::MAP) return nullptr;
        cur = &val->asValueMap();
        start = dot + 1;
    }
    return nullptr;
}

std::string JsonConfig::getString(const std::string& path, const std::string& fb) const
{
    auto* v = lookup(path);
    return v ? v->asString() : fb;
}

int JsonConfig::getInt(const std::string& path, int fb) const
{
    auto* v = lookup(path);
    return v ? v->asInt() : fb;
}

float JsonConfig::getFloat(const std::string& path, float fb) const
{
    auto* v = lookup(path);
    return v ? v->asFloat() : fb;
}

cocos2d::Vec2 JsonConfig::getVec2(const std::string& path, cocos2d::Vec2 fb) const
{
    auto* v = lookup(path);
    if (!v || v->getType() != cocos2d::Value::Type::VECTOR) return fb;
    const auto& arr = v->asValueVector();
    if (arr.size() < 2) return fb;
    return cocos2d::Vec2(arr[0].asFloat(), arr[1].asFloat());
}

std::vector<std::string> JsonConfig::getStringArray(const std::string& path) const
{
    std::vector<std::string> result;
    auto* v = lookup(path);
    if (!v || v->getType() != cocos2d::Value::Type::VECTOR) return result;
    for (const auto& item : v->asValueVector())
        result.push_back(item.asString());
    return result;
}
