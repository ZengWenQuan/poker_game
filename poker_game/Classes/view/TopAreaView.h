/**
 * @file TopAreaView.h
 * @brief 顶部区域视图头文件。
 *
 * 主要功能:
 *   - 明牌窗口、底牌堆、回收按钮
 *   - bindController: 绑定控制器
 *   - syncFrom: 从 GameState 同步数据
 *   - refreshStrings: 刷新文案 (语言切换)
 */
#ifndef POKER_GAME_TOP_AREA_VIEW_H
#define POKER_GAME_TOP_AREA_VIEW_H

#include "PokerCardView.h"
#include "PokerCard.h"
#include <functional>
#include <vector>

// 顶部区域视图：展示明牌窗口、底牌堆和回收入口，并处理点击。
class TopAreaView : public cocos2d::Node
{
public:
    // 创建顶部区域节点。
    static TopAreaView* create();

    bool init() override;

    // 顶部区域包含三部分：明牌窗口、底牌堆、回收入口。
    // cards: 新的顶部窗口牌序。
    void setOpenTopCards(const std::vector<PokerCard>& cards);
    // 获取当前顶牌的世界坐标。
    cocos2d::Vec2 getTopCardWorldPosition() const;

    // 设置底牌堆数量并更新显示。
    // count: 底牌剩余数。
    void setReserveCount(int count);

    // 设置废牌堆数量并更新显示。
    // count: 废牌张数。
    void setWastePileCount(int count);

    // 刷新文案（语言切换后调用）。
    void refreshStrings();

    using DrawCallback = std::function<void()>;
    // 设置底牌点击回调。
    void setDrawCallback(const DrawCallback& cb);

    using RecycleCallback = std::function<void()>;
    // 设置回收按钮回调。
    void setRecycleCallback(const RecycleCallback& cb);

    // 顶部窗口翻牌动画。
    // cards: 目标明牌序列。
    void animateOpenTopCards(const std::vector<PokerCard>& cards);

    // 获取顶部牌区域的世界矩形（用于拖放检测）
    cocos2d::Rect getTopCardWorldRect() const;

    // 获取底牌堆的世界坐标（用于奖励牌飞入动画）
    cocos2d::Vec2 getReserveDeckWorldPosition() const;

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

    std::vector<PokerCardView*> _openTopCardViews; // 当前顶部明牌窗口
    cocos2d::Node* _reserveDeckView;               // 底牌堆背面叠牌视图
    cocos2d::Label* _reserveLabel;                 // reserve / waste 数量文案
    cocos2d::Label* _recycleButton;                // 回收按钮
    DrawCallback _onDraw;
    RecycleCallback _onRecycle;
    int _reserveCount;
    int _wastePileCount;

    float _areaWidth;
    float _areaHeight;
};

#endif
