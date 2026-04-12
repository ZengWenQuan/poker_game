#include "GameScene.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "config/GlobalConfig.h"
#include "presenter/GameplayPresenter.h"

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

    auto* undoLabel = Label::createWithSystemFont(strings.get("undo"), theme.getFont(), theme.getFontSize("undo"));
    undoLabel->setTextColor(Color4B::WHITE);
    _undoMenuItem = MenuItemLabel::create(undoLabel, [this](Ref*) {
        if (!isGameplayMode()) return;
        _gameplayPresenter->handleResult(_controller->onUndo());
    });

    auto* menu = Menu::create(_undoMenuItem, nullptr);
    menu->setPosition(Vec2(origin.x + visibleSize.width - cfg.getUndoPosition().x,
                           origin.y + cfg.getUndoPosition().y));
    addChild(menu, 5);

    _statusLabel = Label::createWithSystemFont("", theme.getFont(), theme.getFontSize("status"));
    _statusLabel->setPosition(Vec2(origin.x + visibleSize.width * cfg.getStatusPositionRatio().x,
                                   origin.y + visibleSize.height * cfg.getStatusPositionRatio().y));
    _statusLabel->setTextColor(theme.getColor4B("statusText"));
    addChild(_statusLabel, 5);
}

// 更新状态栏文案和可选颜色。
void GameScene::setStatusText(const std::string& text, const Color4B* color)
{
    if (_statusLabel == nullptr) return;
    _statusLabel->setString(text);
    if (color)
    {
        _statusLabel->setTextColor(*color);
    }
    // 未传颜色时保持当前颜色不变，便于胜利等特殊状态延续显示风格。
}
