/**
 * @file GameScene.cpp
 * @brief 游戏主场景入口实现。
 *
 * 主要功能:
 *   - 创建游戏主场景，管理游戏流程状态切换
 *   - 协调 GameplayPresenter、SceneChromePresenter、CustomLayoutEditor 协同工作
 */
#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "logging/GameLogger.h"
#include "presenter/GameplayPresenter.h"
#include "presenter/GameplayView.h"
#include "presenter/SceneChromePresenter.h"
#include "presenter/SceneUIManager.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// 释放控制器、Presenter、编辑器等堆对象。
GameScene::~GameScene()
{
    delete _sceneChromePresenter;
    delete _gameplayPresenter;
    delete _gameplayView;
    delete _sceneUIManager;
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

    _sceneChromePresenter = new SceneChromePresenter();
    _sceneUIManager = new SceneUIManager();

    // 初始化顺序很重要：先把静态 UI 搭起来，再初始化控制器和 presenter，最后开局。
    buildBackground();
    buildUI();
    buildLevelSelector();
    initializeGameController();
    _gameplayView = new GameplayView(_mainArea, _topArea);
    _gameplayPresenter = new GameplayPresenter(this, _gameplayView, *_controller, _gameState);
    _gameplayPresenter->setStatusCallback([this](const std::string& text, const Color4B* color) {
        _sceneUIManager->setStatusText(text, color);
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
