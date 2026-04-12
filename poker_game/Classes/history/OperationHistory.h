#ifndef POKER_GAME_OPERATION_HISTORY_H
#define POKER_GAME_OPERATION_HISTORY_H

#include "PokerCard.h"
#include <vector>

enum class OperationType
{
    Match,    // 主牌区匹配操作
    DrawCard  // 从底牌堆抽牌
};

struct GameOperation
{
    OperationType type = OperationType::Match;    // 操作类型
    PokerCard movedCard;                          // 匹配时从主牌区移走的牌
    int slotIndex = -1;                           // 匹配来源槽位
    std::vector<int> revealedSlotIndices;         // 因本次操作翻开的槽位
    PokerCard drawnCard;                          // 抽牌操作中的新牌
    std::vector<PokerCard> previousOpenTopCards;  // 操作前顶部明牌窗口快照
    bool movedOpenCardToWaste = false;            // 抽牌是否把旧牌挤入废牌堆
};

// 操作历史：记录对局过程中的匹配与抽牌，支持撤销。
class OperationHistory
{
public:
    // 记录一次匹配操作。
    // movedCard: 被移走的牌；slotIndex: 来源槽位；
    // revealedSlotIndices: 受影响而翻开的槽位；previousOpenTopCards: 操作前的顶部明牌。
    void recordMatch(const PokerCard& movedCard,
                     int slotIndex,
                     const std::vector<int>& revealedSlotIndices,
                     const std::vector<PokerCard>& previousOpenTopCards);

    // 记录一次抽牌操作。
    // drawnCard: 新抽到的牌；previousOpenTopCards: 抽牌前明牌窗口；
    // movedOpenCardToWaste: 是否将旧明牌推入废牌堆。
    void recordDraw(const PokerCard& drawnCard,
                    const std::vector<PokerCard>& previousOpenTopCards,
                    bool movedOpenCardToWaste);

    // 弹出最近操作并返回。
    GameOperation pop();
    // 是否存在可撤销操作。
    bool canUndo() const;
    // 清空历史。
    void clear();

private:
    std::vector<GameOperation> _operations; // 线性撤销栈，尾部是最近一次操作
};

#endif
