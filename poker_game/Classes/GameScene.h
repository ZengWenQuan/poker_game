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

// 主场景：负责搭建界面、处理关卡切换与启动流程，并将交互事件转发给控制器。
class GameScene : public cocos2d::Scene
{
public:
    // 创建游戏场景实例并返回 cocos2d 兼容的指针。
    static cocos2d::Scene* createScene();
    // 初始化场景元素、控制器和默认关卡。
    virtual bool init() override;
    ~GameScene() override;

    CREATE_FUNC(GameScene);

private:
    // 构建顺序按“背景 -> 基础 UI -> 关卡入口 -> 控制器 -> presenter”展开，
    // 让场景初始化职责清晰分层。
    // 创建背景精灵与布局承载节点。
    void buildBackground();
    // 创建顶部/底部 UI、状态文本和按钮。
    void buildUI();
    // 构造 GameController 并注入 GameState、视图层依赖。
    void initializeGameController();
    // 根据当前关卡配置启动一局游戏。
    void startGame();
    // 绑定 UI 与控制器的事件回调。
    void bindCallbacks();
    // 更新状态提示文本；可选自定义颜色。
    // text: 展示的文案；color: 传入则覆盖主题色。
    void setStatusText(const std::string& text, const cocos2d::Color4B* color = nullptr);
    // 当前是否处于对局（非关卡选择）模式。
    bool isGameplayMode() const;
    // 是否允许处理对局内的输入（用于阻塞动画期间的操作）。
    bool canHandleGameplayInput() const;

    // 牌拖拽结束时的处理入口。
    // slotIndex: 源牌槽索引；worldPos: 松手时的世界坐标。
    void onCardDragEnd(int slotIndex, const cocos2d::Vec2& worldPos);

    // 关卡选择与开局弹层。
    // 构建关卡按钮与标签。
    void buildLevelSelector();
    // 切换到指定布局关卡。
    // layoutIndex: _availableLayouts 中的目标索引。
    void switchToLayout(int layoutIndex);
    // 切换到下一个关卡（循环）。
    void switchToNextLayout();
    // 刷新关卡按钮文案和状态。
    void updateLevelSelectorUI();
    // 扫描配置目录并重建关卡列表。
    void refreshAvailableLayouts();
    // 搭建开局覆盖层与难度选择。
    void buildStartGameOverlay();
    // 按当前选定难度重开一局。
    void restartGameWithSelectedDifficulty();
    // 更新难度按钮的选中态。
    void updateStartGameDifficultyUI();

    // 打开/关闭自定义布局编辑模式。
    void toggleCustomLayoutMode();
    // 自定义布局保存后的回调。
    // relativePath: 布局文件相对路径；layoutName: 展示用名称。
    void onCustomLayoutSaved(const std::string& relativePath, const std::string& layoutName);

    // 视图组件。
    MainAreaView* _mainArea = nullptr;
    TopAreaView* _topArea = nullptr;
    cocos2d::MenuItemLabel* _undoMenuItem = nullptr;
    cocos2d::Label* _statusLabel = nullptr;

    // 关卡选择 UI。
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

    // 逻辑组件。
    GameState _gameState;
    GameController* _controller = nullptr;
    GameplayPresenter* _gameplayPresenter = nullptr;
    CustomLayoutEditor* _customLayoutEditor = nullptr;
};

#endif
