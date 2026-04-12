#ifndef POKER_GAME_CARD_SLOT_H
#define POKER_GAME_CARD_SLOT_H

#include "PokerCard.h"
#include <vector>

class CardSlot
{
public:
    CardSlot();

    void pushCard(const PokerCard& card, bool faceUp = false);
    PokerCard popCard();
    const PokerCard& topCard() const;
    bool isTopCardFaceUp() const;
    void setTopCardFaceUp(bool faceUp);
    bool isEmpty() const;
    int cardCount() const;
    const PokerCard& getCard(int index) const;

private:
    struct SlotCard
    {
        PokerCard card;
        bool faceUp;
    };
    std::vector<SlotCard> _cards;
};

#endif
