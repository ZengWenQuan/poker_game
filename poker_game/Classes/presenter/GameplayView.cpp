/**
 * @file GameplayView.cpp
 * @brief 游戏玩法视图组合实现。
 *
 * 主要功能:
 *   - 委托 MainAreaView 和 TopAreaView 执行具体视图操作
 */
#include "presenter/GameplayView.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "view/CardSlotView.h"

GameplayView::GameplayView(MainAreaView* mainArea, TopAreaView* topArea)
    : _mainArea(mainArea)
    , _topArea(topArea)
{
}

void GameplayView::updateAllSlots(const std::vector<CardSlot>& slots)
{
    _mainArea->updateAllSlots(slots);
}

void GameplayView::updateHighlights(const std::vector<bool>& highlightStates)
{
    _mainArea->updateHighlights(highlightStates);
}

cocos2d::Vec2 GameplayView::getSlotTopCardWorldPosition(int slotIndex) const
{
    auto* slotView = _mainArea->getSlotView(slotIndex);
    return slotView ? slotView->getTopCardWorldPosition() : cocos2d::Vec2::ZERO;
}

cocos2d::Node* GameplayView::getSlotTopCardView(int slotIndex) const
{
    auto* slotView = _mainArea->getSlotView(slotIndex);
    return slotView ? slotView->getTopCardView() : nullptr;
}

void GameplayView::setOpenTopCards(const std::vector<PokerCard>& cards)
{
    _topArea->setOpenTopCards(cards);
}

void GameplayView::animateOpenTopCards(const std::vector<PokerCard>& cards)
{
    _topArea->animateOpenTopCards(cards);
}

void GameplayView::setReserveCount(int count)
{
    _topArea->setReserveCount(count);
}

void GameplayView::setWastePileCount(int count)
{
    _topArea->setWastePileCount(count);
}

cocos2d::Vec2 GameplayView::getTopCardWorldPosition() const
{
    return _topArea->getTopCardWorldPosition();
}

cocos2d::Vec2 GameplayView::getReserveDeckWorldPosition() const
{
    return _topArea->getReserveDeckWorldPosition();
}

cocos2d::Rect GameplayView::getTopCardWorldRect() const
{
    return _topArea->getTopCardWorldRect();
}
