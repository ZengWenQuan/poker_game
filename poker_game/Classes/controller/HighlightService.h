/**
 * @file HighlightService.h
 * @brief 高亮计算服务头文件。
 *
 * 主要功能:
 *   - 根据当前游戏状态计算各槽位是否可匹配
 *   - 供视图层查询高亮状态
 */
#ifndef POKER_GAME_HIGHLIGHT_SERVICE_H
#define POKER_GAME_HIGHLIGHT_SERVICE_H

#include "model/GameState.h"
#include "MatchEngine.h"
#include <vector>

// 高亮计算服务：从 GameController 中抽出的职责，专注判断槽位是否可匹配。
class HighlightService
{
public:
    explicit HighlightService(const GameState& state);

    // 判断指定槽位是否可与任意顶部明牌匹配。
    bool isSlotMatchable(int slotIndex) const;

    // 返回所有槽位是否可匹配的布尔数组，索引与槽位对齐。
    std::vector<bool> getHighlightStates() const;

private:
    const GameState& _state;
};

#endif
