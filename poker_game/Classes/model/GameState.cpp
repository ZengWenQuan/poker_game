#include "GameState.h"
#include <algorithm>

void GameState::setVisibleTopCardCount(int count)
{
    // 当前玩法只支持 1~3 张明牌窗口，这里统一收敛外部输入。
    _visibleTopCardCount = std::max(1, std::min(count, 3));
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

    const int totalSlots = _layoutConfig.getTotalSlots();
    _mainAreaSlots.resize(totalSlots);

    // 场上槽位 + 顶部明牌窗口，是完成开局的最低牌量；不足时自动扩展到多副牌。
    const int requiredCards = totalSlots + _visibleTopCardCount;
    const int deckCount = std::max(1, (requiredCards + 51) / 52);
    CardDeck deck = CardDeck::createMultipleStandardDecks(deckCount);
    deck.shuffle();

    for (int i = 0; i < totalSlots && !deck.isEmpty(); ++i)
    {
        // 开局时只有最上层、没有父节点压住的槽位需要直接翻开。
        const bool faceUp = _coveringParents[i].empty();
        _mainAreaSlots[i].pushCard(deck.dealCard(), faceUp);
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

// 从槽位移走顶牌，并翻开必要的子节点；返回移走的牌。
PokerCard GameState::removeCardFromSlot(int index, std::vector<int>* revealedSlotIndices)
{
    PokerCard removedCard = _mainAreaSlots[index].popCard();

    if (revealedSlotIndices != nullptr)
    {
        revealedSlotIndices->clear();
    }

    for (int childIndex : _coveredChildren[index])
    {
        if (_mainAreaSlots[childIndex].isEmpty())
        {
            continue;
        }

        if (isSlotExposed(childIndex))
        {
            // 只有所有父节点都被清空后，该子节点的顶牌才真正可见。
            _mainAreaSlots[childIndex].setTopCardFaceUp(true);
            if (revealedSlotIndices != nullptr)
            {
                revealedSlotIndices->push_back(childIndex);
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
