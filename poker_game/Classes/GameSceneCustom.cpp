#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "presenter/GameplayPresenter.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

void GameScene::toggleCustomLayoutMode()
{
    if (_customLayoutEditor == nullptr) return;

    _customLayoutEditor->toggle();
    const bool gameplayVisible = isGameplayMode();
    _mainArea->setVisible(gameplayVisible);
    _topArea->setVisible(gameplayVisible);

    if (_undoMenuItem) _undoMenuItem->setEnabled(gameplayVisible);
    if (_levelSwitchMenuItem) _levelSwitchMenuItem->setEnabled(gameplayVisible);

    updateLevelSelectorUI();
}

void GameScene::onCustomLayoutSaved(const std::string& relativePath, const std::string& layoutName)
{
    _mainArea->setVisible(true);
    _topArea->setVisible(true);
    if (_undoMenuItem) _undoMenuItem->setEnabled(true);
    if (_levelSwitchMenuItem) _levelSwitchMenuItem->setEnabled(true);

    refreshAvailableLayouts();

    int savedIndex = -1;
    for (int i = 0; i < static_cast<int>(_availableLayouts.size()); ++i)
    {
        if (_availableLayouts[i].filePath == relativePath)
        {
            savedIndex = i;
            break;
        }
    }

    setStatusText(GlobalConfig::getInstance().get("saveLayoutSuccess") + layoutName);
    updateLevelSelectorUI();

    if (savedIndex >= 0)
    {
        switchToLayout(savedIndex);
        _gameplayPresenter->refreshViews();
    }
}
