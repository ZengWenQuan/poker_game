#ifndef POKER_GAME_GAME_SCENE_H
#define POKER_GAME_GAME_SCENE_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "model/GameState.h"
#include "model/LayoutConfig.h"
#include "controller/GameController.h"

class MainAreaView;
class TopAreaView;
class CustomLayoutEditor;
class GameplayPresenter;

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    ~GameScene() override;

    CREATE_FUNC(GameScene);

private:
    void buildBackground();
    void buildUI();
    void initializeGameController();
    void startGame();
    void bindCallbacks();
    void setStatusText(const std::string& text, const cocos2d::Color4B* color = nullptr);
    bool isGameplayMode() const;
    bool canHandleGameplayInput() const;

    void onCardDragEnd(int slotIndex, const cocos2d::Vec2& worldPos);

    // 关卡选择
    void buildLevelSelector();
    void switchToLayout(int layoutIndex);
    void switchToNextLayout();
    void updateLevelSelectorUI();
    void refreshAvailableLayouts();
    void buildStartGameOverlay();
    void restartGameWithSelectedDifficulty();
    void updateStartGameDifficultyUI();

    void toggleCustomLayoutMode();
    void onCustomLayoutSaved(const std::string& relativePath, const std::string& layoutName);

    // 视图组件
    MainAreaView* _mainArea = nullptr;
    TopAreaView* _topArea = nullptr;
    cocos2d::MenuItemLabel* _undoMenuItem = nullptr;
    cocos2d::Label* _statusLabel = nullptr;

    // 关卡选择 UI
    cocos2d::Menu* _levelMenu = nullptr;
    cocos2d::Label* _levelTitleLabel = nullptr;
    cocos2d::MenuItemLabel* _levelSwitchMenuItem = nullptr;
    cocos2d::MenuItemLabel* _customLayoutMenuItem = nullptr;
    cocos2d::MenuItemLabel* _saveLayoutMenuItem = nullptr;
    std::vector<LayoutConfig::LayoutInfo> _availableLayouts;
    int _currentLayoutIndex = 0;
    cocos2d::LayerColor* _startGameOverlay = nullptr;
    cocos2d::MenuItemLabel* _difficultyOneMenuItem = nullptr;
    cocos2d::MenuItemLabel* _difficultyTwoMenuItem = nullptr;
    cocos2d::MenuItemLabel* _difficultyThreeMenuItem = nullptr;
    cocos2d::MenuItemLabel* _startGameMenuItem = nullptr;
    int _selectedVisibleTopCardCount = 1;

    // 逻辑组件
    GameState _gameState;
    GameController* _controller = nullptr;
    GameplayPresenter* _gameplayPresenter = nullptr;
    CustomLayoutEditor* _customLayoutEditor = nullptr;
};

#endif
