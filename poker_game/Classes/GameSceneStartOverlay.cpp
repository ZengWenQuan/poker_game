#include "GameScene.h"
#include "config/GlobalConfig.h"
#include "logging/GameLogger.h"

USING_NS_CC;

// 搭建开局难度选择弹层（选择顶部明牌数量）。
void GameScene::buildStartGameOverlay()
{
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    // 开局弹层只负责选择顶部明牌窗口大小，相当于难度选择。
    _startGameOverlay = LayerColor::create(Color4B(0, 0, 0, 170), visibleSize.width, visibleSize.height);
    _startGameOverlay->setPosition(origin);
    addChild(_startGameOverlay, 12);

    auto* panel = LayerColor::create(Color4B(26, 48, 30, 235), 620.0f, 280.0f);
    panel->setIgnoreAnchorPointForPosition(false);
    panel->setAnchorPoint(Vec2(0.5f, 0.5f));
    panel->setPosition(origin + visibleSize / 2.0f);
    _startGameOverlay->addChild(panel);

    auto* title = Label::createWithSystemFont(strings.get("startGame"), theme.getFont(), theme.getFontSize("levelTitle"));
    title->setPosition(Vec2(310.0f, 220.0f));
    panel->addChild(title);

    auto* subtitle = Label::createWithSystemFont(strings.get("openBaseCards"), theme.getFont(), theme.getFontSize("status"));
    subtitle->setPosition(Vec2(310.0f, 175.0f));
    panel->addChild(subtitle);

    auto makeDifficultyItem = [&](const std::string& text, const Vec2& pos, int count) {
        auto* label = Label::createWithSystemFont(text, theme.getFont(), theme.getFontSize("levelBtn"));
        auto* item = MenuItemLabel::create(label, [this, count](Ref*) {
            _selectedVisibleTopCardCount = count;
            updateStartGameDifficultyUI();
        });
        item->setPosition(pos);
        return item;
    };

    _difficultyOneMenuItem = makeDifficultyItem("1 " + strings.get("difficultyHard"), Vec2(170.0f, 118.0f), 1);
    _difficultyTwoMenuItem = makeDifficultyItem("2 " + strings.get("difficultyMedium"), Vec2(310.0f, 118.0f), 2);
    _difficultyThreeMenuItem = makeDifficultyItem("3 " + strings.get("difficultyEasy"), Vec2(460.0f, 118.0f), 3);

    auto* startLabel = Label::createWithSystemFont(strings.get("startGame"), theme.getFont(), theme.getFontSize("levelTitle"));
    _startGameMenuItem = MenuItemLabel::create(startLabel, [this](Ref*) {
        restartGameWithSelectedDifficulty();
        _startGameOverlay->setVisible(false);
    });
    _startGameMenuItem->setPosition(Vec2(310.0f, 52.0f));

    auto* menu = Menu::create(_difficultyOneMenuItem,
                              _difficultyTwoMenuItem,
                              _difficultyThreeMenuItem,
                              _startGameMenuItem,
                              nullptr);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu);

    updateStartGameDifficultyUI();
}

// 按当前选定明牌数量重开一局。
void GameScene::restartGameWithSelectedDifficulty()
{
    // 难度本质上就是“顶部明牌窗口数量”。
    _gameState.setVisibleTopCardCount(_selectedVisibleTopCardCount);
    startGame();
    setStatusText(GlobalConfig::getInstance().get("openBaseCards") + " " + std::to_string(_selectedVisibleTopCardCount));
    GAME_LOG_INFO("Restarted game with visibleTopCardCount=%d", _selectedVisibleTopCardCount);
}

// 刷新难度按钮高亮态。
void GameScene::updateStartGameDifficultyUI()
{
    auto& theme = GlobalConfig::getInstance();
    // 当前选中项通过颜色和轻微放大共同强调。
    auto updateItem = [&](MenuItemLabel* item, bool selected) {
        if (!item) return;
        auto* label = dynamic_cast<Label*>(item->getLabel());
        if (!label) return;
        label->setTextColor(selected ? theme.getColor4B("winGold") : theme.getColor4B("goldHighlight"));
        label->setScale(selected ? 1.08f : 1.0f);
    };

    updateItem(_difficultyOneMenuItem, _selectedVisibleTopCardCount == 1);
    updateItem(_difficultyTwoMenuItem, _selectedVisibleTopCardCount == 2);
    updateItem(_difficultyThreeMenuItem, _selectedVisibleTopCardCount == 3);
    updateItem(_startGameMenuItem, false);
}
