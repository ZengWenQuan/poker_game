/**
 * @file CardSlot.h
 * @brief 卡槽数据模型头文件。
 *
 * 主要功能:
 *   - 卡槽中的卡牌列表、朝向管理
 *   - covers/coveredBy: 覆盖关系查询
 */
#ifndef POKER_GAME_CARD_SLOT_H
#define POKER_GAME_CARD_SLOT_H

#include "PokerCard.h"
#include <vector>

// 单个主牌区槽位：维护一列叠放的牌及其朝向。
class CardSlot
{
public:
    CardSlot();

    // 单个槽位只负责维护“叠起来的一列牌”和顶牌朝向。
    // card: 要压入的牌；faceUp: 压入时是否翻开。
    void pushCard(const PokerCard& card, bool faceUp = false);
    // 弹出顶牌并返回。
    PokerCard popCard();
    // 查看顶牌（不移除）。
    const PokerCard& topCard() const;
    // 顶牌是否朝上。
    bool isTopCardFaceUp() const;
    // 设置顶牌朝向。
    void setTopCardFaceUp(bool faceUp);
    // 槽位是否无牌。
    bool isEmpty() const;
    // 返回槽位内牌数量。
    int cardCount() const;
    // 访问指定序号的牌（0 为底部）。
    // index: 目标索引。
    const PokerCard& getCard(int index) const;

private:
    struct SlotCard
    {
        PokerCard card; // 牌对象
        bool faceUp;    // 当前在槽位中的朝向
    };
    std::vector<SlotCard> _cards;
};

#endif
