#ifndef POKER_GAME_GAMEPLAY_PRESENTER_H
#define POKER_GAME_GAMEPLAY_PRESENTER_H

#include "cocos2d.h"
#include "controller/GameController.h"
#include <functional>

class MainAreaView;
class TopAreaView;
class GameState;

class GameplayPresenter
{
public:
    using StatusCallback = std::function<void(const std::string&, const cocos2d::Color4B*)>;

    GameplayPresenter(cocos2d::Node* host,
                      MainAreaView* mainArea,
                      TopAreaView* topArea,
                      GameController& controller,
                      GameState& state);

    void setStatusCallback(const StatusCallback& cb);

    void refreshViews();
    void syncTopAreaState(bool animated);
    void handleResult(const GameActionResult& result);

    void setInputLocked(bool locked);
    bool isInputLocked() const;
    bool isNearTopCardArea(const cocos2d::Vec2& worldPos) const;

private:
    void refreshMainArea();
    void finishAnimation(const std::string& statusText);

    cocos2d::Node* _host;
    MainAreaView* _mainArea;
    TopAreaView* _topArea;
    GameController& _controller;
    GameState& _state;
    StatusCallback _setStatus;
    bool _inputLocked = false;
};

#endif
