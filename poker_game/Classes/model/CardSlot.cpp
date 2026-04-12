#include "CardSlot.h"

CardSlot::CardSlot()
{
}

void CardSlot::pushCard(const PokerCard& card, bool faceUp)
{
    _cards.push_back({card, faceUp});
}

PokerCard CardSlot::popCard()
{
    SlotCard sc = _cards.back();
    _cards.pop_back();
    return sc.card;
}

const PokerCard& CardSlot::topCard() const
{
    return _cards.back().card;
}

bool CardSlot::isTopCardFaceUp() const
{
    if (_cards.empty()) return false;
    return _cards.back().faceUp;
}

void CardSlot::setTopCardFaceUp(bool faceUp)
{
    if (!_cards.empty())
    {
        _cards.back().faceUp = faceUp;
    }
}

bool CardSlot::isEmpty() const
{
    return _cards.empty();
}

int CardSlot::cardCount() const
{
    return static_cast<int>(_cards.size());
}

const PokerCard& CardSlot::getCard(int index) const
{
    return _cards[index].card;
}
