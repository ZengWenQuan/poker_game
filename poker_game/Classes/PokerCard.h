/**
 * @file PokerCard.h
 * @brief 卡牌数据模型头文件。
 *
 * 主要功能:
 *   - CardSuit   (花色枚举: 黑桃/红心/梅花/方块)
 *   - CardRank   (点数枚举: A~K)
 *   - CardKind   (牌堆类型: none/reserve/main/recycle/waste/reward)
 *   - PokerCard  (卡牌类: 花色+点数+类型+可见性)
 */
#ifndef POKER_GAME_POKER_CARD_H
#define POKER_GAME_POKER_CARD_H

#include "CardTypes.h"
#include <string>

// 牌对象：包含花色与点数，并提供纹理路径与规则辅助接口。
class PokerCard
{
public:
    // suit/rank: 牌的花色与点数，默认黑桃 A；kind 默认为普通牌。
    PokerCard(CardSuit suit = CardSuit::Spade, CardRank rank = CardRank::Ace, CardKind kind = CardKind::Normal);

    // 创建一张奖励牌。
    static PokerCard RewardCard();

    // 花色与纹理资源辅助接口。
    // 是否为红色花色（红桃或方块）。
    bool isRedSuit() const;
    // 是否为奖励牌。
    bool isReward() const { return _kind == CardKind::Reward; }
    // 返回点数的字符串标记（如 "A"、"10"、"K"）。
    std::string rankToken() const;
    // 获取花色贴图路径。
    std::string suitTexturePath() const;
    // 获取大号点数字体贴图路径。
    std::string bigRankTexturePath() const;
    // 获取小号点数字体贴图路径。
    std::string smallRankTexturePath() const;

    // 规则层辅助接口。
    // 返回点数的整数值，便于比较。
    int rankValue() const;
    // 牌面完全相等比较。
    bool operator==(const PokerCard& other) const;
    bool operator!=(const PokerCard& other) const;

private:
    CardSuit _suit {CardSuit::Spade}; // 牌的花色
    CardRank _rank {CardRank::Ace};   // 牌的点数
    CardKind _kind {CardKind::Normal}; // 牌类型（普通/奖励）
};

#endif
