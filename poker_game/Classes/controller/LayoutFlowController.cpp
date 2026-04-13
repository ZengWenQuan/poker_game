/**
 * @file LayoutFlowController.cpp
 * @brief 布局流程控制器实现。
 *
 * 主要功能:
 *   - 根据布局名称查找并加载 layouts/*.json
 *   - 自动跳过用户自定义布局文件
 *   - advanceToNextLevel: 获取下一个关卡的布局名称
 */
#include "controller/LayoutFlowController.h"

#include "config/GlobalConfig.h"
#include "editor/CustomLayoutEditor.h"
#include <algorithm>

void LayoutFlowController::refresh()
{
    const std::string currentFile =
        (_currentIndex >= 0 && _currentIndex < static_cast<int>(_layouts.size()))
            ? _layouts[_currentIndex].filePath
            : std::string();

    _layouts = LayoutConfig::getAvailableLayouts();

    if (!currentFile.empty())
    {
        for (int i = 0; i < static_cast<int>(_layouts.size()); ++i)
        {
            if (_layouts[i].filePath == currentFile)
            {
                _currentIndex = i;
                return;
            }
        }
    }

    if (_currentIndex >= static_cast<int>(_layouts.size()))
    {
        _currentIndex = 0;
    }
}

void LayoutFlowController::initializeDefaultIndex(int defaultIndex)
{
    if (_layouts.empty())
    {
        _currentIndex = 0;
        return;
    }

    _currentIndex = (defaultIndex >= 0 && defaultIndex < static_cast<int>(_layouts.size())) ? defaultIndex : 0;
}

const std::vector<LayoutConfig::LayoutInfo>& LayoutFlowController::layouts() const
{
    return _layouts;
}

bool LayoutFlowController::hasLayouts() const
{
    return !_layouts.empty();
}

int LayoutFlowController::currentIndex() const
{
    return _currentIndex;
}

bool LayoutFlowController::selectIndex(int layoutIndex)
{
    if (layoutIndex < 0 || layoutIndex >= static_cast<int>(_layouts.size())) return false;
    _currentIndex = layoutIndex;
    return true;
}

void LayoutFlowController::selectSavedLayout(const std::string& relativePath)
{
    for (int i = 0; i < static_cast<int>(_layouts.size()); ++i)
    {
        if (_layouts[i].filePath == relativePath)
        {
            _currentIndex = i;
            return;
        }
    }
}

int LayoutFlowController::nextGameplayIndex() const
{
    if (_layouts.empty()) return -1;
    return (_currentIndex + 1) % static_cast<int>(_layouts.size());
}

int LayoutFlowController::nextEditorIndex(bool editingNewLayout) const
{
    if (_layouts.empty()) return -1;
    if (editingNewLayout) return 0;

    const int nextIndex = _currentIndex + 1;
    return nextIndex < static_cast<int>(_layouts.size()) ? nextIndex : -1;
}

std::string LayoutFlowController::currentDisplayName(bool editorActive, const std::string& editorLayoutName) const
{
    if (editorActive) return editorLayoutName;
    if (_currentIndex >= 0 && _currentIndex < static_cast<int>(_layouts.size()))
    {
        return _layouts[_currentIndex].name;
    }
    return std::string();
}

bool LayoutFlowController::loadCurrentIntoGameplay(GameState& state, int visibleTopCardCount) const
{
    if (_currentIndex < 0 || _currentIndex >= static_cast<int>(_layouts.size())) return false;

    state = GameState();
    state.setVisibleTopCardCount(visibleTopCardCount);
    return state.loadLayout(_layouts[_currentIndex].filePath);
}

bool LayoutFlowController::loadIndexIntoEditor(int layoutIndex, CustomLayoutEditor& editor)
{
    if (!selectIndex(layoutIndex)) return false;

    LayoutConfig layout;
    if (!layout.loadFromFile(_layouts[_currentIndex].filePath))
    {
        return false;
    }

    editor.loadLayout(layout, _layouts[_currentIndex].filePath, _layouts[_currentIndex].name);
    return true;
}
