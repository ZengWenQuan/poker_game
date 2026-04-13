/**
 * @file GameSceneStartOverlay.cpp
 * @brief 开局难度选择弹层实现。
 */
#include "GameScene.h"
#include "config/GlobalConfig.h"
#include "logging/GameLogger.h"
#include "presenter/SceneChromePresenter.h"
#include "view/UILabelHelper.h"

USING_NS_CC;

void GameScene::buildStartGameOverlay()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _startGameOverlay = LayerColor::create(cfg.getColor4B("overlayMask"), visibleSize.width, visibleSize.height);
    _startGameOverlay->setPosition(origin);
    addChild(_startGameOverlay, 12);

    const Size panelSize = cfg.getDialogSize("startOverlay", "panelSize", Size(620, 280));
    auto* panel = LayerColor::create(cfg.getColor4B("dialogPanel"), panelSize.width, panelSize.height);
    panel->setIgnoreAnchorPointForPosition(false);
    panel->setAnchorPoint(Vec2(0.5f, 0.5f));
    panel->setPosition(origin + visibleSize / 2.0f);
    _startGameOverlay->addChild(panel);

    const Vec2 titlePos = cfg.getDialogVec2("startOverlay", "titlePosition", Vec2(310, 220));
    auto* title = UiLabelHelper::create(strings.get("startGame"), cfg.getFont(), cfg.getFontSize("levelTitle"));
    title->setPosition(titlePos);
    panel->addChild(title);
    _startOverlayTitle = title;

    const Vec2 subtitlePos = cfg.getDialogVec2("startOverlay", "subtitlePosition", Vec2(310, 175));
    auto* subtitle = UiLabelHelper::create(strings.get("openBaseCards"), cfg.getFont(), cfg.getFontSize("status"));
    subtitle->setPosition(subtitlePos);
    panel->addChild(subtitle);
    _startOverlaySubtitle = subtitle;

    const Vec2 diffOnePos = cfg.getDialogVec2("startOverlay", "difficultyOnePosition", Vec2(170, 118));
    const Vec2 diffTwoPos = cfg.getDialogVec2("startOverlay", "difficultyTwoPosition", Vec2(310, 118));
    const Vec2 diffThreePos = cfg.getDialogVec2("startOverlay", "difficultyThreePosition", Vec2(460, 118));

    auto makeDifficultyItem = [&](const std::string& text, const Vec2& pos, int count) {
        auto* label = UiLabelHelper::create(text, cfg.getFont(), cfg.getFontSize("levelBtn"));
        auto* item = MenuItemLabel::create(label, [this, count](Ref*) {
            _selectedVisibleTopCardCount = count;
            updateStartGameDifficultyUI();
        });
        item->setPosition(pos);
        return item;
    };

    _difficultyOneMenuItem = makeDifficultyItem(strings.get("difficultyOne") + strings.get("difficultyHard"), diffOnePos, 1);
    _difficultyTwoMenuItem = makeDifficultyItem(strings.get("difficultyTwo") + strings.get("difficultyMedium"), diffTwoPos, 2);
    _difficultyThreeMenuItem = makeDifficultyItem(strings.get("difficultyThree") + strings.get("difficultyEasy"), diffThreePos, 3);

    const Vec2 startPos = cfg.getDialogVec2("startOverlay", "startButtonPosition", Vec2(310, 52));
    auto* startLabel = UiLabelHelper::create(strings.get("startGame"), cfg.getFont(), cfg.getFontSize("levelTitle"));
    _startGameMenuItem = MenuItemLabel::create(startLabel, [this](Ref*) {
        restartGameWithSelectedDifficulty();
        _startGameOverlay->setVisible(false);
    });
    _startGameMenuItem->setPosition(startPos);

    auto* menu = Menu::create(_difficultyOneMenuItem,
                              _difficultyTwoMenuItem,
                              _difficultyThreeMenuItem,
                              _startGameMenuItem,
                              nullptr);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu);

    if (_sceneChromePresenter)
    {
        _sceneChromePresenter->bindStartOverlay(_startOverlayTitle,
                                                _startOverlaySubtitle,
                                                _difficultyOneMenuItem,
                                                _difficultyTwoMenuItem,
                                                _difficultyThreeMenuItem,
                                                _startGameMenuItem);
    }

    updateStartGameDifficultyUI();
}

void GameScene::restartGameWithSelectedDifficulty()
{
    _gameState.setVisibleTopCardCount(_selectedVisibleTopCardCount);
    startGame();
    setStatusText(GlobalConfig::getInstance().get("openBaseCards") + " " + std::to_string(_selectedVisibleTopCardCount));
    GAME_LOG_INFO("Restarted game with visibleTopCardCount=%d", _selectedVisibleTopCardCount);
}

void GameScene::updateStartGameDifficultyUI()
{
    if (_sceneChromePresenter)
    {
        _sceneChromePresenter->refreshStartOverlay(_selectedVisibleTopCardCount);
    }
}
