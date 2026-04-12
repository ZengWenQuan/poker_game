#include "LayoutConfig.h"
#include "config/GlobalConfig.h"
#include "logging/GameLogger.h"
#include "view/PokerCardView.h"
#include "json/document.h"
#include "json/istreamwrapper.h"
#include "platform/CCFileUtils.h"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <array>
#include <cmath>

namespace
{
using cocos2d::Vec2;

Vec2 resolveSlotPosition(const SlotLayout& slot,
                         bool usePileCenterOrigin,
                         int mainPileCount,
                         float areaWidth,
                         float areaHeight)
{
    if (!usePileCenterOrigin)
    {
        return Vec2(slot.x, slot.y);
    }

    const int pileCount = std::max(1, mainPileCount);
    const int pileIndex = std::max(0, std::min(slot.pileIndex, pileCount - 1));
    const float sectionWidth = areaWidth / pileCount;
    const float centerX = sectionWidth * (static_cast<float>(pileIndex) + 0.5f);
    const float centerY = areaHeight * 0.5f;
    return Vec2(centerX + slot.x, centerY + slot.y);
}

Vec2 rotateAround(const Vec2& point, const Vec2& pivot, float clockwiseDegrees)
{
    const float radians = -clockwiseDegrees * 3.14159265358979323846f / 180.0f;
    const float s = std::sin(radians);
    const float c = std::cos(radians);
    const Vec2 offset = point - pivot;
    return Vec2(pivot.x + offset.x * c - offset.y * s,
                pivot.y + offset.x * s + offset.y * c);
}

std::array<Vec2, 4> buildCardPolygon(const SlotLayout& slot, bool usePileCenterOrigin, int mainPileCount)
{
    auto& cfg = GlobalConfig::getInstance();
    const Vec2 center = resolveSlotPosition(slot,
                                            usePileCenterOrigin,
                                            mainPileCount,
                                            cfg.getDesignWidth(),
                                            cfg.getDesignHeight());
    const float cardWidth = PokerCardView::getCardWidth();
    const float cardHeight = PokerCardView::getCardHeight();
    const Vec2 bottomLeft(center.x - cardWidth * 0.5f, center.y - cardHeight * 0.5f);

    std::array<Vec2, 4> corners = {
        bottomLeft,
        Vec2(bottomLeft.x + cardWidth, bottomLeft.y),
        Vec2(bottomLeft.x + cardWidth, bottomLeft.y + cardHeight),
        Vec2(bottomLeft.x, bottomLeft.y + cardHeight)
    };

    if (slot.rotation == 0.0f)
    {
        return corners;
    }

    for (auto& corner : corners)
    {
        corner = rotateAround(corner, bottomLeft, slot.rotation);
    }
    return corners;
}

float cross(const Vec2& a, const Vec2& b, const Vec2& c)
{
    const Vec2 ab = b - a;
    const Vec2 ap = c - a;
    return ab.cross(ap);
}

bool onSegment(const Vec2& a, const Vec2& b, const Vec2& p)
{
    constexpr float kEpsilon = 0.001f;
    return p.x >= std::min(a.x, b.x) - kEpsilon &&
           p.x <= std::max(a.x, b.x) + kEpsilon &&
           p.y >= std::min(a.y, b.y) - kEpsilon &&
           p.y <= std::max(a.y, b.y) + kEpsilon;
}

int orientation(const Vec2& a, const Vec2& b, const Vec2& c)
{
    constexpr float kEpsilon = 0.001f;
    const float value = cross(a, b, c);
    if (value > kEpsilon) return 1;
    if (value < -kEpsilon) return -1;
    return 0;
}

bool segmentsIntersect(const Vec2& a1, const Vec2& a2, const Vec2& b1, const Vec2& b2)
{
    const int o1 = orientation(a1, a2, b1);
    const int o2 = orientation(a1, a2, b2);
    const int o3 = orientation(b1, b2, a1);
    const int o4 = orientation(b1, b2, a2);

    if (o1 != o2 && o3 != o4)
    {
        return true;
    }

    if (o1 == 0 && onSegment(a1, a2, b1)) return true;
    if (o2 == 0 && onSegment(a1, a2, b2)) return true;
    if (o3 == 0 && onSegment(b1, b2, a1)) return true;
    if (o4 == 0 && onSegment(b1, b2, a2)) return true;
    return false;
}

bool pointInConvexQuad(const Vec2& point, const std::array<Vec2, 4>& quad)
{
    bool hasPositive = false;
    bool hasNegative = false;
    constexpr float kEpsilon = 0.001f;

    for (int i = 0; i < 4; ++i)
    {
        const float value = cross(quad[i], quad[(i + 1) % 4], point);
        if (value > kEpsilon) hasPositive = true;
        if (value < -kEpsilon) hasNegative = true;
        if (hasPositive && hasNegative) return false;
    }
    return true;
}

bool polygonsOverlap(const std::array<Vec2, 4>& lhs, const std::array<Vec2, 4>& rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        const Vec2& lhsA = lhs[i];
        const Vec2& lhsB = lhs[(i + 1) % 4];
        for (int j = 0; j < 4; ++j)
        {
            const Vec2& rhsA = rhs[j];
            const Vec2& rhsB = rhs[(j + 1) % 4];
            if (segmentsIntersect(lhsA, lhsB, rhsA, rhsB))
            {
                return true;
            }
        }
    }

    if (pointInConvexQuad(lhs[0], rhs)) return true;
    if (pointInConvexQuad(rhs[0], lhs)) return true;
    return false;
}

void rebuildDynamicCovers(std::vector<SlotLayout>& slots, bool usePileCenterOrigin, int mainPileCount)
{
    std::vector<std::array<Vec2, 4>> polygons;
    polygons.reserve(slots.size());
    for (const auto& slot : slots)
    {
        polygons.push_back(buildCardPolygon(slot, usePileCenterOrigin, mainPileCount));
    }

    for (auto& slot : slots)
    {
        slot.covers.clear();
    }

    for (int lowerIndex = 0; lowerIndex < static_cast<int>(slots.size()); ++lowerIndex)
    {
        const auto& lowerSlot = slots[lowerIndex];
        const auto& lowerPolygon = polygons[lowerIndex];

        for (int upperIndex = 0; upperIndex < static_cast<int>(slots.size()); ++upperIndex)
        {
            const auto& upperSlot = slots[upperIndex];
            if (upperSlot.layer >= lowerSlot.layer) continue;

            if (polygonsOverlap(lowerPolygon, polygons[upperIndex]))
            {
                slots[lowerIndex].covers.push_back(upperSlot.id);
            }
        }
    }
}
}

void LayoutConfig::rebuildDynamicCoversForSlots(std::vector<SlotLayout>& slots,
                                                bool usePileCenterOrigin,
                                                int mainPileCount)
{
    rebuildDynamicCovers(slots, usePileCenterOrigin, mainPileCount);
}

cocos2d::Vec2 LayoutConfig::resolveSlotPosition(const SlotLayout& slot,
                                                bool usePileCenterOrigin,
                                                int mainPileCount,
                                                float areaWidth,
                                                float areaHeight)
{
    return ::resolveSlotPosition(slot, usePileCenterOrigin, mainPileCount, areaWidth, areaHeight);
}

bool LayoutConfig::loadFromFile(const std::string& filePath)
{
    std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filePath);
    if (fullPath.empty())
    {
        GAME_LOG_ERROR("LayoutConfig file not found: %s", filePath.c_str());
        return false;
    }

    std::ifstream ifs(fullPath);
    if (!ifs.is_open())
    {
        GAME_LOG_ERROR("LayoutConfig cannot open file: %s", fullPath.c_str());
        return false;
    }

    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);

    if (doc.HasParseError())
    {
        GAME_LOG_ERROR("LayoutConfig JSON parse error: %s", filePath.c_str());
        return false;
    }

    if (!doc.HasMember("name") || !doc.HasMember("totalSlots") || !doc.HasMember("slots"))
    {
        GAME_LOG_ERROR("LayoutConfig missing required fields: %s", filePath.c_str());
        return false;
    }

    _name = doc["name"].GetString();
    _totalSlots = doc["totalSlots"].GetInt();
    _mainPileCount = 1;
    _usePileCenterOrigin = false;
    if (doc.HasMember("mainPileCount") && doc["mainPileCount"].IsInt())
    {
        _mainPileCount = std::max(1, doc["mainPileCount"].GetInt());
        _usePileCenterOrigin = true;
    }
    _slots.clear();

    const auto& slotsArr = doc["slots"];
    bool hasLayerField = false;
    for (rapidjson::SizeType i = 0; i < slotsArr.Size(); ++i)
    {
        const auto& slotObj = slotsArr[i];
        SlotLayout layout;
        layout.id = slotObj["id"].GetInt();
        layout.x = slotObj["x"].GetFloat();
        layout.y = slotObj["y"].GetFloat();
        layout.pileIndex = slotObj.HasMember("pileIndex") ? slotObj["pileIndex"].GetInt() : 0;
        layout.layer = slotObj.HasMember("layer") ? slotObj["layer"].GetInt() : 0;
        hasLayerField = hasLayerField || slotObj.HasMember("layer");
        layout.rotation = slotObj.HasMember("rotation") ? slotObj["rotation"].GetFloat() : 0.0f;

        if (slotObj.HasMember("covers") && slotObj["covers"].IsArray())
        {
            const auto& coversArr = slotObj["covers"];
            for (rapidjson::SizeType j = 0; j < coversArr.Size(); ++j)
            {
                layout.covers.push_back(coversArr[j].GetInt());
            }
        }

        _slots.push_back(layout);
    }

    const bool shouldDuplicateTemplate =
        _mainPileCount > 1 &&
        !_slots.empty() &&
        std::all_of(_slots.begin(), _slots.end(), [](const SlotLayout& slot) {
            return slot.pileIndex == 0;
        });

    if (shouldDuplicateTemplate)
    {
        const std::vector<SlotLayout> templateSlots = _slots;
        std::unordered_map<int, int> idToTemplateIndex;
        idToTemplateIndex.reserve(templateSlots.size());

        for (int i = 0; i < static_cast<int>(templateSlots.size()); ++i)
        {
            idToTemplateIndex[templateSlots[i].id] = i;
        }

        _slots.clear();
        _slots.reserve(templateSlots.size() * _mainPileCount);

        for (int pile = 0; pile < _mainPileCount; ++pile)
        {
            for (int i = 0; i < static_cast<int>(templateSlots.size()); ++i)
            {
                SlotLayout layout = templateSlots[i];
                layout.id = pile * static_cast<int>(templateSlots.size()) + i;
                layout.pileIndex = pile;

                std::vector<int> remappedCovers;
                remappedCovers.reserve(layout.covers.size());
                for (int coveredId : layout.covers)
                {
                    auto it = idToTemplateIndex.find(coveredId);
                    if (it != idToTemplateIndex.end())
                    {
                        remappedCovers.push_back(pile * static_cast<int>(templateSlots.size()) + it->second);
                    }
                }
                layout.covers = std::move(remappedCovers);
                _slots.push_back(layout);
            }
        }
    }

    if (hasLayerField)
    {
        rebuildDynamicCoversForSlots(_slots, _usePileCenterOrigin, _mainPileCount);
        GAME_LOG_INFO("Dynamic covers rebuilt for layout=%s slots=%d mainPileCount=%d",
                      _name.c_str(), static_cast<int>(_slots.size()), _mainPileCount);
    }

    _totalSlots = static_cast<int>(_slots.size());
    GAME_LOG_INFO("Layout loaded: name=%s totalSlots=%d mainPileCount=%d usePileCenterOrigin=%d",
                  _name.c_str(), _totalSlots, _mainPileCount, _usePileCenterOrigin ? 1 : 0);

    return true;
}

std::vector<LayoutConfig::LayoutInfo> LayoutConfig::getAvailableLayouts()
{
    std::vector<LayoutInfo> layouts;
    auto* fileUtils = cocos2d::FileUtils::getInstance();
    std::vector<std::string> candidateFiles;
    std::set<std::string> seenFiles;

    auto appendIfJson = [&](const std::string& filePath) {
        if (filePath.size() < 5 || filePath.substr(filePath.size() - 5) != ".json")
        {
            return;
        }
        if (seenFiles.insert(filePath).second)
        {
            candidateFiles.push_back(filePath);
        }
    };

    auto& cfg = GlobalConfig::getInstance();
    for (const auto& filePath : cfg.getLayoutFiles())
    {
        appendIfJson(filePath);
    }

    const std::vector<std::string> layoutDirs = {
        "config/layouts/",
        "layouts/"
    };

    for (const auto& dir : layoutDirs)
    {
        for (const auto& filePath : fileUtils->listFiles(dir))
        {
            appendIfJson(filePath);
        }
    }

    for (const auto& filePath : candidateFiles)
    {
        std::string fullPath = fileUtils->fullPathForFilename(filePath);
        if (fullPath.empty()) continue;

        std::ifstream ifs(fullPath);
        if (!ifs.is_open()) continue;

        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document doc;
        doc.ParseStream(isw);

        if (doc.HasParseError()) continue;
        if (!doc.HasMember("name")) continue;

        LayoutInfo info;
        info.filePath = filePath;
        info.name = doc["name"].GetString();
        layouts.push_back(info);
    }

    std::sort(layouts.begin(), layouts.end(), [](const LayoutInfo& lhs, const LayoutInfo& rhs) {
        return lhs.name < rhs.name;
    });

    return layouts;
}
