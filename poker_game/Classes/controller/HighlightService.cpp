/**
 * @file HighlightService.cpp
 * @brief 高亮计算服务实现。
 *
 * 主要功能:
 *   - 遍历槽位与顶部明牌，判定匹配关系
 *   - 生成高亮状态数组供视图使用
 */
#include "HighlightService.h"

HighlightService::HighlightService(const GameState& state)
    : _state(state)
{
}

bool HighlightService::isSlotMatchable(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= _state.slotCount()) return false;
    const auto& slot = _state.getSlot(slotIndex);
    if (slot.isEmpty() || !slot.isTopCardFaceUp()) return false;
    if (slot.topCard().isReward()) return false;

    for (const auto& topCard : _state.getOpenTopCards())
    {
        if (MatchEngine::canMatch(slot.topCard(), topCard))
        {
            return true;
        }
    }
    return false;
}

std::vector<bool> HighlightService::getHighlightStates() const
{
    std::vector<bool> highlightStates;
    highlightStates.reserve(_state.slotCount());

    for (int i = 0; i < _state.slotCount(); ++i)
    {
        highlightStates.push_back(isSlotMatchable(i));
    }

    return highlightStates;
}
