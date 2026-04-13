/**
 * @file GameSceneCustom.cpp
 * @brief 自定义布局编辑场景实现。
 *
 * 主要功能:
 *   - 构建自定义布局编辑界面
 *   - 初始化 CustomLayoutEditor 并进入编辑模式
 */
#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "presenter/GameplayPresenter.h"
#include "presenter/SceneUIManager.h"
#include "view/MainAreaView.h"
#include "view/TopAreaView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

void GameScene::setGameplayUIVisible(bool visible)
{
    if (_sceneUIManager) _sceneUIManager->setGameplayUIVisible(visible, _mainArea, _topArea);
    updateLevelSelectorUI();
}

// 切换自定义布局模式，并同步隐藏/显示玩法 UI。
void GameScene::toggleCustomLayoutMode()
{
    if (_customLayoutEditor == nullptr) return;

    if (_customLayoutEditor->isActive())
    {
        _customLayoutEditor->confirmSaveBeforeAction([this]() {
            _customLayoutEditor->exit();
            setGameplayUIVisible(true);
        });
        return;
    }

    _customLayoutEditor->enter();
    setGameplayUIVisible(false);
}

// 自定义布局保存成功后的处理：刷新列表，并保持在当前编辑上下文。
void GameScene::onCustomLayoutSaved(const std::string& relativePath, const std::string& layoutName)
{
    _layoutFlowController.refresh();
    _layoutFlowController.selectSavedLayout(relativePath);

    setStatusText(GlobalConfig::getInstance().get("saveLayoutSuccess") + layoutName);
    updateLevelSelectorUI();
}
