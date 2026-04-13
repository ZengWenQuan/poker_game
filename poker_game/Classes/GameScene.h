/**
 * @file GameScene.h
 * @brief 游戏主场景头文件。
 *
 * 主要功能:
 *   - 管理游戏状态流程 (开始 -> 关卡选择 -> 难度选择 -> 游戏中)
 *   - 管理自定义布局编辑器状态
 *   - 声明所有子模块的成员变量和公共接口
 */
#ifndef POKER_GAME_GAME_SCENE_H
#define POKER_GAME_GAME_SCENE_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "controller/LayoutFlowController.h"
#include "model/GameState.h"
#include "controller/GameController.h"

class MainAreaView;
class TopAreaView;
class CustomLayoutEditor;
class GameplayPresenter;
class GameplayView;
class SceneChromePresenter;
class SceneUIManager;

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
    // layoutIndex: 当前布局列表中的目标索引。
    void switchToLayout(int layoutIndex);
    // 切换到下一个关卡（循环）。
    void switchToNextLayout();
    // 同步编辑模式下主玩法 UI 的显示状态。
    void setGameplayUIVisible(bool visible);
    // 在编辑模式中加载指定关卡。
    void loadLayoutIntoEditor(int layoutIndex);
    // 刷新关卡按钮文案和状态。
    void updateLevelSelectorUI();
    // 获取当前应展示的关卡名称。
    std::string getCurrentLevelDisplayName() const;
    // 切换中英文并刷新所有 UI 文案。
    void toggleLanguage();
    // 刷新场景中所有文案（切换语言后调用）。
    void refreshAllUI();
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
    cocos2d::MenuItemLabel* _langSwitchMenuItem = nullptr;
    cocos2d::LayerColor* _startGameOverlay = nullptr;
    cocos2d::Label* _startOverlayTitle = nullptr;
    cocos2d::Label* _startOverlaySubtitle = nullptr;
    cocos2d::MenuItemLabel* _difficultyOneMenuItem = nullptr;
    cocos2d::MenuItemLabel* _difficultyTwoMenuItem = nullptr;
    cocos2d::MenuItemLabel* _difficultyThreeMenuItem = nullptr;
    cocos2d::MenuItemLabel* _startGameMenuItem = nullptr;
    int _selectedVisibleTopCardCount = 1; // 默认值，运行时由配置的 visibleTopCardCountMin 决定

    // 逻辑组件。
    LayoutFlowController _layoutFlowController;
    GameState _gameState;
    GameController* _controller = nullptr;
    GameplayPresenter* _gameplayPresenter = nullptr;
    GameplayView* _gameplayView = nullptr;
    SceneChromePresenter* _sceneChromePresenter = nullptr;
    SceneUIManager* _sceneUIManager = nullptr;
    CustomLayoutEditor* _customLayoutEditor = nullptr;
};

#endif
