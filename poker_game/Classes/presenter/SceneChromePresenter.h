/**
 * @file SceneChromePresenter.h
 * @brief 场景界面装饰协调头文件。
 *
 * 主要功能:
 *   - bindUndo: 绑定撤销按钮
 *   - bindStartGame: 绑定开始游戏按钮
 *   - _undoMenuItem / _startGameMenuItem: 按钮引用
 */
#ifndef POKER_GAME_SCENE_CHROME_PRESENTER_H
#define POKER_GAME_SCENE_CHROME_PRESENTER_H

#include "cocos2d.h"
#include <string>

namespace cocos2d
{
class MenuItemLabel;
class Label;
}

class SceneChromePresenter
{
public:
    void bindUndo(cocos2d::MenuItemLabel* undoMenuItem);
    void bindLevelSelector(cocos2d::Label* levelTitleLabel,
                           cocos2d::MenuItemLabel* levelSwitchMenuItem,
                           cocos2d::MenuItemLabel* customLayoutMenuItem,
                           cocos2d::MenuItemLabel* saveLayoutMenuItem,
                           cocos2d::MenuItemLabel* langSwitchMenuItem);
    void bindStartOverlay(cocos2d::Label* titleLabel,
                          cocos2d::Label* subtitleLabel,
                          cocos2d::MenuItemLabel* difficultyOneMenuItem,
                          cocos2d::MenuItemLabel* difficultyTwoMenuItem,
                          cocos2d::MenuItemLabel* difficultyThreeMenuItem,
                          cocos2d::MenuItemLabel* startGameMenuItem);

    void refreshLevelSelector(const std::string& currentLevelName, bool editorActive) const;
    void refreshUndoText() const;
    void refreshStartOverlay(int selectedVisibleTopCardCount) const;
    void refreshAll(const std::string& currentLevelName,
                    bool editorActive,
                    int selectedVisibleTopCardCount) const;

private:
    static cocos2d::Label* getMenuLabel(cocos2d::MenuItemLabel* item);
    static void updateMenuLabel(cocos2d::MenuItemLabel* item,
                                const std::string& text,
                                const cocos2d::Color4B& color,
                                float scale = 1.0f);

    cocos2d::MenuItemLabel* _undoMenuItem = nullptr;
    cocos2d::Label* _levelTitleLabel = nullptr;
    cocos2d::MenuItemLabel* _levelSwitchMenuItem = nullptr;
    cocos2d::MenuItemLabel* _customLayoutMenuItem = nullptr;
    cocos2d::MenuItemLabel* _saveLayoutMenuItem = nullptr;
    cocos2d::MenuItemLabel* _langSwitchMenuItem = nullptr;
    cocos2d::Label* _startOverlayTitle = nullptr;
    cocos2d::Label* _startOverlaySubtitle = nullptr;
    cocos2d::MenuItemLabel* _difficultyOneMenuItem = nullptr;
    cocos2d::MenuItemLabel* _difficultyTwoMenuItem = nullptr;
    cocos2d::MenuItemLabel* _difficultyThreeMenuItem = nullptr;
    cocos2d::MenuItemLabel* _startGameMenuItem = nullptr;
};

#endif
