/**
 * @file GameState.cpp
 * @brief 游戏状态数据模型实现。
 *
 * 主要功能:
 *   - 持有主牌区列表、底牌堆、废牌堆、奖励牌
 *   - 设置/获取可见明牌窗口数量
 *   - 初始化布局、移动卡牌、翻开卡牌
 */
#include "GameState.h"
#include "config/GlobalConfig.h"
#include <algorithm>

void GameState::setVisibleTopCardCount(int count)
{
    auto& cfg = GlobalConfig::getInstance();
    _visibleTopCardCount = std::max(cfg.getVisibleTopCardCountMin(), std::min(count, cfg.getVisibleTopCardCountMax()));
}

int GameState::getVisibleTopCardCount() const
{
    return _visibleTopCardCount;
}

// 读取布局文件并构建覆盖索引。
bool GameState::loadLayout(const std::string& layoutFilePath)
{
    if (!_layoutConfig.loadFromFile(layoutFilePath))
    {
        return false;
    }

    // 布局加载完成后，立即整理遮挡索引，避免运行时反复遍历原始配置。
    buildCoverLinksFromLayout();
    return true;
}

// 构建覆盖父子索引，便于运行时快速判断暴露状态。
void GameState::buildCoverLinksFromLayout()
{
    int totalSlots = _layoutConfig.getTotalSlots();
    _coveringParents.assign(totalSlots, {});
    _coveredChildren.assign(totalSlots, {});

    for (const auto& slotLayout : _layoutConfig.getSlots())
    {
        int childId = slotLayout.id;
        _coveringParents[childId] = slotLayout.covers;

        for (int parentId : slotLayout.covers)
        {
            if (parentId >= 0 && parentId < totalSlots)
            {
                // 反向索引用于“移走一张牌后，只检查可能被它解锁的子节点”。
                _coveredChildren[parentId].push_back(childId);
            }
        }
    }
}

// 重置全局状态并按布局发牌。
void GameState::initializeGame()
{
    // 开新局时彻底重建运行时状态，避免上一局残留数据污染本局。
    _mainAreaSlots.clear();
    _reserveDeck = CardDeck();
    _openTopCards.clear();
    _wastePile.clear();
    _rewardPayload.clear();

    const int totalSlots = _layoutConfig.getTotalSlots();
    int rewardSlotCount = 0;
    for (const auto& slot : _layoutConfig.getSlots())
    {
        if (slot.isReward) ++rewardSlotCount;
    }
    _mainAreaSlots.resize(totalSlots);

    // 场上槽位（排除奖励牌）+ 顶部明牌窗口 + 每张奖励牌预留的底牌。
    auto& cfg = GlobalConfig::getInstance();
    const int requiredCards = (totalSlots - rewardSlotCount) + _visibleTopCardCount + rewardSlotCount * cfg.getRewardCardsPerBonus();
    const int deckCount = std::max(1, (requiredCards + cfg.getStandardDeckSize() - 1) / cfg.getStandardDeckSize());
    CardDeck deck = CardDeck::createMultipleStandardDecks(deckCount);
    deck.shuffle();

    for (int i = 0; i < totalSlots && !deck.isEmpty(); ++i)
    {
        // 开局时只有最上层、没有父节点压住的槽位需要直接翻开。
        const bool faceUp = _coveringParents[i].empty();
        const auto& slotLayout = _layoutConfig.getSlots()[i];
        if (slotLayout.isReward)
        {
            // 奖励牌本身不占用普通牌，但要预留底牌。
            std::vector<PokerCard> payload;
            for (int p = 0; p < cfg.getRewardCardsPerBonus() && !deck.isEmpty(); ++p)
            {
                payload.push_back(deck.dealCard());
            }
            _rewardPayload[i] = payload;
            _mainAreaSlots[i].pushCard(PokerCard::RewardCard(), faceUp);
        }
        else
        {
            _mainAreaSlots[i].pushCard(deck.dealCard(), faceUp);
        }
    }

    // 顶部明牌窗口优先补满，确保玩家开局就能操作。
    for (int i = 0; i < _visibleTopCardCount && !deck.isEmpty(); ++i)
    {
        _openTopCards.push_back(deck.dealCard());
    }

    // 剩余牌全部进入 reserve，作为后续抽牌来源。
    std::vector<PokerCard> reserveCards;
    while (!deck.isEmpty())
    {
        reserveCards.push_back(deck.dealCard());
    }
    _reserveDeck.reset(reserveCards);

    // 若关卡配置把奖励牌暴露在初始态，则立即触发其奖励，避免停留在场上。
    for (int i = 0; i < totalSlots; ++i)
    {
        if (_mainAreaSlots[i].isEmpty()) continue;
        if (!_mainAreaSlots[i].topCard().isReward()) continue;
        if (!isSlotExposed(i)) continue;
        auto it = _rewardPayload.find(i);
        if (it != _rewardPayload.end())
        {
            for (const auto& c : it->second)
            {
                _reserveDeck.addCardToTop(c);
            }
            _rewardPayload.erase(it);
            _mainAreaSlots[i].popCard();
        }
    }
}

// 获取当前顶部窗口最上方的牌。
const PokerCard& GameState::getCurrentTopCard() const
{
    return _openTopCards.back();
}

// 用新牌替换顶部窗口指定位置。
void GameState::replaceOpenTopCard(int index, const PokerCard& newTop)
{
    if (index < 0 || index >= static_cast<int>(_openTopCards.size()))
    {
        return;
    }
    _openTopCards[index] = newTop;
}

// 抽一张底牌到窗口，必要时滑动窗口并推入废牌堆。
void GameState::drawFromReserve()
{
    if (!_reserveDeck.isEmpty())
    {
        if (static_cast<int>(_openTopCards.size()) >= _visibleTopCardCount && !_openTopCards.empty())
        {
            // 顶部窗口满时，最旧的那张牌进入废牌堆，窗口向前滑动。
            _wastePile.push_back(_openTopCards.front());
            _openTopCards.erase(_openTopCards.begin());
        }
        _openTopCards.push_back(_reserveDeck.dealCard());
    }
}

const std::vector<PokerCard>& GameState::getOpenTopCards() const
{
    return _openTopCards;
}

// 恢复顶部窗口到指定快照。
void GameState::restoreOpenTopCards(const std::vector<PokerCard>& cards)
{
    _openTopCards = cards;
}

bool GameState::hasReserveCards() const
{
    return !_reserveDeck.isEmpty();
}

int GameState::reserveDeckSize() const
{
    return _reserveDeck.size();
}

int GameState::wastePileSize() const
{
    return static_cast<int>(_wastePile.size());
}

// 将废牌堆回收并洗牌后接到底牌堆末尾。
void GameState::recycleWastePile()
{
    if (_wastePile.empty()) return;

    // 回收逻辑刻意保持简单：废牌单独洗牌后再接回 reserve 末尾。
    std::vector<PokerCard> recycled = _wastePile;
    _wastePile.clear();

    CardDeck tempDeck;
    tempDeck.reset(recycled);
    tempDeck.shuffle();

    std::vector<PokerCard> currentReserve;
    while (!_reserveDeck.isEmpty())
    {
        // dealCard 从顶部取牌，这里插到头部是为了保持原 reserve 的先后顺序不变。
        currentReserve.insert(currentReserve.begin(), _reserveDeck.dealCard());
    }
    while (!tempDeck.isEmpty())
    {
        currentReserve.push_back(tempDeck.dealCard());
    }
    _reserveDeck.reset(currentReserve);
}

void GameState::returnCardToReserve(const PokerCard& card)
{
    _reserveDeck.addCardToTop(card);
}

// 废牌堆弹出一张。
void GameState::popFromWastePile()
{
    if (!_wastePile.empty())
    {
        _wastePile.pop_back();
    }
}

CardSlot& GameState::getSlot(int index)
{
    return _mainAreaSlots[index];
}

const CardSlot& GameState::getSlot(int index) const
{
    return _mainAreaSlots[index];
}

int GameState::slotCount() const
{
    return static_cast<int>(_mainAreaSlots.size());
}

PokerCard GameState::removeCardFromSlot(int index,
                                        std::vector<int>* revealedSlotIndices,
                                        std::vector<RewardGrant>* rewardGrants)
{
    PokerCard removedCard = _mainAreaSlots[index].popCard();

    if (revealedSlotIndices != nullptr)
    {
        revealedSlotIndices->clear();
    }

    // BFS: 移除牌后逐级检查子节点是否暴露，奖励牌移除后也要级联。
    std::vector<int> toCheck(_coveredChildren[index].begin(),
                             _coveredChildren[index].end());

    for (size_t qi = 0; qi < toCheck.size(); ++qi)
    {
        int childIndex = toCheck[qi];
        if (_mainAreaSlots[childIndex].isEmpty())
        {
            continue;
        }

        if (isSlotExposed(childIndex))
        {
            _mainAreaSlots[childIndex].setTopCardFaceUp(true);
            if (revealedSlotIndices != nullptr)
            {
                revealedSlotIndices->push_back(childIndex);
            }

            // 奖励牌在被翻开且已暴露时立即触发。
            if (_mainAreaSlots[childIndex].topCard().isReward())
            {
                auto it = _rewardPayload.find(childIndex);
                if (it != _rewardPayload.end())
                {
                    for (const auto& c : it->second)
                    {
                        _reserveDeck.addCardToTop(c);
                    }
                    if (rewardGrants)
                    {
                        RewardGrant grant;
                        grant.slotIndex = childIndex;
                        grant.payload = it->second;
                        rewardGrants->push_back(grant);
                    }
                    _rewardPayload.erase(it);
                    _mainAreaSlots[childIndex].popCard(); // 奖励牌触发后移除

                    // 奖励牌移除后，其子节点也需要检查是否暴露。
                    for (int grandChild : _coveredChildren[childIndex])
                    {
                        toCheck.push_back(grandChild);
                    }
                }
            }
        }
    }

    return removedCard;
}

// 将牌放回槽位，并把本次操作翻开的牌重新盖回。
void GameState::restoreCardToSlot(int index, const PokerCard& card, const std::vector<int>& revealedSlotIndices)
{
    // 被玩家移走的牌原本就是一张可见顶牌，撤销时需要按可见状态放回。
    _mainAreaSlots[index].pushCard(card, true);

    for (int revealedIndex : revealedSlotIndices)
    {
        if (!_mainAreaSlots[revealedIndex].isEmpty())
        {
            // 本次操作临时翻开的子节点，在撤销时要重新盖回去。
            _mainAreaSlots[revealedIndex].setTopCardFaceUp(false);
        }
    }
}

// 判断槽位是否已完全暴露（没有父节点压住）。
bool GameState::isSlotExposed(int index) const
{
    // 只要还有任意一个父节点非空，该槽位就不算暴露。
    for (int parentIndex : _coveringParents[index])
    {
        if (!_mainAreaSlots[parentIndex].isEmpty())
        {
            return false;
        }
    }
    return true;
}

bool GameState::isWin() const
{
    for (const auto& slot : _mainAreaSlots)
    {
        if (!slot.isEmpty())
        {
            return false;
        }
    }
    return true;
}

void GameState::undoReward(const RewardGrant& grant)
{
    // 回滚预存牌到奖励映射，并把奖励牌放回槽位顶端（保持面朝下）。
    for (size_t i = 0; i < grant.payload.size(); ++i)
    {
        if (!_reserveDeck.isEmpty())
        {
            _reserveDeck.popTopCard();
        }
    }
    _rewardPayload[grant.slotIndex] = grant.payload;
    if (!_mainAreaSlots[grant.slotIndex].isEmpty())
    {
        // 理论上触发后槽位应为空，这里做保护避免重复叠加。
        _mainAreaSlots[grant.slotIndex].popCard();
    }
    _mainAreaSlots[grant.slotIndex].pushCard(PokerCard::RewardCard(), false);
}
