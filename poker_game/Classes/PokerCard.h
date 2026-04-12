#ifndef POKER_GAME_POKER_CARD_H
#define POKER_GAME_POKER_CARD_H

#include "CardTypes.h"
#include <string>

class PokerCard
{
public:
    PokerCard(CardSuit suit = CardSuit::Spade, CardRank rank = CardRank::Ace);

    bool isRedSuit() const;
    std::string rankToken() const;
    std::string suitTexturePath() const;
    std::string bigRankTexturePath() const;
    std::string smallRankTexturePath() const;

    // 新增方法
    int rankValue() const;
    bool operator==(const PokerCard& other) const;
    bool operator!=(const PokerCard& other) const;

private:
    CardSuit _suit {CardSuit::Spade};
    CardRank _rank {CardRank::Ace};
};

#endif
