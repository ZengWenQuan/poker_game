/**
 * @file LayoutFlowController.h
 * @brief 布局流程控制器头文件。
 *
 * 主要功能:
 *   - advanceToNextLevel: 切换到下一关布局
 *   - loadLayoutByName: 按名称加载布局 JSON
 */
#ifndef POKER_GAME_LAYOUT_FLOW_CONTROLLER_H
#define POKER_GAME_LAYOUT_FLOW_CONTROLLER_H

#include "model/GameState.h"
#include "model/LayoutConfig.h"
#include <string>
#include <vector>

class CustomLayoutEditor;

class LayoutFlowController
{
public:
    void refresh();
    void initializeDefaultIndex(int defaultIndex);

    const std::vector<LayoutConfig::LayoutInfo>& layouts() const;
    bool hasLayouts() const;
    int currentIndex() const;
    bool selectIndex(int layoutIndex);
    void selectSavedLayout(const std::string& relativePath);

    int nextGameplayIndex() const;
    int nextEditorIndex(bool editingNewLayout) const;
    std::string currentDisplayName(bool editorActive, const std::string& editorLayoutName) const;

    bool loadCurrentIntoGameplay(GameState& state, int visibleTopCardCount) const;
    bool loadIndexIntoEditor(int layoutIndex, CustomLayoutEditor& editor);

private:
    std::vector<LayoutConfig::LayoutInfo> _layouts;
    int _currentIndex = 0;
};

#endif
