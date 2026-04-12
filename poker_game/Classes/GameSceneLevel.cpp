#include "GameScene.h"
#include "config/GlobalConfig.h"
#include "editor/CustomLayoutEditor.h"
#include "logging/GameLogger.h"

USING_NS_CC;

void GameScene::buildLevelSelector()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    refreshAvailableLayouts();

    _levelTitleLabel = Label::createWithSystemFont(strings.get("level"), theme.getFont(), theme.getFontSize("levelTitle"));
    _levelTitleLabel->setTextColor(theme.getColor4B("goldHighlight"));
    _levelTitleLabel->setPosition(Vec2(origin.x + visibleSize.width * cfg.getLevelTitlePositionRatio().x,
                                       origin.y + visibleSize.height * cfg.getLevelTitlePositionRatio().y));
    addChild(_levelTitleLabel, 6);

    auto* nextLabel = Label::createWithSystemFont(strings.get("switchLevel"), theme.getFont(), theme.getFontSize("levelBtn"));
    nextLabel->setTextColor(Color4B::WHITE);
    _levelSwitchMenuItem = MenuItemLabel::create(nextLabel, [this](Ref*) {
        if (!isGameplayMode()) return;
        switchToNextLayout();
    });
    _levelSwitchMenuItem->setPosition(Vec2(origin.x + visibleSize.width * cfg.getLevelBtnStartRatio().x
                                           + cfg.getLevelBtnWidth() * 0.5f,
                                           origin.y + visibleSize.height * cfg.getLevelBtnStartRatio().y));

    auto* customLabel = Label::createWithSystemFont(strings.get("customLayout"), theme.getFont(), theme.getFontSize("levelBtn"));
    customLabel->setTextColor(Color4B::WHITE);
    _customLayoutMenuItem = MenuItemLabel::create(customLabel, [this](Ref*) {
        toggleCustomLayoutMode();
    });
    _customLayoutMenuItem->setPosition(_levelSwitchMenuItem->getPosition() + Vec2(-150.0f, 0.0f));

    auto* saveLabel = Label::createWithSystemFont(strings.get("saveLayout"), theme.getFont(), theme.getFontSize("levelBtn"));
    saveLabel->setTextColor(Color4B::WHITE);
    _saveLayoutMenuItem = MenuItemLabel::create(saveLabel, [this](Ref*) {
        if (_customLayoutEditor && _customLayoutEditor->isActive())
        {
            _customLayoutEditor->showSaveDialog();
        }
    });
    _saveLayoutMenuItem->setPosition(_customLayoutMenuItem->getPosition() + Vec2(-120.0f, 0.0f));
    _saveLayoutMenuItem->setVisible(false);
    _saveLayoutMenuItem->setEnabled(false);

    _levelMenu = Menu::create(_levelSwitchMenuItem, _customLayoutMenuItem, _saveLayoutMenuItem, nullptr);
    _levelMenu->setPosition(Vec2::ZERO);
    addChild(_levelMenu, 6);
}

void GameScene::refreshAvailableLayouts()
{
    const std::string currentFile =
        (_currentLayoutIndex >= 0 && _currentLayoutIndex < static_cast<int>(_availableLayouts.size()))
            ? _availableLayouts[_currentLayoutIndex].filePath
            : std::string();

    _availableLayouts = LayoutConfig::getAvailableLayouts();

    if (!currentFile.empty())
    {
        for (int i = 0; i < static_cast<int>(_availableLayouts.size()); ++i)
        {
            if (_availableLayouts[i].filePath == currentFile)
            {
                _currentLayoutIndex = i;
                return;
            }
        }
    }

    if (_currentLayoutIndex >= static_cast<int>(_availableLayouts.size()))
    {
        _currentLayoutIndex = 0;
    }
}

void GameScene::switchToLayout(int layoutIndex)
{
    if (layoutIndex < 0 || layoutIndex >= static_cast<int>(_availableLayouts.size())) return;
    if (!canHandleGameplayInput()) return;

    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    _currentLayoutIndex = layoutIndex;
    const std::string& layoutFile = _availableLayouts[layoutIndex].filePath;
    GAME_LOG_INFO("Switching to layout index=%d name=%s file=%s",
                  layoutIndex,
                  _availableLayouts[layoutIndex].name.c_str(),
                  layoutFile.c_str());

    _gameState = GameState();
    _gameState.setVisibleTopCardCount(_selectedVisibleTopCardCount);
    if (!_gameState.loadLayout(layoutFile))
    {
        GAME_LOG_ERROR("Failed to load layout: %s", layoutFile.c_str());
        return;
    }

    startGame();

    const Color4B statusColor = theme.getColor4B("statusText");
    setStatusText(strings.get("levelPrefix") + _availableLayouts[layoutIndex].name, &statusColor);

    updateLevelSelectorUI();
}

void GameScene::switchToNextLayout()
{
    if (_availableLayouts.empty()) return;
    switchToLayout((_currentLayoutIndex + 1) % static_cast<int>(_availableLayouts.size()));
}

void GameScene::updateLevelSelectorUI()
{
    auto& strings = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();

    if (_levelTitleLabel)
    {
        if (!_availableLayouts.empty() &&
            _currentLayoutIndex >= 0 &&
            _currentLayoutIndex < static_cast<int>(_availableLayouts.size()))
        {
            _levelTitleLabel->setString(strings.get("levelPrefix") + _availableLayouts[_currentLayoutIndex].name);
        }
        else
        {
            _levelTitleLabel->setString(strings.get("level"));
        }
    }

    auto updateMenuLabel = [&](MenuItemLabel* item, const Color4B& color) {
        if (!item) return;
        auto* label = dynamic_cast<Label*>(item->getLabel());
        if (!label) return;
        label->setTextColor(color);
        label->setScale(1.0f);
    };

    updateMenuLabel(_levelSwitchMenuItem, theme.getColor4B("goldHighlight"));
    updateMenuLabel(_customLayoutMenuItem,
                    _customLayoutEditor && _customLayoutEditor->isActive()
                        ? theme.getColor4B("winGold")
                        : theme.getColor4B("goldHighlight"));
    updateMenuLabel(_saveLayoutMenuItem, theme.getColor4B("goldHighlight"));

    if (_saveLayoutMenuItem)
    {
        const bool active = _customLayoutEditor && _customLayoutEditor->isActive();
        _saveLayoutMenuItem->setVisible(active);
        _saveLayoutMenuItem->setEnabled(active);
    }
}
