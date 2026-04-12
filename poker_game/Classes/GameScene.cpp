#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "logging/GameLogger.h"
#include "presenter/GameplayPresenter.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// 释放控制器、Presenter、编辑器等堆对象。
GameScene::~GameScene()
{
    delete _gameplayPresenter;
    delete _customLayoutEditor;
    delete _controller;
}

// 场景初始化：搭建 UI、加载配置、创建控制器与 Presenter。
bool GameScene::init()
{
    if (!Scene::init())
    {
        GAME_LOG_ERROR("GameScene init failed");
        return false;
    }

    GAME_LOG_INFO("GameScene init started");

    // 初始化顺序很重要：先把静态 UI 搭起来，再初始化控制器和 presenter，最后开局。
    buildBackground();
    buildUI();
    buildLevelSelector();
    initializeGameController();
    _gameplayPresenter = new GameplayPresenter(this, _mainArea, _topArea, *_controller, _gameState);
    _gameplayPresenter->setStatusCallback([this](const std::string& text, const Color4B* color) {
        setStatusText(text, color);
    });
    startGame();
    bindCallbacks();
    buildStartGameOverlay();

    _customLayoutEditor = new CustomLayoutEditor(this);
    _customLayoutEditor->setStatusCallback([this](const std::string& text) { setStatusText(text); });
    _customLayoutEditor->setSavedCallback([this](const std::string& relativePath, const std::string& layoutName) {
        onCustomLayoutSaved(relativePath, layoutName);
    });

    GAME_LOG_INFO("GameScene init finished");
    return true;
}
