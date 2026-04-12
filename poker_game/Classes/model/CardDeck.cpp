#include "CardDeck.h"

CardDeck::CardDeck()
{
}

CardDeck CardDeck::createMultipleStandardDecks(int deckCount)
{
    CardDeck deck;
    const int safeDeckCount = std::max(1, deckCount);
    for (int copy = 0; copy < safeDeckCount; ++copy)
    {
        for (int s = 0; s < 4; ++s)
        {
            for (int r = 1; r <= 13; ++r)
            {
                deck._cards.emplace_back(static_cast<CardSuit>(s), static_cast<CardRank>(r));
            }
        }
    }
    return deck;
}

void CardDeck::shuffle()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::random_shuffle(_cards.begin(), _cards.end());
}

PokerCard CardDeck::dealCard()
{
    PokerCard card = _cards.back();
    _cards.pop_back();
    return card;
}

bool CardDeck::isEmpty() const
{
    return _cards.empty();
}

int CardDeck::size() const
{
    return static_cast<int>(_cards.size());
}

void CardDeck::reset(const std::vector<PokerCard>& cards)
{
    _cards = cards;
}

void CardDeck::addCardToTop(const PokerCard& card)
{
    _cards.push_back(card);
}
