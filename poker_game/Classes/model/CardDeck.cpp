#include "CardDeck.h"

CardDeck::CardDeck()
{
}

// 构造多副标准牌的牌堆，不洗牌。
CardDeck CardDeck::createMultipleStandardDecks(int deckCount)
{
    CardDeck deck;
    const int safeDeckCount = std::max(1, deckCount);

    // 先顺序生成标准牌组，洗牌交给后续流程。
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

// 简单基于时间种子洗牌。
void CardDeck::shuffle()
{
    // 当前项目保持最简单实现：按时间种子洗牌。
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::random_shuffle(_cards.begin(), _cards.end());
}

// 从顶部发出一张牌。
PokerCard CardDeck::dealCard()
{
    // 统一从尾部发牌，和 push_back/pop_back 配套。
    PokerCard card = _cards.back();
    _cards.pop_back();
    return card;
}

// 牌堆是否为空。
bool CardDeck::isEmpty() const
{
    return _cards.empty();
}

// 当前牌数。
int CardDeck::size() const
{
    return static_cast<int>(_cards.size());
}

// 用指定牌组替换当前牌堆。
void CardDeck::reset(const std::vector<PokerCard>& cards)
{
    _cards = cards;
}

// 将一张牌压入顶部。
void CardDeck::addCardToTop(const PokerCard& card)
{
    _cards.push_back(card);
}
