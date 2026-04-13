/**
 * @file RewardGrant.h
 * @brief 奖励牌数据结构。
 *
 * 主要功能:
 *   - 记录奖励牌触发时的信息 (槽位索引、奖励卡牌列表)
 *   - 用于撤销与重放
 */
#ifndef POKER_GAME_REWARD_GRANT_H
#define POKER_GAME_REWARD_GRANT_H

#include "PokerCard.h"
#include <vector>

// 奖励牌触发时生成的奖励信息，用于撤销与重放。
struct RewardGrant
{
    int slotIndex = -1;              // 奖励牌所在槽位
    std::vector<PokerCard> payload;  // 触发时加入底牌堆的实际牌
};

#endif
