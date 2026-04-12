#ifndef POKER_GAME_TOP_AREA_VIEW_H
#define POKER_GAME_TOP_AREA_VIEW_H

#include "PokerCardView.h"
#include "PokerCard.h"
#include <functional>
#include <vector>

class TopAreaView : public cocos2d::Node
{
public:
    static TopAreaView* create();

    bool init() override;

    void setOpenTopCards(const std::vector<PokerCard>& cards);
    cocos2d::Vec2 getTopCardWorldPosition() const;

    void setReserveCount(int count);

    void setWastePileCount(int count);

    using DrawCallback = std::function<void()>;
    void setDrawCallback(const DrawCallback& cb);

    using RecycleCallback = std::function<void()>;
    void setRecycleCallback(const RecycleCallback& cb);

    void animateOpenTopCards(const std::vector<PokerCard>& cards);

    // 获取顶部牌区域的世界矩形（用于拖放检测）
    cocos2d::Rect getTopCardWorldRect() const;

private:
    void buildReserveDeck();
    void buildReserveLabel();
    void buildRecycleButton();
    void rebuildReserveDeckVisual();
    void updateReserveVisual();
    void updateRecycleButton();

    bool onReserveTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onReserveTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    bool onRecycleTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onRecycleTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    std::vector<PokerCardView*> _openTopCardViews;
    cocos2d::Node* _reserveDeckView;
    cocos2d::Label* _reserveLabel;
    cocos2d::Label* _recycleButton;
    DrawCallback _onDraw;
    RecycleCallback _onRecycle;
    int _reserveCount;
    int _wastePileCount;

    float _areaWidth;
    float _areaHeight;
};

#endif
