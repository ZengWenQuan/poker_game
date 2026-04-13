/**
 * @file GameState.h
 * @brief 游戏状态数据模型头文件。
 *
 * 主要功能:
 *   - 主牌区 (mainPiles)、底牌堆 (reservePile)、废牌堆 (wastePile)
 *   - 可见牌数量、奖励牌
 *   - initLayout / moveCard / flipCard
 */
#ifndef POKER_GAME_GAME_STATE_H
#define POKER_GAME_GAME_STATE_H

#include "CardDeck.h"
#include "CardSlot.h"
#include "LayoutConfig.h"
#include "RewardGrant.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

// 游戏状态核心：维护布局、主牌区、顶部明牌、底牌堆与废牌堆。
class GameState
{
public:
    // 加载布局配置，并把配置中的遮挡关系整理成运行时索引结构。
    // 只准备静态布局数据，不在这里发牌。
    // layoutFilePath: Resources 下的布局 JSON 路径。
    bool loadLayout(const std::string& layoutFilePath);

    // 用当前已加载的布局重新初始化整局游戏。
    void initializeGame();
    // 设置顶部明牌窗口大小。
    // count: 1~3 的可见张数。
    void setVisibleTopCardCount(int count);
    int getVisibleTopCardCount() const;

    // 顶部明牌区与底牌堆相关接口。
    // openTopCards 是当前可参与匹配的窗口，reserve 是后续抽牌来源。
    // 获取当前窗口最顶的可匹配牌。
    const PokerCard& getCurrentTopCard() const;
    // 抽一张新牌到窗口（同时可能挤出旧牌）。
    void drawFromReserve();
    // 替换窗口中指定位置的牌。
    // index: 目标窗口索引；newTop: 新牌。
    void replaceOpenTopCard(int index, const PokerCard& newTop);
    // 获取窗口内所有明牌。
    const std::vector<PokerCard>& getOpenTopCards() const;
    // 将明牌窗口恢复为指定快照。
    void restoreOpenTopCards(const std::vector<PokerCard>& cards);
    // 底牌是否还有可抽的牌。
    bool hasReserveCards() const;
    // 底牌剩余数量。
    int reserveDeckSize() const;

    int wastePileSize() const;

    // 回收废牌堆到底牌堆。
    void recycleWastePile();

    // 回退相关接口，仅供撤销流程使用。
    // 将一张牌放回底牌堆顶部。
    void returnCardToReserve(const PokerCard& card);
    // 从废牌堆弹出一张（通常与撤销对应）。
    void popFromWastePile();

    // 主牌区接口。
    // 获取可变槽位。
    // index: 槽位索引。
    CardSlot& getSlot(int index);
    // 获取只读槽位。
    const CardSlot& getSlot(int index) const;
    // 槽位总数。
    int slotCount() const;

    // 从主牌区移走一张牌，并返回因此被翻开的槽位列表，供撤销与界面刷新使用。
    // index: 来源槽位；revealedSlotIndices: 输出被翻开的槽位索引；
    // rewardGrants: 触发的奖励牌及其预存底牌。
    PokerCard removeCardFromSlot(int index,
                                 std::vector<int>* revealedSlotIndices = nullptr,
                                 std::vector<RewardGrant>* rewardGrants = nullptr);

    // 把移走的牌恢复回槽位，并撤销当次操作触发的翻牌效果。
    // index: 目标槽位；card: 要恢复的牌；revealedSlotIndices: 需要恢复朝下的槽位列表。
    void restoreCardToSlot(int index, const PokerCard& card, const std::vector<int>& revealedSlotIndices);

    // 判断一个槽位当前是否已没有任何父节点压住。
    // index: 槽位索引。
    bool isSlotExposed(int index) const;

    // 布局信息（供 View 使用）。
    const LayoutConfig& getLayoutConfig() const { return _layoutConfig; }

    bool isWin() const;

    // 撤销奖励牌触发：把奖励牌与预存底牌放回。
    void undoReward(const RewardGrant& grant);

private:
    // 把布局中的 covers 转成正反两个索引表：
    // 1. _coveringParents: 某槽位被哪些父节点压住
    // 2. _coveredChildren: 某槽位移除后可能影响哪些子节点
    void buildCoverLinksFromLayout();

    LayoutConfig _layoutConfig;                      // 当前关卡布局定义
    std::vector<CardSlot> _mainAreaSlots;           // 主牌区全部槽位
    std::vector<std::vector<int>> _coveringParents; // index -> 父节点列表
    std::vector<std::vector<int>> _coveredChildren; // index -> 子节点列表
    CardDeck _reserveDeck;                          // 底牌堆，抽牌来源
    std::vector<PokerCard> _openTopCards;           // 顶部可操作明牌窗口
    int _visibleTopCardCount = 1;                   // 明牌窗口大小，范围由配置控制

    std::vector<PokerCard> _wastePile;              // 被顶部窗口挤出的旧牌，供回收使用

    // 奖励牌预留的底牌映射：槽位索引 -> 预存的三张牌。
    std::unordered_map<int, std::vector<PokerCard>> _rewardPayload;
};

#endif
