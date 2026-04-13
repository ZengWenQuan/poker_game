/**
 * @file GameSceneUI.cpp
 * @brief 游戏 UI 辅助功能实现。
 *
 * 主要功能:
 *   - 构建顶栏撤销按钮
 *   - 更新状态栏文本 (STATUS/撤销/胜利等)
 *   - 语言切换 (toggleLanguage) 和全量 UI 刷新 (refreshAllUI)
 */
#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "view/UILabelHelper.h"
#include "config/GlobalConfig.h"
#include "presenter/GameplayPresenter.h"
#include "presenter/SceneChromePresenter.h"
#include "presenter/SceneUIManager.h"

USING_NS_CC;

// 创建并铺满场景的背景图。
void GameScene::buildBackground()
{
    auto& cfg = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    // 背景图始终铺满当前可视区域，不依赖设计分辨率。
    auto* background = Sprite::create(cfg.getImage("background"));
    if (background == nullptr) return;

    background->setAnchorPoint(Vec2(0.5f, 0.5f));
    background->setPosition(origin + visibleSize / 2.0f);

    if (background->getContentSize().width > 0 && background->getContentSize().height > 0)
    {
        background->setScaleX(visibleSize.width / background->getContentSize().width);
        background->setScaleY(visibleSize.height / background->getContentSize().height);
    }

    addChild(background);
}

// 搭建顶部区域、主牌区、撤销按钮与状态文本。
void GameScene::buildUI()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    // 顶部牌区和主牌区都挂在场景中心，内部再按设计分辨率摆放自己的内容。
    _topArea = TopAreaView::create();
    _topArea->setPosition(Vec2(origin.x + visibleSize.width * 0.5f,
                               origin.y + visibleSize.height * 0.5f));
    addChild(_topArea, 2);

    _mainArea = MainAreaView::create();
    _mainArea->setPosition(Vec2(origin.x + visibleSize.width * 0.5f,
                                origin.y + visibleSize.height * 0.5f));
    addChild(_mainArea, 1);

    auto* undoLabel = UiLabelHelper::create(strings.get("undo"), theme.getFont(), theme.getFontSize("undo"));
    undoLabel->setTextColor(theme.getColor4B("buttonText"));
    _undoMenuItem = MenuItemLabel::create(undoLabel, [this](Ref*) {
        if (!isGameplayMode()) return;
        _gameplayPresenter->handleResult(_controller->onUndo());
    });

    auto* menu = Menu::create(_undoMenuItem, nullptr);
    menu->setPosition(Vec2(origin.x + visibleSize.width - cfg.getUndoPosition().x,
                           origin.y + cfg.getUndoPosition().y));
    addChild(menu, 5);
    if (_sceneChromePresenter) _sceneChromePresenter->bindUndo(_undoMenuItem);

    _statusLabel = UiLabelHelper::create("", theme.getFont(), theme.getFontSize("status"));
    _statusLabel->setPosition(Vec2(origin.x + visibleSize.width * cfg.getStatusPositionRatio().x,
                                   origin.y + visibleSize.height * cfg.getStatusPositionRatio().y));
    _statusLabel->setTextColor(theme.getColor4B("statusText"));
    addChild(_statusLabel, 5);

    // 将 UI 组件绑定到 SceneUIManager。
    if (_sceneUIManager)
    {
        _sceneUIManager->bindStatusLabel(_statusLabel);
        _sceneUIManager->bindUndoMenuItem(_undoMenuItem);
    }
}

// 更新状态栏文案和可选颜色。
void GameScene::setStatusText(const std::string& text, const Color4B* color)
{
    if (_sceneUIManager) _sceneUIManager->setStatusText(text, color);
}

void GameScene::toggleLanguage()
{
    auto& cfg = GlobalConfig::getInstance();
    cfg.setLanguage(cfg.getLanguage() == "en" ? "zh" : "en");
    refreshAllUI();
}

std::string GameScene::getCurrentLevelDisplayName() const
{
    return _layoutFlowController.currentDisplayName(_customLayoutEditor && _customLayoutEditor->isActive(),
                                                    _customLayoutEditor ? _customLayoutEditor->getCurrentLayoutName() : std::string());
}

void GameScene::refreshAllUI()
{
    if (_sceneChromePresenter)
    {
        _sceneChromePresenter->refreshAll(getCurrentLevelDisplayName(),
                                          _customLayoutEditor && _customLayoutEditor->isActive(),
                                          _selectedVisibleTopCardCount);
    }

    // 顶部区域文案（底牌/废牌/回收）
    if (_topArea)
    {
        _topArea->refreshStrings();
    }
}
