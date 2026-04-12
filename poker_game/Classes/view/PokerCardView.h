#ifndef POKER_GAME_POKER_CARD_VIEW_H
#define POKER_GAME_POKER_CARD_VIEW_H

#include "PokerCard.h"
#include "cocos2d.h"

class PokerCardView : public cocos2d::Node
{
public:
    static PokerCardView* create(const PokerCard& card, bool faceUp = true);

    bool initWithCard(const PokerCard& card, bool faceUp);

    const PokerCard& getCard() const;
    void setFaceUp(bool faceUp);
    bool isFaceUp() const;
    void flipToFaceUp(float duration = -1.0f);  // -1 表示使用配置中的默认值
    void setHighlight(bool highlight);

    // 卡牌缩放与尺寸
    static cocos2d::Size getSourceCardSize();
    static cocos2d::Size getCardSize();
    static float getCardWidth();
    static float getCardHeight();
    static void refreshCardMetrics();

private:
    static cocos2d::Size detectTextureSize(const std::string& path);

    void buildCardFace();
    void buildCardBack();

    PokerCard _card;
    bool _faceUp;
    cocos2d::Node* _faceNode;
    cocos2d::Node* _backNode;

    static float s_cardScale;
};

#endif
