/**
 * @file GameSceneLevel.cpp
 * @brief 关卡选择场景实现。
 *
 * 主要功能:
 *   - 构建关卡选择按钮 (NEXT/CUSTOM/SAVE)
 *   - 更新关卡选择区 UI (标题、按钮文案、颜色)
 *   - 管理语言切换按钮 (右上角)
 */
#include "GameScene.h"
#include "config/GlobalConfig.h"
#include "editor/CustomLayoutEditor.h"
#include "logging/GameLogger.h"
#include "presenter/SceneChromePresenter.h"
#include "presenter/SceneUIManager.h"
#include "view/UILabelHelper.h"

USING_NS_CC;

void GameScene::buildLevelSelector()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _layoutFlowController.refresh();

    _levelTitleLabel = UiLabelHelper::create(strings.get("level"), theme.getFont(), theme.getFontSize("levelTitle"));
    _levelTitleLabel->setTextColor(theme.getColor4B("goldHighlight"));
    _levelTitleLabel->setPosition(Vec2(origin.x + visibleSize.width * cfg.getLevelTitlePositionRatio().x,
                                       origin.y + visibleSize.height * cfg.getLevelTitlePositionRatio().y));
    addChild(_levelTitleLabel, 6);

    auto* nextLabel = UiLabelHelper::create(strings.get("switchLevel"), theme.getFont(), theme.getFontSize("levelBtn"));
    nextLabel->setTextColor(theme.getColor4B("buttonText"));
    _levelSwitchMenuItem = MenuItemLabel::create(nextLabel, [this](Ref*) {
        switchToNextLayout();
    });
    _levelSwitchMenuItem->setPosition(Vec2(origin.x + visibleSize.width * cfg.getLevelBtnStartRatio().x
                                           + cfg.getLevelBtnWidth() * 0.5f,
                                           origin.y + visibleSize.height * cfg.getLevelBtnStartRatio().y));

    auto* customLabel = UiLabelHelper::create(strings.get("customLayout"), theme.getFont(), theme.getFontSize("levelBtn"));
    customLabel->setTextColor(theme.getColor4B("buttonText"));
    _customLayoutMenuItem = MenuItemLabel::create(customLabel, [this](Ref*) {
        toggleCustomLayoutMode();
    });
    _customLayoutMenuItem->setPosition(_levelSwitchMenuItem->getPosition() + Vec2(-150.0f, 0.0f));

    auto* saveLabel = UiLabelHelper::create(strings.get("saveLayout"), theme.getFont(), theme.getFontSize("levelBtn"));
    saveLabel->setTextColor(theme.getColor4B("buttonText"));
    _saveLayoutMenuItem = MenuItemLabel::create(saveLabel, [this](Ref*) {
        if (_customLayoutEditor && _customLayoutEditor->isActive())
        {
            _customLayoutEditor->saveCurrentLayout();
        }
    });
    _saveLayoutMenuItem->setPosition(_customLayoutMenuItem->getPosition() + Vec2(-120.0f, 0.0f));
    _saveLayoutMenuItem->setVisible(false);
    _saveLayoutMenuItem->setEnabled(false);

    auto* langLabel = UiLabelHelper::create(strings.get("langSwitch"), theme.getFont(), theme.getFontSize("levelBtn"));
    langLabel->setTextColor(theme.getColor4B("buttonText"));
    _langSwitchMenuItem = MenuItemLabel::create(langLabel, [this](Ref*) {
        toggleLanguage();
    });
    _langSwitchMenuItem->setPosition(_customLayoutMenuItem->getPosition() + Vec2(0.0f, 56.0f));

    _levelMenu = Menu::create(_levelSwitchMenuItem,
                              _customLayoutMenuItem,
                              _saveLayoutMenuItem,
                              _langSwitchMenuItem,
                              nullptr);
    _levelMenu->setPosition(Vec2::ZERO);
    addChild(_levelMenu, 6);

    if (_sceneChromePresenter)
    {
        _sceneChromePresenter->bindLevelSelector(_levelTitleLabel,
                                                 _levelSwitchMenuItem,
                                                 _customLayoutMenuItem,
                                                 _saveLayoutMenuItem,
                                                 _langSwitchMenuItem);
    }

    if (_sceneUIManager)
    {
        _sceneUIManager->bindLevelSwitchMenuItem(_levelSwitchMenuItem);
    }
}

void GameScene::loadLayoutIntoEditor(int layoutIndex)
{
    auto& strings = GlobalConfig::getInstance();
    if (!_layoutFlowController.loadIndexIntoEditor(layoutIndex, *_customLayoutEditor))
    {
        setStatusText(strings.get("loadLayoutFail"));
        return;
    }

    updateLevelSelectorUI();
    setStatusText(strings.get("editing") + getCurrentLevelDisplayName());
}

void GameScene::switchToLayout(int layoutIndex)
{
    if (!_layoutFlowController.selectIndex(layoutIndex)) return;
    if (_customLayoutEditor && _customLayoutEditor->isActive())
    {
        _customLayoutEditor->confirmSaveBeforeAction([this, layoutIndex]() {
            loadLayoutIntoEditor(layoutIndex);
        });
        return;
    }

    if (!canHandleGameplayInput()) return;

    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();
    const auto& layoutInfo = _layoutFlowController.layouts()[_layoutFlowController.currentIndex()];

    GAME_LOG_INFO("Switching to layout index=%d name=%s file=%s",
                  _layoutFlowController.currentIndex(),
                  layoutInfo.name.c_str(),
                  layoutInfo.filePath.c_str());

    if (!_layoutFlowController.loadCurrentIntoGameplay(_gameState, _selectedVisibleTopCardCount))
    {
        GAME_LOG_ERROR("Failed to load layout: %s", layoutInfo.filePath.c_str());
        return;
    }

    startGame();

    const Color4B statusColor = theme.getColor4B("statusText");
    setStatusText(strings.get("levelPrefix") + layoutInfo.name, &statusColor);
    updateLevelSelectorUI();
}

void GameScene::switchToNextLayout()
{
    if (_customLayoutEditor && _customLayoutEditor->isActive())
    {
        const int nextIndex = _layoutFlowController.nextEditorIndex(_customLayoutEditor->isEditingNewLayout());
        if (nextIndex >= 0)
        {
            switchToLayout(nextIndex);
            return;
        }

        _customLayoutEditor->confirmSaveBeforeAction([this]() {
            auto& strings = GlobalConfig::getInstance();
            _customLayoutEditor->startNewLayout();
            updateLevelSelectorUI();
            setStatusText(strings.get("editing") + _customLayoutEditor->getCurrentLayoutName());
        });
        return;
    }

    const int nextIndex = _layoutFlowController.nextGameplayIndex();
    if (nextIndex >= 0) switchToLayout(nextIndex);
}

void GameScene::updateLevelSelectorUI()
{
    if (_sceneChromePresenter)
    {
        _sceneChromePresenter->refreshLevelSelector(getCurrentLevelDisplayName(),
                                                    _customLayoutEditor && _customLayoutEditor->isActive());
    }
}
