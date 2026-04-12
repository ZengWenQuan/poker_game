#ifndef POKER_GAME_GAMEPLAY_PRESENTER_H
#define POKER_GAME_GAMEPLAY_PRESENTER_H

#include "cocos2d.h"
#include "controller/GameController.h"
#include <functional>

class MainAreaView;
class TopAreaView;
class GameState;

// Presenter：连接 GameController/GameState 与视图层，触发动画和 UI 更新。
class GameplayPresenter
{
public:
    using StatusCallback = std::function<void(const std::string&, const cocos2d::Color4B*)>;

    // Presenter 负责把 Controller/State 的结果翻译成具体界面刷新和动画。
    // host: 动画挂载父节点；mainArea/topArea: 视图组件；
    // controller/state: 业务逻辑入口与数据。
    GameplayPresenter(cocos2d::Node* host,
                      MainAreaView* mainArea,
                      TopAreaView* topArea,
                      GameController& controller,
                      GameState& state);

    // 设置状态栏输出回调。
    void setStatusCallback(const StatusCallback& cb);

    // 全量刷新入口，通常在开局、切关、动画结束后调用。
    void refreshViews();
    // 同步顶部牌区状态，支持动画。
    // animated: 是否播放动画。
    void syncTopAreaState(bool animated);
    // 根据一次游戏动作结果驱动 UI。
    // result: 控制器返回的动作结果摘要。
    void handleResult(const GameActionResult& result);

    // 设置/查询输入锁，用于动画期间阻断交互。
    void setInputLocked(bool locked);
    bool isInputLocked() const;
    // 判断给定坐标是否接近顶部区域，便于判定拖拽落点。
    // worldPos: 世界坐标。
    bool isNearTopCardArea(const cocos2d::Vec2& worldPos) const;

private:
    // 刷新主牌区所有槽位。
    void refreshMainArea();
    // 动画完成后的收尾，更新状态文本。
    // statusText: 需要展示的提示。
    void finishAnimation(const std::string& statusText);

    cocos2d::Node* _host;        // 动画挂载宿主，通常是 GameScene
    MainAreaView* _mainArea;     // 主牌区视图
    TopAreaView* _topArea;       // 顶部牌区视图
    GameController& _controller; // 流程控制器
    GameState& _state;           // 数据状态
    StatusCallback _setStatus;   // 状态栏输出回调
    bool _inputLocked = false;   // 动画进行时锁输入
};

#endif
