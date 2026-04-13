/**
 * @file SceneUIManager.h
 * @brief 场景 UI 管理器头文件。
 *
 * 主要功能:
 *   - 持有所有 UI 组件引用 (按钮、标签、面板)
 *   - 管理状态栏文本
 *   - 管理玩法 UI 的显隐状态
 */
#ifndef POKER_GAME_SCENE_UI_MANAGER_H
#define POKER_GAME_SCENE_UI_MANAGER_H

#include "cocos2d.h"
#include <string>

// 场景 UI 管理器：从 GameScene 中提取的 UI 状态管理职责。
class SceneUIManager
{
public:
    // 绑定状态文本标签。
    void bindStatusLabel(cocos2d::Label* statusLabel);
    // 绑定撤销按钮。
    void bindUndoMenuItem(cocos2d::MenuItemLabel* undoMenuItem);
    // 绑定关卡切换按钮。
    void bindLevelSwitchMenuItem(cocos2d::MenuItemLabel* levelSwitchMenuItem);

    // 更新状态栏文案，可选自定义颜色。
    void setStatusText(const std::string& text, const cocos2d::Color4B* color = nullptr);

    // 设置玩法 UI 的显隐（编辑模式切换时调用）。
    void setGameplayUIVisible(bool visible,
                               cocos2d::Node* mainArea,
                               cocos2d::Node* topArea);

private:
    cocos2d::Label* _statusLabel = nullptr;
    cocos2d::MenuItemLabel* _undoMenuItem = nullptr;
    cocos2d::MenuItemLabel* _levelSwitchMenuItem = nullptr;
};

#endif
