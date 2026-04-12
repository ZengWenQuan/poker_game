#include "GameScene.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "config/GlobalConfig.h"
#include "editor/CustomLayoutEditor.h"
#include "logging/GameLogger.h"
#include "presenter/GameplayPresenter.h"

USING_NS_CC;

void GameScene::initializeGameController()
{
    auto& cfg = GlobalConfig::getInstance();

    _controller = new GameController(_gameState);

    if (!_availableLayouts.empty())
    {
        int defaultIdx = cfg.getDefaultLayoutIndex();
        if (defaultIdx < 0 || defaultIdx >= static_cast<int>(_availableLayouts.size()))
        {
            defaultIdx = 0;
        }
        _currentLayoutIndex = defaultIdx;
        if (!_gameState.loadLayout(_availableLayouts[defaultIdx].filePath))
        {
            GAME_LOG_ERROR("Failed to load default layout config: %s", _availableLayouts[defaultIdx].filePath.c_str());
        }
        else
        {
            GAME_LOG_INFO("Default layout loaded: index=%d name=%s file=%s",
                          defaultIdx,
                          _availableLayouts[defaultIdx].name.c_str(),
                          _availableLayouts[defaultIdx].filePath.c_str());
        }
    }

    _gameState.setVisibleTopCardCount(_selectedVisibleTopCardCount);
}

void GameScene::startGame()
{
    if (_controller == nullptr)
    {
        _controller = new GameController(_gameState);
        bindCallbacks();
    }

    _controller->startGame();
    _mainArea->setupFromLayout(_gameState.getLayoutConfig());
    _gameplayPresenter->refreshViews();
    updateLevelSelectorUI();
}

void GameScene::bindCallbacks()
{
    _mainArea->setSlotTapCallback([this](int slotIndex) {
        if (!isGameplayMode()) return;
        _gameplayPresenter->handleResult(_controller->onSlotCardTapped(slotIndex));
    });
    _mainArea->setDragEndCallback([this](int slotIndex, const Vec2& worldPos) {
        if (!isGameplayMode()) return;
        onCardDragEnd(slotIndex, worldPos);
    });

    _topArea->setDrawCallback([this]() {
        if (!isGameplayMode()) return;
        _gameplayPresenter->handleResult(_controller->onDrawReserveCard());
    });
    _topArea->setRecycleCallback([this]() {
        if (!isGameplayMode()) return;
        _gameplayPresenter->handleResult(_controller->onRecycleWastePile());
    });
}

void GameScene::onCardDragEnd(int slotIndex, const Vec2& worldPos)
{
    if (_gameplayPresenter->isInputLocked()) return;

    auto* slotView = _mainArea->getSlotView(slotIndex);
    if (slotView == nullptr) return;

    if (_gameplayPresenter->isNearTopCardArea(worldPos) && _controller->isSlotMatchable(slotIndex))
    {
        _gameplayPresenter->handleResult(_controller->onSlotCardTapped(slotIndex));
        return;
    }

    _gameplayPresenter->setInputLocked(true);
    auto* callback = CallFunc::create([this]() {
        _gameplayPresenter->setInputLocked(false);
    });
    slotView->animateTopCardBack(callback);

    if (_gameplayPresenter->isNearTopCardArea(worldPos))
    {
        setStatusText(GlobalConfig::getInstance().get("cannotMatch"));
    }
}

bool GameScene::isGameplayMode() const
{
    return _customLayoutEditor == nullptr || !_customLayoutEditor->isActive();
}

bool GameScene::canHandleGameplayInput() const
{
    return isGameplayMode() && !_gameplayPresenter->isInputLocked();
}
