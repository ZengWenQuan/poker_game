/**
 * @file CardSlot.cpp
 * @brief 卡槽数据模型实现。
 *
 * 主要功能:
 *   - pushCard: 压入卡牌
 *   - popCard: 弹出顶部卡牌
 *   - getTopCard: 获取顶部卡牌
 *   - 计算槽位覆盖关系
 */
#include "CardSlot.h"

CardSlot::CardSlot()
{
}

// 将牌压入槽位顶部，记录朝向。
void CardSlot::pushCard(const PokerCard& card, bool faceUp)
{
    // 新牌永远压到槽位顶部。
    _cards.push_back({card, faceUp});
}

// 弹出槽位顶部的牌。
PokerCard CardSlot::popCard()
{
    SlotCard sc = _cards.back();
    _cards.pop_back();
    return sc.card;
}

// 查看顶牌。
const PokerCard& CardSlot::topCard() const
{
    return _cards.back().card;
}

// 顶牌是否朝上。
bool CardSlot::isTopCardFaceUp() const
{
    if (_cards.empty()) return false;
    return _cards.back().faceUp;
}

// 设置顶牌朝向。
void CardSlot::setTopCardFaceUp(bool faceUp)
{
    if (!_cards.empty())
    {
        _cards.back().faceUp = faceUp;
    }
}

// 槽位是否为空。
bool CardSlot::isEmpty() const
{
    return _cards.empty();
}

// 槽位内牌数量。
int CardSlot::cardCount() const
{
    return static_cast<int>(_cards.size());
}

// 获取指定序号的牌（0 为底部）。
const PokerCard& CardSlot::getCard(int index) const
{
    return _cards[index].card;
}
