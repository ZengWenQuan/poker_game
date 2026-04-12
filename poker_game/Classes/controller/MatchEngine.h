#ifndef POKER_GAME_MATCH_ENGINE_H
#define POKER_GAME_MATCH_ENGINE_H

#include "PokerCard.h"
#include <utility>

class MatchEngine
{
public:
    static bool canMatch(const PokerCard& cardA, const PokerCard& cardB);
    static std::pair<int, int> getMatchingRanks(const PokerCard& topCard);
};

#endif
