#include "MatchEngine.h"
#include <cmath>

bool MatchEngine::canMatch(const PokerCard& a, const PokerCard& b)
{
    int diff = std::abs(a.rankValue() - b.rankValue());
    return diff == 1;
}

std::pair<int, int> MatchEngine::getMatchingRanks(const PokerCard& topCard)
{
    int val = topCard.rankValue();
    int lower = (val > 1) ? val - 1 : -1;
    int upper = (val < 13) ? val + 1 : -1;
    return {lower, upper};
}
