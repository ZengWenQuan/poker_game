#include "model/CustomLayoutDraft.h"

#include <algorithm>
#include <cctype>

namespace
{
float computeOverlapArea(const cocos2d::Rect& lhs, const cocos2d::Rect& rhs)
{
    const float left = std::max(lhs.getMinX(), rhs.getMinX());
    const float right = std::min(lhs.getMaxX(), rhs.getMaxX());
    const float bottom = std::max(lhs.getMinY(), rhs.getMinY());
    const float top = std::min(lhs.getMaxY(), rhs.getMaxY());
    if (right <= left || top <= bottom)
    {
        return 0.0f;
    }
    return (right - left) * (top - bottom);
}
}

std::vector<SlotLayout> CustomLayoutDraft::buildSlots(const std::vector<CustomLayoutDraftCard>& cards,
                                                      float cardWidth,
                                                      float cardHeight,
                                                      float designWidth,
                                                      float designHeight)
{
    std::vector<SlotLayout> slots;
    slots.reserve(cards.size());

    const float centerX = designWidth * 0.5f;
    const float centerY = designHeight * 0.5f;

    for (int i = 0; i < static_cast<int>(cards.size()); ++i)
    {
        SlotLayout slot;
        slot.id = i;
        slot.pileIndex = 0;
        slot.rotation = 0.0f;
        slot.layer = 0;
        slot.x = cards[i].position.x - centerX;
        slot.y = cards[i].position.y - centerY;
        slot.isReward = cards[i].isReward;
        slots.push_back(slot);
    }

    std::vector<cocos2d::Rect> bounds;
    std::vector<int> zOrders;
    bounds.reserve(cards.size());
    zOrders.reserve(cards.size());

    for (const auto& card : cards)
    {
        bounds.emplace_back(card.position.x - cardWidth * 0.5f,
                            card.position.y - cardHeight * 0.5f,
                            cardWidth,
                            cardHeight);
        zOrders.push_back(card.zOrder);
    }

    std::vector<std::vector<int>> overlappingParents(slots.size());
    for (int lower = 0; lower < static_cast<int>(slots.size()); ++lower)
    {
        for (int upper = 0; upper < static_cast<int>(slots.size()); ++upper)
        {
            if (lower == upper || zOrders[upper] <= zOrders[lower]) continue;
            if (computeOverlapArea(bounds[lower], bounds[upper]) > 1.0f)
            {
                overlappingParents[lower].push_back(upper);
            }
        }
    }

    std::vector<int> order(slots.size());
    for (int i = 0; i < static_cast<int>(order.size()); ++i)
    {
        order[i] = i;
    }
    std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
        return zOrders[lhs] > zOrders[rhs];
    });

    for (int index : order)
    {
        for (int parentIndex : overlappingParents[index])
        {
            slots[index].layer = std::max(slots[index].layer, slots[parentIndex].layer + 1);
        }
    }

    for (auto& slot : slots)
    {
        slot.covers.clear();
    }

    for (int lower = 0; lower < static_cast<int>(slots.size()); ++lower)
    {
        for (int upper : overlappingParents[lower])
        {
            if (slots[upper].layer == slots[lower].layer - 1)
            {
                slots[lower].covers.push_back(slots[upper].id);
            }
        }
    }

    return slots;
}

bool CustomLayoutDraft::hasUncoveredRewardCard(const std::vector<SlotLayout>& slots)
{
    for (const auto& slot : slots)
    {
        if (slot.isReward && slot.covers.empty())
        {
            return true;
        }
    }
    return false;
}

std::string CustomLayoutDraft::buildSavePath(const std::string& currentLayoutPath,
                                             bool isNewLayout,
                                             const std::string& layoutName)
{
    if (isNewLayout || currentLayoutPath.empty())
    {
        return "layouts/" + sanitizeFileName(layoutName) + ".json";
    }

    if (currentLayoutPath.rfind("layouts/", 0) == 0)
    {
        return currentLayoutPath;
    }

    const size_t slashPos = currentLayoutPath.find_last_of("/\\");
    const std::string fileName = slashPos == std::string::npos
        ? sanitizeFileName(layoutName) + ".json"
        : currentLayoutPath.substr(slashPos + 1);
    return "layouts/" + fileName;
}

std::string CustomLayoutDraft::sanitizeFileName(const std::string& rawName)
{
    std::string result;
    result.reserve(rawName.size());

    for (unsigned char ch : rawName)
    {
        if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' || ch == '?' ||
            ch == '"' || ch == '<' || ch == '>' || ch == '|')
        {
            result.push_back('_');
        }
        else if (std::isspace(ch))
        {
            result.push_back('_');
        }
        else
        {
            result.push_back(static_cast<char>(ch));
        }
    }

    while (!result.empty() && result.front() == '_')
    {
        result.erase(result.begin());
    }
    while (!result.empty() && result.back() == '_')
    {
        result.pop_back();
    }

    if (result.empty())
    {
        result = "custom_layout";
    }

    return result;
}
