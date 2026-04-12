#ifndef POKER_GAME_GAME_STATE_H
#define POKER_GAME_GAME_STATE_H

#include "CardDeck.h"
#include "CardSlot.h"
#include "LayoutConfig.h"
#include <vector>

class GameState
{
public:
    // 加载布局配置，初始化覆盖关系
    bool loadLayout(const std::string& layoutFilePath);

    // 用当前已加载的布局初始化牌局
    void initializeGame();
    void setVisibleTopCardCount(int count);
    int getVisibleTopCardCount() const;

    // 底牌堆
    const PokerCard& getCurrentTopCard() const;
    void drawFromReserve();
    void replaceOpenTopCard(int index, const PokerCard& newTop);
    const std::vector<PokerCard>& getOpenTopCards() const;
    void restoreOpenTopCards(const std::vector<PokerCard>& cards);
    bool hasReserveCards() const;
    int reserveDeckSize() const;

    int wastePileSize() const;

    // 回收废牌堆到底牌堆
    void recycleWastePile();

    // 回退用
    void returnCardToReserve(const PokerCard& card);
    void popFromWastePile();

    // 主牌区
    CardSlot& getSlot(int index);
    const CardSlot& getSlot(int index) const;
    int slotCount() const;
    PokerCard removeCardFromSlot(int index, std::vector<int>* revealedSlotIndices = nullptr);
    void restoreCardToSlot(int index, const PokerCard& card, const std::vector<int>& revealedSlotIndices);
    bool isSlotExposed(int index) const;

    // 布局信息（供 View 使用）
    const LayoutConfig& getLayoutConfig() const { return _layoutConfig; }

    bool isWin() const;

private:
    void buildCoverLinksFromLayout();

    LayoutConfig _layoutConfig;
    std::vector<CardSlot> _mainAreaSlots;
    std::vector<std::vector<int>> _coveringParents;
    std::vector<std::vector<int>> _coveredChildren;
    CardDeck _reserveDeck;
    std::vector<PokerCard> _openTopCards;
    int _visibleTopCardCount = 1;

    std::vector<PokerCard> _wastePile;
};

#endif
