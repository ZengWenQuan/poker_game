#include "GameState.h"
#include <algorithm>

void GameState::setVisibleTopCardCount(int count)
{
    _visibleTopCardCount = std::max(1, std::min(count, 3));
}

int GameState::getVisibleTopCardCount() const
{
    return _visibleTopCardCount;
}

bool GameState::loadLayout(const std::string& layoutFilePath)
{
    if (!_layoutConfig.loadFromFile(layoutFilePath))
    {
        return false;
    }

    buildCoverLinksFromLayout();
    return true;
}

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
                _coveredChildren[parentId].push_back(childId);
            }
        }
    }
}

void GameState::initializeGame()
{
    _mainAreaSlots.clear();
    _reserveDeck = CardDeck();
    _openTopCards.clear();
    _wastePile.clear();

    const int totalSlots = _layoutConfig.getTotalSlots();
    _mainAreaSlots.resize(totalSlots);

    const int requiredCards = totalSlots + _visibleTopCardCount;
    const int deckCount = std::max(1, (requiredCards + 51) / 52);
    CardDeck deck = CardDeck::createMultipleStandardDecks(deckCount);
    deck.shuffle();

    for (int i = 0; i < totalSlots && !deck.isEmpty(); ++i)
    {
        // 没被任何牌压着的 slot → 翻开
        const bool faceUp = _coveringParents[i].empty();
        _mainAreaSlots[i].pushCard(deck.dealCard(), faceUp);
    }

    for (int i = 0; i < _visibleTopCardCount && !deck.isEmpty(); ++i)
    {
        _openTopCards.push_back(deck.dealCard());
    }

    std::vector<PokerCard> reserveCards;
    while (!deck.isEmpty())
    {
        reserveCards.push_back(deck.dealCard());
    }
    _reserveDeck.reset(reserveCards);
}

const PokerCard& GameState::getCurrentTopCard() const
{
    return _openTopCards.back();
}

void GameState::replaceOpenTopCard(int index, const PokerCard& newTop)
{
    if (index < 0 || index >= static_cast<int>(_openTopCards.size()))
    {
        return;
    }
    _openTopCards[index] = newTop;
}

void GameState::drawFromReserve()
{
    if (!_reserveDeck.isEmpty())
    {
        if (static_cast<int>(_openTopCards.size()) >= _visibleTopCardCount && !_openTopCards.empty())
        {
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

void GameState::recycleWastePile()
{
    if (_wastePile.empty()) return;

    std::vector<PokerCard> recycled = _wastePile;
    _wastePile.clear();

    CardDeck tempDeck;
    tempDeck.reset(recycled);
    tempDeck.shuffle();

    std::vector<PokerCard> currentReserve;
    while (!_reserveDeck.isEmpty())
    {
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
            _mainAreaSlots[childIndex].setTopCardFaceUp(true);
            if (revealedSlotIndices != nullptr)
            {
                revealedSlotIndices->push_back(childIndex);
            }
        }
    }

    return removedCard;
}

void GameState::restoreCardToSlot(int index, const PokerCard& card, const std::vector<int>& revealedSlotIndices)
{
    _mainAreaSlots[index].pushCard(card, true);

    for (int revealedIndex : revealedSlotIndices)
    {
        if (!_mainAreaSlots[revealedIndex].isEmpty())
        {
            _mainAreaSlots[revealedIndex].setTopCardFaceUp(false);
        }
    }
}

bool GameState::isSlotExposed(int index) const
{
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
