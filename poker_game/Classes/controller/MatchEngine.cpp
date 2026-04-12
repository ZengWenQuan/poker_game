#include "MatchEngine.h"
#include <cmath>

// 判断两张牌是否可匹配：点数相邻即可。
bool MatchEngine::canMatch(const PokerCard& a, const PokerCard& b)
{
    // 当前玩法不区分花色，只根据点数相邻判断是否可匹配。
    int diff = std::abs(a.rankValue() - b.rankValue());
    return diff == 1;
}

// 给定顶部牌，返回可匹配的点数范围。
std::pair<int, int> MatchEngine::getMatchingRanks(const PokerCard& topCard)
{
    // 供提示或高亮逻辑快速反推出目标点数范围。
    int val = topCard.rankValue();
    int lower = (val > 1) ? val - 1 : -1;
    int upper = (val < 13) ? val + 1 : -1;
    return {lower, upper};
}
