/**
 * @file MatchEngine.h
 * @brief 匹配规则引擎头文件。
 *
 * 主要功能:
 *   - canMatch(a, b): 两张牌是否可匹配
 *   - findCoveredBy: 查找覆盖关系
 */
#ifndef POKER_GAME_MATCH_ENGINE_H
#define POKER_GAME_MATCH_ENGINE_H

#include "PokerCard.h"
#include <utility>

class MatchEngine
{
public:
    // 判断两张牌是否满足当前玩法规则。
    // 目前规则很简单：点数差 1 即可，不处理 A/K 环绕。
    static bool canMatch(const PokerCard& cardA, const PokerCard& cardB);

    // 给定顶部牌，返回理论上可以匹配的上下相邻点数；不存在则返回 -1。
    static std::pair<int, int> getMatchingRanks(const PokerCard& topCard);
};

#endif
