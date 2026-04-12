#ifndef POKER_GAME_POKER_CARD_VIEW_H
#define POKER_GAME_POKER_CARD_VIEW_H

#include "PokerCard.h"
#include "cocos2d.h"

// 单张牌的可视化节点，支持翻转、缩放和高亮。
class PokerCardView : public cocos2d::Node
{
public:
    // 创建一张牌的视图。
    // card: 要展示的牌；faceUp: 是否正面朝上。
    static PokerCardView* create(const PokerCard& card, bool faceUp = true);

    // 初始化牌面内容与初始朝向。
    bool initWithCard(const PokerCard& card, bool faceUp);

    // 访问底层牌数据。
    const PokerCard& getCard() const;
    // 设置当前朝向。
    void setFaceUp(bool faceUp);
    bool isFaceUp() const;
    // 播放翻面动画到正面；duration 为 -1 时使用配置默认时长。
    void flipToFaceUp(float duration = -1.0f);  // -1 表示使用配置中的默认值
    // 切换高亮外观。
    void setHighlight(bool highlight);

    // 卡牌缩放与尺寸。
    // 牌面真实尺寸优先从资源探测，避免配置和素材脱节。
    // 获取原始素材尺寸。
    static cocos2d::Size getSourceCardSize();
    // 获取运行时实际展示尺寸。
    static cocos2d::Size getCardSize();
    static float getCardWidth();
    static float getCardHeight();
    // 从资源重新探测尺寸并刷新缩放。
    static void refreshCardMetrics();

private:
    static cocos2d::Size detectTextureSize(const std::string& path);

    // 正反面节点拆开维护，翻牌时只需切换显隐或做简单缩放动画。
    void buildCardFace();
    void buildCardBack();

    PokerCard _card;           // 当前展示的牌
    bool _faceUp;              // 当前是否正面朝上
    cocos2d::Node* _faceNode;  // 正面节点
    cocos2d::Node* _backNode;  // 背面节点

    static float s_cardScale;  // 原始资源到运行时展示尺寸的统一缩放
};

#endif
