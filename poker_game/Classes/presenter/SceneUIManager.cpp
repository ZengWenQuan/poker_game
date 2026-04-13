/**
 * @file SceneUIManager.cpp
 * @brief 场景 UI 管理器实现。
 *
 * 主要功能:
 *   - 管理状态栏文本更新
 *   - 管理玩法 UI 显隐
 */
#include "presenter/SceneUIManager.h"

USING_NS_CC;

void SceneUIManager::bindStatusLabel(Label* statusLabel)
{
    _statusLabel = statusLabel;
}

void SceneUIManager::bindUndoMenuItem(MenuItemLabel* undoMenuItem)
{
    _undoMenuItem = undoMenuItem;
}

void SceneUIManager::bindLevelSwitchMenuItem(MenuItemLabel* levelSwitchMenuItem)
{
    _levelSwitchMenuItem = levelSwitchMenuItem;
}

void SceneUIManager::setStatusText(const std::string& text, const Color4B* color)
{
    if (_statusLabel == nullptr) return;
    _statusLabel->setString(text);
    if (color)
    {
        _statusLabel->setTextColor(*color);
    }
}

void SceneUIManager::setGameplayUIVisible(bool visible, Node* mainArea, Node* topArea)
{
    if (mainArea) mainArea->setVisible(visible);
    if (topArea) topArea->setVisible(visible);
    if (_undoMenuItem) _undoMenuItem->setEnabled(visible);
    if (_levelSwitchMenuItem) _levelSwitchMenuItem->setEnabled(true);
}
