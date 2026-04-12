#ifndef POKER_GAME_CARD_DECK_H
#define POKER_GAME_CARD_DECK_H

#include "PokerCard.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

class CardDeck
{
public:
    CardDeck();

    static CardDeck createMultipleStandardDecks(int deckCount);

    void shuffle();
    PokerCard dealCard();
    bool isEmpty() const;
    int size() const;
    void reset(const std::vector<PokerCard>& cards);
    void addCardToTop(const PokerCard& card);

private:
    std::vector<PokerCard> _cards;
};

#endif
