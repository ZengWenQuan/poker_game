#ifndef POKER_GAME_CARD_TYPES_H
#define POKER_GAME_CARD_TYPES_H

enum class CardSuit
{
    Spade,   // 黑桃
    Heart,   // 红桃
    Club,    // 梅花
    Diamond  // 方块
};

enum class CardRank
{
    Ace = 1, // A 从 1 开始，便于规则层直接做整数比较
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King
};

#endif
