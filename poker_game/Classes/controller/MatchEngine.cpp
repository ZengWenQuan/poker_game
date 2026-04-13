/**
 * @file MatchEngine.cpp
 * @brief 匹配规则引擎实现。
 *
 * 主要功能:
 *   - canMatch: 判断两张牌是否可匹配 (点数相差1)
 *   - findCoveredBy: 查找被指定牌覆盖的所有牌
 */
#include "MatchEngine.h"
#include "config/GlobalConfig.h"
#include <cmath>

bool MatchEngine::canMatch(const PokerCard& a, const PokerCard& b)
{
    int diff = std::abs(a.rankValue() - b.rankValue());
    int matchDiff = GlobalConfig::getInstance().getMatchRankDiff();
    return diff == matchDiff;
}

std::pair<int, int> MatchEngine::getMatchingRanks(const PokerCard& topCard)
{
    int val = topCard.rankValue();
    int matchDiff = GlobalConfig::getInstance().getMatchRankDiff();
    int lower = (val > matchDiff) ? val - matchDiff : -1;
    int upper = (val < 14 - matchDiff) ? val + matchDiff : -1;
    return {lower, upper};
}
