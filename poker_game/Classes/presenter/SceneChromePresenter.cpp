/**
 * @file SceneChromePresenter.cpp
 * @brief 场景界面装饰协调实现。
 *
 * 主要功能:
 *   - 绑定撤销按钮并响应撤销/重做操作
 *   - 绑定开始游戏按钮
 */
#include "presenter/SceneChromePresenter.h"

#include "config/GlobalConfig.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

void SceneChromePresenter::bindUndo(MenuItemLabel* undoMenuItem)
{
    _undoMenuItem = undoMenuItem;
}

void SceneChromePresenter::bindLevelSelector(Label* levelTitleLabel,
                                             MenuItemLabel* levelSwitchMenuItem,
                                             MenuItemLabel* customLayoutMenuItem,
                                             MenuItemLabel* saveLayoutMenuItem,
                                             MenuItemLabel* langSwitchMenuItem)
{
    _levelTitleLabel = levelTitleLabel;
    _levelSwitchMenuItem = levelSwitchMenuItem;
    _customLayoutMenuItem = customLayoutMenuItem;
    _saveLayoutMenuItem = saveLayoutMenuItem;
    _langSwitchMenuItem = langSwitchMenuItem;
}

void SceneChromePresenter::bindStartOverlay(Label* titleLabel,
                                            Label* subtitleLabel,
                                            MenuItemLabel* difficultyOneMenuItem,
                                            MenuItemLabel* difficultyTwoMenuItem,
                                            MenuItemLabel* difficultyThreeMenuItem,
                                            MenuItemLabel* startGameMenuItem)
{
    _startOverlayTitle = titleLabel;
    _startOverlaySubtitle = subtitleLabel;
    _difficultyOneMenuItem = difficultyOneMenuItem;
    _difficultyTwoMenuItem = difficultyTwoMenuItem;
    _difficultyThreeMenuItem = difficultyThreeMenuItem;
    _startGameMenuItem = startGameMenuItem;
}

void SceneChromePresenter::refreshLevelSelector(const std::string& currentLevelName, bool editorActive) const
{
    auto& cfg = GlobalConfig::getInstance();
    const Color4B gold = cfg.getColor4B("goldHighlight");
    const Color4B active = cfg.getColor4B("winGold");

    if (_levelTitleLabel)
    {
        _levelTitleLabel->setString(currentLevelName.empty()
            ? cfg.get("level")
            : cfg.get("levelPrefix") + currentLevelName);
    }

    updateMenuLabel(_levelSwitchMenuItem, cfg.get("switchLevel"), gold);
    updateMenuLabel(_customLayoutMenuItem, cfg.get("customLayout"), editorActive ? active : gold);
    updateMenuLabel(_saveLayoutMenuItem, cfg.get("saveLayout"), gold);
    updateMenuLabel(_langSwitchMenuItem, cfg.get("langSwitch"), gold);

    if (_saveLayoutMenuItem)
    {
        _saveLayoutMenuItem->setVisible(editorActive);
        _saveLayoutMenuItem->setEnabled(editorActive);
    }
}

void SceneChromePresenter::refreshUndoText() const
{
    updateMenuLabel(_undoMenuItem, GlobalConfig::getInstance().get("undo"),
                    GlobalConfig::getInstance().getColor4B("buttonText"));
}

void SceneChromePresenter::refreshStartOverlay(int selectedVisibleTopCardCount) const
{
    auto& cfg = GlobalConfig::getInstance();
    const Color4B gold = cfg.getColor4B("goldHighlight");
    const Color4B active = cfg.getColor4B("winGold");
    const float selScale = cfg.getSelectedScale();

    if (_startOverlayTitle) _startOverlayTitle->setString(cfg.get("startGame"));
    if (_startOverlaySubtitle) _startOverlaySubtitle->setString(cfg.get("openBaseCards"));

    updateMenuLabel(_difficultyOneMenuItem, cfg.get("difficultyOne") + cfg.get("difficultyHard"),
                    selectedVisibleTopCardCount == 1 ? active : gold,
                    selectedVisibleTopCardCount == 1 ? selScale : 1.0f);
    updateMenuLabel(_difficultyTwoMenuItem, cfg.get("difficultyTwo") + cfg.get("difficultyMedium"),
                    selectedVisibleTopCardCount == 2 ? active : gold,
                    selectedVisibleTopCardCount == 2 ? selScale : 1.0f);
    updateMenuLabel(_difficultyThreeMenuItem, cfg.get("difficultyThree") + cfg.get("difficultyEasy"),
                    selectedVisibleTopCardCount == 3 ? active : gold,
                    selectedVisibleTopCardCount == 3 ? selScale : 1.0f);
    updateMenuLabel(_startGameMenuItem, cfg.get("startGame"), gold);
}

void SceneChromePresenter::refreshAll(const std::string& currentLevelName,
                                      bool editorActive,
                                      int selectedVisibleTopCardCount) const
{
    refreshUndoText();
    refreshLevelSelector(currentLevelName, editorActive);
    refreshStartOverlay(selectedVisibleTopCardCount);
}

Label* SceneChromePresenter::getMenuLabel(MenuItemLabel* item)
{
    return item ? dynamic_cast<Label*>(item->getLabel()) : nullptr;
}

void SceneChromePresenter::updateMenuLabel(MenuItemLabel* item,
                                           const std::string& text,
                                           const Color4B& color,
                                           float scale)
{
    auto* label = getMenuLabel(item);
    if (!label) return;
    label->setString(text);
    label->setTextColor(color);
    label->setScale(scale);
}
