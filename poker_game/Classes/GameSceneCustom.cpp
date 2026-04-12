#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "presenter/GameplayPresenter.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

// 切换自定义布局模式，并同步隐藏/显示玩法 UI。
void GameScene::toggleCustomLayoutMode()
{
    if (_customLayoutEditor == nullptr) return;

    // 自定义布局模式和正式玩法模式互斥，切换时同步隐藏主玩法视图。
    _customLayoutEditor->toggle();
    const bool gameplayVisible = isGameplayMode();
    _mainArea->setVisible(gameplayVisible);
    _topArea->setVisible(gameplayVisible);

    if (_undoMenuItem) _undoMenuItem->setEnabled(gameplayVisible);
    if (_levelSwitchMenuItem) _levelSwitchMenuItem->setEnabled(gameplayVisible);

    updateLevelSelectorUI();
}

// 自定义布局保存成功后的处理：刷新列表并切换到新布局。
void GameScene::onCustomLayoutSaved(const std::string& relativePath, const std::string& layoutName)
{
    // 保存成功后立即刷新布局列表，并尝试切到刚保存的新布局。
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
