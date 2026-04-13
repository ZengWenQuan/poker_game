#ifndef POKER_GAME_CUSTOM_LAYOUT_DRAFT_H
#define POKER_GAME_CUSTOM_LAYOUT_DRAFT_H

#include "cocos2d.h"
#include "model/LayoutConfig.h"
#include <string>
#include <vector>

struct CustomLayoutDraftCard
{
    cocos2d::Vec2 position;
    int zOrder = 0;
    bool isReward = false;
};

class CustomLayoutDraft
{
public:
    static std::vector<SlotLayout> buildSlots(const std::vector<CustomLayoutDraftCard>& cards,
                                              float cardWidth,
                                              float cardHeight,
                                              float designWidth,
                                              float designHeight);

    static bool hasUncoveredRewardCard(const std::vector<SlotLayout>& slots);

    static std::string buildSavePath(const std::string& currentLayoutPath,
                                     bool isNewLayout,
                                     const std::string& layoutName);

private:
    static std::string sanitizeFileName(const std::string& rawName);
};

#endif
