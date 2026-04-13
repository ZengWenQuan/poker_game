/**
 * @file GameSceneGameplay.cpp
 * @brief 游戏玩法场景实现。
 *
 * 主要功能:
 *   - 构建游戏主界面 (主牌区 MainAreaView + 顶部区 TopAreaView)
 *   - 构建关卡选择区和顶栏按钮
 */
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

    // 控制器依赖 GameState，布局列表则来自前面已构建好的关卡选择数据。
    _controller = new GameController(_gameState);
    _layoutFlowController.initializeDefaultIndex(cfg.getDefaultLayoutIndex());

    if (_layoutFlowController.hasLayouts())
    {
        const auto& info = _layoutFlowController.layouts()[_layoutFlowController.currentIndex()];
        if (!_layoutFlowController.loadCurrentIntoGameplay(_gameState, _selectedVisibleTopCardCount))
        {
            GAME_LOG_ERROR("Failed to load default layout config: %s", info.filePath.c_str());
        }
        else
        {
            GAME_LOG_INFO("Default layout loaded: index=%d name=%s file=%s",
                          _layoutFlowController.currentIndex(),
                          info.name.c_str(),
                          info.filePath.c_str());
        }
    }
}

// 重开或首开一局，根据当前布局刷新视图。
void GameScene::startGame()
{
    if (_controller == nullptr)
    {
        _controller = new GameController(_gameState);
        bindCallbacks();
    }

    // 每次开局都重新让 MainAreaView 按当前布局重建槽位节点。
    _controller->startGame();
    _mainArea->setupFromLayout(_gameState.getLayoutConfig());
    _gameplayPresenter->refreshViews();
    updateLevelSelectorUI();
}

// 绑定 UI 回调到控制器与 presenter。
void GameScene::bindCallbacks()
{
    // GameScene 只做模式判断和事件路由，真正的结果处理交给 presenter。
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

// 处理拖拽结束：若靠近顶部且可匹配则触发匹配，否则弹回。
void GameScene::onCardDragEnd(int slotIndex, const Vec2& worldPos)
{
    if (_gameplayPresenter->isInputLocked()) return;

    auto* slotView = _mainArea->getSlotView(slotIndex);
    if (slotView == nullptr) return;

    // 拖到顶部牌区附近且规则允许时，直接视为一次匹配输入。
    if (_gameplayPresenter->isNearTopCardArea(worldPos) && _controller->getHighlightService().isSlotMatchable(slotIndex))
    {
        _gameplayPresenter->handleResult(_controller->onSlotCardTapped(slotIndex));
        return;
    }

    // 否则把牌平滑弹回原位，维持当前主牌区状态不变。
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

// 当前是否处于对局模式（非自定义编辑）。
bool GameScene::isGameplayMode() const
{
    return _customLayoutEditor == nullptr || !_customLayoutEditor->isActive();
}

// 对局模式且未锁输入时才处理交互。
bool GameScene::canHandleGameplayInput() const
{
    return isGameplayMode() && !_gameplayPresenter->isInputLocked();
}
