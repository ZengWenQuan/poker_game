#ifndef POKER_GAME_CARD_DECK_H
#define POKER_GAME_CARD_DECK_H

#include "PokerCard.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// 牌堆模型：维护一组牌并提供洗牌、发牌等操作。
class CardDeck
{
public:
    // 构造空牌堆，调用方可后续重置或追加。
    CardDeck();

    // 创建若干副标准扑克牌，用于主牌区牌量超过 52 张时自动扩容。
    // deckCount: 需要的整副数量。
    static CardDeck createMultipleStandardDecks(int deckCount);

    // Fisher-Yates 洗牌。
    void shuffle();
    // 从顶部发出一张牌并移除。
    PokerCard dealCard();
    // 牌堆是否为空。
    bool isEmpty() const;
    // 当前剩余牌数。
    int size() const;
    // 用指定牌组重置牌堆。
    // cards: 新的牌序列，尾部视为顶部。
    void reset(const std::vector<PokerCard>& cards);
    // 将一张牌压入顶部。
    // card: 要入堆的牌。
    void addCardToTop(const PokerCard& card);

private:
    std::vector<PokerCard> _cards; // 约定尾部为牌堆顶部
};

#endif
