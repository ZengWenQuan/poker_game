/**
 * @file GameplayView.h
 * @brief 游戏玩法视图组合实现头文件。
 *
 * 主要功能:
 *   - 组合 MainAreaView 和 TopAreaView 实现 IGameplayView
 *   - 提供 GameplayPresenter 所需的全部视图操作
 */
#ifndef POKER_GAME_GAMEPLAY_VIEW_H
#define POKER_GAME_GAMEPLAY_VIEW_H

#include "presenter/IGameplayView.h"

class MainAreaView;
class TopAreaView;

// 游戏玩法视图：组合主牌区和顶部区域，实现 IGameplayView 接口。
class GameplayView : public IGameplayView
{
public:
    GameplayView(MainAreaView* mainArea, TopAreaView* topArea);

    // IGameplayView 接口实现
    void updateAllSlots(const std::vector<CardSlot>& slots) override;
    void updateHighlights(const std::vector<bool>& highlightStates) override;
    cocos2d::Vec2 getSlotTopCardWorldPosition(int slotIndex) const override;
    cocos2d::Node* getSlotTopCardView(int slotIndex) const override;

    void setOpenTopCards(const std::vector<PokerCard>& cards) override;
    void animateOpenTopCards(const std::vector<PokerCard>& cards) override;
    void setReserveCount(int count) override;
    void setWastePileCount(int count) override;
    cocos2d::Vec2 getTopCardWorldPosition() const override;
    cocos2d::Vec2 getReserveDeckWorldPosition() const override;
    cocos2d::Rect getTopCardWorldRect() const override;

private:
    MainAreaView* _mainArea;
    TopAreaView* _topArea;
};

#endif
