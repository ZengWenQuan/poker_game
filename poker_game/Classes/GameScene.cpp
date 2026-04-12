#include "GameScene.h"
#include "editor/CustomLayoutEditor.h"
#include "logging/GameLogger.h"
#include "presenter/GameplayPresenter.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

GameScene::~GameScene()
{
    delete _gameplayPresenter;
    delete _customLayoutEditor;
    delete _controller;
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        GAME_LOG_ERROR("GameScene init failed");
        return false;
    }

    GAME_LOG_INFO("GameScene init started");

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
