/**
 * @file IGameplayView.h
 * @brief 游戏玩法视图抽象接口。
 *
 * 主要功能:
 *   - 定义 GameplayPresenter 所需的视图操作契约
 *   - 解耦 Presenter 与具体 View 实现 (MainAreaView/TopAreaView)
 */
#ifndef POKER_GAME_IGAMEPLAY_VIEW_H
#define POKER_GAME_IGAMEPLAY_VIEW_H

#include "cocos2d.h"
#include "model/CardSlot.h"
#include "PokerCard.h"
#include <vector>

// 游戏玩法视图接口：Presenter 通过此接口操作视图，不直接依赖具体 View 类。
class IGameplayView
{
public:
    virtual ~IGameplayView() = default;

    // 主牌区操作
    virtual void updateAllSlots(const std::vector<CardSlot>& slots) = 0;
    virtual void updateHighlights(const std::vector<bool>& highlightStates) = 0;
    virtual cocos2d::Vec2 getSlotTopCardWorldPosition(int slotIndex) const = 0;
    virtual cocos2d::Node* getSlotTopCardView(int slotIndex) const = 0;

    // 顶部区域操作
    virtual void setOpenTopCards(const std::vector<PokerCard>& cards) = 0;
    virtual void animateOpenTopCards(const std::vector<PokerCard>& cards) = 0;
    virtual void setReserveCount(int count) = 0;
    virtual void setWastePileCount(int count) = 0;
    virtual cocos2d::Vec2 getTopCardWorldPosition() const = 0;
    virtual cocos2d::Vec2 getReserveDeckWorldPosition() const = 0;
    virtual cocos2d::Rect getTopCardWorldRect() const = 0;
};

#endif
