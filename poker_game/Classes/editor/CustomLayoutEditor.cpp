/**
 * @file CustomLayoutEditor.cpp
 * @brief 自定义布局编辑器实现。
 */
#include "CustomLayoutEditor.h"
#include "config/GlobalConfig.h"
#include "logging/GameLogger.h"
#include "model/CustomLayoutDraft.h"
#include "view/PokerCardView.h"
#include "view/UILabelHelper.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <fstream>

USING_NS_CC;

CustomLayoutEditor::CustomLayoutEditor(Node* host)
    : _host(host)
{
}

CustomLayoutEditor::~CustomLayoutEditor()
{
    hideSaveDialog();
    hideConfirmDialog();
    clearEditor();
}

void CustomLayoutEditor::setStatusCallback(const std::function<void(const std::string&)>& cb)
{
    _onStatus = cb;
}

void CustomLayoutEditor::setSavedCallback(const std::function<void(const std::string&, const std::string&)>& cb)
{
    _onSaved = cb;
}

bool CustomLayoutEditor::isActive() const
{
    return _active;
}

void CustomLayoutEditor::enter()
{
    if (_active) return;
    _active = true;
    startNewLayout();
    if (_onStatus) _onStatus(GlobalConfig::getInstance().get("customModeOn"));
    GAME_LOG_INFO("Entered custom layout mode");
}

void CustomLayoutEditor::exit()
{
    if (!_active) return;
    hideSaveDialog();
    hideConfirmDialog();
    clearEditor();
    _isDirty = false;
    _isNewLayout = true;
    _currentLayoutPath.clear();
    _currentLayoutName.clear();
    _pendingContinueAction = nullptr;
    _active = false;
    if (_onStatus) _onStatus(GlobalConfig::getInstance().get("customModeOff"));
    GAME_LOG_INFO("Exited custom layout mode");
}

void CustomLayoutEditor::startNewLayout()
{
    if (!_active) return;
    clearEditor();
    buildEditorDeckView();
    _isDirty = false;
    _isNewLayout = true;
    _currentLayoutPath.clear();
    _currentLayoutName = GlobalConfig::getInstance().get("newLayout");
}

bool CustomLayoutEditor::isEditingNewLayout() const
{
    return _isNewLayout;
}

const std::string& CustomLayoutEditor::getCurrentLayoutName() const
{
    return _currentLayoutName;
}

void CustomLayoutEditor::loadLayout(const LayoutConfig& layout,
                                    const std::string& relativePath,
                                    const std::string& layoutName)
{
    if (!_active || !_editorLayer) return;
    const auto& slots = layout.getSlots();
    if (slots.empty()) return;

    clearEditor();
    buildEditorDeckView();

    std::vector<int> rewardIndices;
    std::vector<int> normalIndices;
    for (int i = 0; i < static_cast<int>(_cards.size()); ++i)
    {
        if (_cards[i].isReward()) rewardIndices.push_back(i);
        else normalIndices.push_back(i);
    }
    auto allocCardIndex = [&](bool needReward) -> int {
        std::vector<int>& pool = needReward ? rewardIndices : normalIndices;
        if (pool.empty()) return -1;
        int idx = pool.back();
        pool.pop_back();
        return idx;
    };

    auto& cfg = GlobalConfig::getInstance();

    for (const auto& slot : slots)
    {
        int cardIndex = allocCardIndex(slot.isReward);
        if (cardIndex < 0 || cardIndex >= static_cast<int>(_cardViews.size())) continue;

        cocos2d::Vec2 scenePos = LayoutConfig::resolveSlotPosition(slot,
                                                                   layout.usePileCenterOrigin(),
                                                                   layout.getMainPileCount(),
                                                                   cfg.getDesignWidth(),
                                                                   cfg.getDesignHeight());
        _cardViews[cardIndex]->setPosition(scenePos);
        _cardInMainArea[cardIndex] = true;
        bringCardToFront(cardIndex);
    }

    _isDirty = false;
    _isNewLayout = false;
    _currentLayoutPath = relativePath;
    _currentLayoutName = layoutName;
}

std::vector<PokerCard> CustomLayoutEditor::buildEditorDeck() const
{
    auto& cfg = GlobalConfig::getInstance();
    std::vector<PokerCard> deck;
    deck.reserve(52 + cfg.getEditorRewardCardCount());

    const CardSuit suits[] = {
        CardSuit::Spade,
        CardSuit::Heart,
        CardSuit::Club,
        CardSuit::Diamond
    };

    for (CardSuit suit : suits)
    {
        for (int rank = static_cast<int>(CardRank::Ace); rank <= static_cast<int>(CardRank::King); ++rank)
        {
            deck.emplace_back(suit, static_cast<CardRank>(rank));
        }
    }

    for (int i = 0; i < cfg.getEditorRewardCardCount(); ++i)
    {
        deck.push_back(PokerCard::RewardCard());
    }

    return deck;
}

Rect CustomLayoutEditor::getCustomTrayRect() const
{
    auto& cfg = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    float marginLeft = cfg.getEditorFloat("trayMarginLeft", 60.0f);
    float marginBottom = cfg.getEditorFloat("trayMarginBottom", 40.0f);
    float widthAdjust = cfg.getEditorFloat("trayWidthAdjust", -120.0f);
    float heightRatio = cfg.getEditorFloat("trayHeightRatio", 0.28f);
    return Rect(marginLeft, marginBottom, visibleSize.width + widthAdjust, visibleSize.height * heightRatio);
}

Rect CustomLayoutEditor::getCustomMainAreaRect() const
{
    auto& cfg = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    float marginLeft = cfg.getEditorFloat("mainAreaMarginLeft", 80.0f);
    float bottomRatio = cfg.getEditorFloat("mainAreaBottomRatio", 0.22f);
    float widthAdjust = cfg.getEditorFloat("mainAreaWidthAdjust", -160.0f);
    float heightRatio = cfg.getEditorFloat("mainAreaHeightRatio", 0.62f);
    return Rect(marginLeft, visibleSize.height * bottomRatio, visibleSize.width + widthAdjust, visibleSize.height * heightRatio);
}

Vec2 CustomLayoutEditor::getCustomMainAreaGridStep() const
{
    auto& cfg = GlobalConfig::getInstance();
    const Rect mainAreaRect = getCustomMainAreaRect();
    const int divisions = cfg.getEditorInt("mainAreaGridDivisions", 100);
    return Vec2(mainAreaRect.size.width / static_cast<float>(divisions),
                mainAreaRect.size.height / static_cast<float>(divisions));
}

Vec2 CustomLayoutEditor::getCustomTrayPosition(int index) const
{
    auto& cfg = GlobalConfig::getInstance();
    const Rect trayRect = getCustomTrayRect();
    const int columns = cfg.getEditorInt("trayColumns", 13);
    const int row = index / columns;
    const int col = index % columns;

    const float cardWidth = PokerCardView::getCardWidth();
    const float cardHeight = PokerCardView::getCardHeight();
    const float spacingXRatio = cfg.getEditorFloat("traySpacingXRatio", 1.05f);
    const float spacingYRatio = cfg.getEditorFloat("traySpacingYRatio", 1.02f);
    const float spacingX = std::min(cardWidth * spacingXRatio, (trayRect.size.width - cardWidth) / 12.0f);
    const float spacingY = std::min(cardHeight * spacingYRatio, (trayRect.size.height - cardHeight) / 3.0f);
    const float startX = trayRect.getMinX() + cardWidth * 0.5f;
    const float startY = trayRect.getMinY() + cardHeight * 0.5f;

    return Vec2(startX + spacingX * static_cast<float>(col),
                startY + spacingY * static_cast<float>(row));
}

Vec2 CustomLayoutEditor::snapToMainAreaGrid(const Vec2& position) const
{
    const Rect mainAreaRect = getCustomMainAreaRect();
    const Vec2 step = getCustomMainAreaGridStep();
    const float halfCardWidth = PokerCardView::getCardWidth() * 0.5f;
    const float halfCardHeight = PokerCardView::getCardHeight() * 0.5f;

    const float minX = mainAreaRect.getMinX() + halfCardWidth;
    const float maxX = mainAreaRect.getMaxX() - halfCardWidth;
    const float minY = mainAreaRect.getMinY() + halfCardHeight;
    const float maxY = mainAreaRect.getMaxY() - halfCardHeight;

    const float clampedX = clampf(position.x, minX, maxX);
    const float clampedY = clampf(position.y, minY, maxY);
    const float snappedX = minX + std::round((clampedX - minX) / step.x) * step.x;
    const float snappedY = minY + std::round((clampedY - minY) / step.y) * step.y;

    return Vec2(clampf(snappedX, minX, maxX),
                clampf(snappedY, minY, maxY));
}

void CustomLayoutEditor::buildEditorDeckView()
{
    clearEditor();

    auto& cfg = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _editorLayer = LayerColor::create(Color4B(0, 0, 0, 0), visibleSize.width, visibleSize.height);
    _editorLayer->setIgnoreAnchorPointForPosition(false);
    _editorLayer->setAnchorPoint(Vec2::ZERO);
    _editorLayer->setPosition(origin);
    _host->addChild(_editorLayer, 7);

    auto* mainAreaMask = LayerColor::create(cfg.getColor4B("editorMainAreaMask"),
                                            getCustomMainAreaRect().size.width,
                                            getCustomMainAreaRect().size.height);
    mainAreaMask->setAnchorPoint(Vec2::ZERO);
    mainAreaMask->setPosition(getCustomMainAreaRect().origin);
    _editorLayer->addChild(mainAreaMask, 0);

    auto* trayMask = LayerColor::create(cfg.getColor4B("editorTrayMask"),
                                        getCustomTrayRect().size.width,
                                        getCustomTrayRect().size.height);
    trayMask->setAnchorPoint(Vec2::ZERO);
    trayMask->setPosition(getCustomTrayRect().origin);
    _editorLayer->addChild(trayMask, 0);

    _cards = buildEditorDeck();
    _cardViews.reserve(_cards.size());
    _trayPositions.reserve(_cards.size());
    _cardInMainArea.assign(_cards.size(), false);
    _zCounter = 100;

    for (int i = 0; i < static_cast<int>(_cards.size()); ++i)
    {
        const Vec2 trayPos = getCustomTrayPosition(i);
        _trayPositions.push_back(trayPos);

        auto* cardView = PokerCardView::create(_cards[i], true);
        cardView->setPosition(trayPos);
        _editorLayer->addChild(cardView, i + 1);
        _cardViews.push_back(cardView);

        auto* listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, i](Touch* touch, Event* event) {
            CC_UNUSED_PARAM(event);
            if (!_active || _editorLayer == nullptr) return false;
            auto* cardView = _cardViews[i];
            if (cardView == nullptr || !cardView->isVisible()) return false;

            const Vec2 localPos = cardView->convertToNodeSpace(touch->getLocation());
            const Rect bounds(0.0f, 0.0f, PokerCardView::getCardWidth(), PokerCardView::getCardHeight());
            if (!bounds.containsPoint(localPos)) return false;

            _dragCardIndex = i;
            const Vec2 touchInLayer = _editorLayer->convertToNodeSpace(touch->getLocation());
            _dragOffset = cardView->getPosition() - touchInLayer;
            bringCardToFront(i);
            return true;
        };
        listener->onTouchMoved = [this, i](Touch* touch, Event* event) {
            CC_UNUSED_PARAM(event);
            if (_dragCardIndex != i || _editorLayer == nullptr) return;
            const Vec2 touchInLayer = _editorLayer->convertToNodeSpace(touch->getLocation());
            _cardViews[i]->setPosition(touchInLayer + _dragOffset);
        };
        listener->onTouchEnded = [this, i](Touch* touch, Event* event) {
            CC_UNUSED_PARAM(touch);
            CC_UNUSED_PARAM(event);
            if (_dragCardIndex != i) return;

            const Vec2 pos = _cardViews[i]->getPosition();
            if (getCustomMainAreaRect().containsPoint(pos))
            {
                _cardInMainArea[i] = true;
                _cardViews[i]->setPosition(snapToMainAreaGrid(pos));
            }
            else
            {
                _cardInMainArea[i] = false;
                _cardViews[i]->setPosition(_trayPositions[i]);
            }
            markDirty();
            _dragCardIndex = -1;
        };
        listener->onTouchCancelled = listener->onTouchEnded;
        _host->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, cardView);
    }

    auto* mouseListener = EventListenerMouse::create();
    mouseListener->onMouseUp = [this](EventMouse* event) {
        if (!_active || _editorLayer == nullptr) return;
        if (event->getMouseButton() != EventMouse::MouseButton::BUTTON_RIGHT) return;

        const Vec2 worldPos = event->getLocationInView();
        for (int i = static_cast<int>(_cardViews.size()) - 1; i >= 0; --i)
        {
            auto* cardView = _cardViews[i];
            if (cardView == nullptr || !cardView->isVisible()) continue;
            if (!_cardInMainArea[i]) continue;

            const Vec2 localPos = cardView->convertToNodeSpace(worldPos);
            const Rect bounds(0.0f, 0.0f, PokerCardView::getCardWidth(), PokerCardView::getCardHeight());
            if (bounds.containsPoint(localPos))
            {
                _cardInMainArea[i] = false;
                cardView->setPosition(_trayPositions[i]);
                markDirty();
                break;
            }
        }
    };
    _host->getEventDispatcher()->addEventListenerWithFixedPriority(mouseListener, -1);
    _mouseListener = mouseListener;
}

void CustomLayoutEditor::clearEditor()
{
    _cards.clear();
    _cardViews.clear();
    _trayPositions.clear();
    _cardInMainArea.clear();
    _dragCardIndex = -1;

    if (_mouseListener)
    {
        _host->getEventDispatcher()->removeEventListener(_mouseListener);
        _mouseListener = nullptr;
    }

    if (_editorLayer)
    {
        _editorLayer->removeFromParent();
        _editorLayer = nullptr;
    }
}

void CustomLayoutEditor::markDirty()
{
    _isDirty = true;
}

void CustomLayoutEditor::bringCardToFront(int cardIndex)
{
    if (cardIndex < 0 || cardIndex >= static_cast<int>(_cardViews.size())) return;
    ++_zCounter;
    _cardViews[cardIndex]->setLocalZOrder(_zCounter);
}

void CustomLayoutEditor::showSaveDialog()
{
    if (!_active) return;
    hideSaveDialog();
    hideConfirmDialog();

    auto& cfg = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _saveDialogLayer = LayerColor::create(cfg.getColor4B("overlayMask"), visibleSize.width, visibleSize.height);
    _saveDialogLayer->setPosition(origin);
    _host->addChild(_saveDialogLayer, 20);

    const Size panelSize = cfg.getDialogSize("saveDialog", "panelSize", Size(520, 220));
    auto* panel = LayerColor::create(cfg.getColor4B("dialogPanelAlt"), panelSize.width, panelSize.height);
    panel->setIgnoreAnchorPointForPosition(false);
    panel->setAnchorPoint(Vec2(0.5f, 0.5f));
    panel->setPosition(origin + visibleSize / 2.0f);
    _saveDialogLayer->addChild(panel);

    const Vec2 titlePos = cfg.getDialogVec2("saveDialog", "titlePosition", Vec2(260, 175));
    auto* title = UiLabelHelper::create(strings.get("saveLayoutTitle"), cfg.getFont(), cfg.getFontSize("levelTitle"));
    title->setPosition(titlePos);
    panel->addChild(title);

    const Size editBoxSize = cfg.getDialogSize("saveDialog", "editBoxSize", Size(360, 56));
    const Vec2 editBoxPos = cfg.getDialogVec2("saveDialog", "editBoxPosition", Vec2(260, 110));
    const int maxLength = cfg.getDialogInt("saveDialog", "editBoxMaxLength", 48);
    _saveNameEditBox = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create());
    _saveNameEditBox->setPosition(editBoxPos);
    _saveNameEditBox->setPlaceHolder(strings.get("saveLayoutPlaceholder").c_str());
    _saveNameEditBox->setMaxLength(maxLength);
    _saveNameEditBox->setFont(cfg.getFont().c_str(), static_cast<int>(cfg.getFontSize("levelBtn")));
    _saveNameEditBox->setFontColor(cfg.getColor3B("buttonText"));
    _saveNameEditBox->setPlaceholderFont(cfg.getFont().c_str(), static_cast<int>(cfg.getFontSize("levelBtn")));
    _saveNameEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    _saveNameEditBox->setText(_isNewLayout ? "" : _currentLayoutName.c_str());
    panel->addChild(_saveNameEditBox);

    auto* confirmLabel = UiLabelHelper::create(strings.get("saveLayoutConfirm"), cfg.getFont(), cfg.getFontSize("levelBtn"));
    auto* confirmItem = MenuItemLabel::create(confirmLabel, [this](Ref*) {
        const std::string layoutName = _saveNameEditBox ? _saveNameEditBox->getText() : "";
        if (layoutName.empty())
        {
            if (_onStatus) _onStatus(GlobalConfig::getInstance().get("saveLayoutEmpty"));
            return;
        }
        saveLayoutToPath(CustomLayoutDraft::buildSavePath(_currentLayoutPath, _isNewLayout, layoutName),
                         layoutName);
    });
    const Vec2 confirmPos = cfg.getDialogVec2("saveDialog", "confirmPosition", Vec2(180, 42));
    confirmItem->setPosition(confirmPos);

    auto* cancelLabel = UiLabelHelper::create(strings.get("saveLayoutCancel"), cfg.getFont(), cfg.getFontSize("levelBtn"));
    auto* cancelItem = MenuItemLabel::create(cancelLabel, [this](Ref*) {
        _pendingContinueAction = nullptr;
        hideSaveDialog();
    });
    const Vec2 cancelPos = cfg.getDialogVec2("saveDialog", "cancelPosition", Vec2(340, 42));
    cancelItem->setPosition(cancelPos);

    auto* menu = Menu::create(confirmItem, cancelItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu);
}

void CustomLayoutEditor::hideSaveDialog()
{
    _saveNameEditBox = nullptr;
    if (_saveDialogLayer)
    {
        _saveDialogLayer->removeFromParent();
        _saveDialogLayer = nullptr;
    }
}

void CustomLayoutEditor::saveCurrentLayout()
{
    if (!_active) return;
    _isNewLayout ? showSaveDialog()
                 : saveLayoutToPath(CustomLayoutDraft::buildSavePath(_currentLayoutPath,
                                                                    _isNewLayout,
                                                                    _currentLayoutName),
                                    _currentLayoutName);
}

void CustomLayoutEditor::confirmSaveBeforeAction(const std::function<void()>& onContinue)
{
    if (!_active)
    {
        if (onContinue) onContinue();
        return;
    }

    if (!_isDirty)
    {
        if (onContinue) onContinue();
        return;
    }

    hideSaveDialog();
    hideConfirmDialog();
    _pendingContinueAction = onContinue;

    auto& cfg = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _confirmDialogLayer = LayerColor::create(cfg.getColor4B("overlayMask"), visibleSize.width, visibleSize.height);
    _confirmDialogLayer->setPosition(origin);
    _host->addChild(_confirmDialogLayer, 20);

    const Size panelSize = cfg.getDialogSize("confirmDialog", "panelSize", Size(620, 240));
    auto* panel = LayerColor::create(cfg.getColor4B("dialogPanelAlt"), panelSize.width, panelSize.height);
    panel->setIgnoreAnchorPointForPosition(false);
    panel->setAnchorPoint(Vec2(0.5f, 0.5f));
    panel->setPosition(origin + visibleSize / 2.0f);
    _confirmDialogLayer->addChild(panel);

    const Vec2 titlePos = cfg.getDialogVec2("confirmDialog", "titlePosition", Vec2(310, 185));
    auto* title = UiLabelHelper::create(strings.get("saveBeforeSwitchTitle"),
                                        cfg.getFont(),
                                        cfg.getFontSize("levelTitle"));
    title->setPosition(titlePos);
    panel->addChild(title);

    const Vec2 msgPos = cfg.getDialogVec2("confirmDialog", "messagePosition", Vec2(310, 118));
    auto* message = UiLabelHelper::create(strings.get("saveBeforeSwitchMessage"),
                                          cfg.getFont(),
                                          cfg.getFontSize("levelBtn"));
    message->setPosition(msgPos);
    panel->addChild(message);

    auto* saveLabel = UiLabelHelper::create(strings.get("saveBeforeSwitchSave"),
                                            cfg.getFont(),
                                            cfg.getFontSize("levelBtn"));
    auto* saveItem = MenuItemLabel::create(saveLabel, [this](Ref*) {
        hideConfirmDialog();
        saveCurrentLayout();
    });
    const Vec2 savePos = cfg.getDialogVec2("confirmDialog", "savePosition", Vec2(160, 50));
    saveItem->setPosition(savePos);

    auto* discardLabel = UiLabelHelper::create(strings.get("saveBeforeSwitchDiscard"),
                                               cfg.getFont(),
                                               cfg.getFontSize("levelBtn"));
    auto* discardItem = MenuItemLabel::create(discardLabel, [this](Ref*) {
        auto onContinue = _pendingContinueAction;
        _pendingContinueAction = nullptr;
        hideConfirmDialog();
        if (onContinue) onContinue();
    });
    const Vec2 discardPos = cfg.getDialogVec2("confirmDialog", "discardPosition", Vec2(310, 50));
    discardItem->setPosition(discardPos);

    auto* cancelLabel = UiLabelHelper::create(strings.get("saveLayoutCancel"),
                                              cfg.getFont(),
                                              cfg.getFontSize("levelBtn"));
    auto* cancelItem = MenuItemLabel::create(cancelLabel, [this](Ref*) {
        _pendingContinueAction = nullptr;
        hideConfirmDialog();
    });
    const Vec2 cancelPos = cfg.getDialogVec2("confirmDialog", "cancelPosition", Vec2(460, 50));
    cancelItem->setPosition(cancelPos);

    auto* menu = Menu::create(saveItem, discardItem, cancelItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu);
}

void CustomLayoutEditor::hideConfirmDialog()
{
    if (_confirmDialogLayer)
    {
        _confirmDialogLayer->removeFromParent();
        _confirmDialogLayer = nullptr;
    }
}

void CustomLayoutEditor::saveLayoutToPath(const std::string& relativePath, const std::string& layoutName)
{
    std::vector<CustomLayoutDraftCard> draftCards;
    draftCards.reserve(_cardViews.size());

    for (int i = 0; i < static_cast<int>(_cardViews.size()); ++i)
    {
        if (_cardInMainArea[i])
        {
            CustomLayoutDraftCard draftCard;
            draftCard.position = _cardViews[i]->getPosition();
            draftCard.zOrder = _cardViews[i]->getLocalZOrder();
            draftCard.isReward = _cards[i].isReward();
            draftCards.push_back(draftCard);
        }
    }

    auto& cfg = GlobalConfig::getInstance();
    const std::vector<SlotLayout> slots = CustomLayoutDraft::buildSlots(draftCards,
                                                                        PokerCardView::getCardWidth(),
                                                                        PokerCardView::getCardHeight(),
                                                                        cfg.getDesignWidth(),
                                                                        cfg.getDesignHeight());

    if (CustomLayoutDraft::hasUncoveredRewardCard(slots))
    {
        if (_onStatus) _onStatus(cfg.get("rewardCardMustBeCovered"));
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("name");
    writer.String(layoutName.c_str());
    writer.Key("mainPileCount");
    writer.Int(1);
    writer.Key("totalSlots");
    writer.Int(static_cast<int>(slots.size()));
    writer.Key("slots");
    writer.StartArray();
    for (const auto& slot : slots)
    {
        writer.StartObject();
        writer.Key("id");
        writer.Int(slot.id);
        writer.Key("x");
        writer.Double(slot.x);
        writer.Key("y");
        writer.Double(slot.y);
        writer.Key("pileIndex");
        writer.Int(slot.pileIndex);
        writer.Key("layer");
        writer.Int(slot.layer);
        writer.Key("rotation");
        writer.Double(slot.rotation);
        writer.Key("reward");
        writer.Bool(slot.isReward);
        writer.Key("covers");
        writer.StartArray();
        for (int parentId : slot.covers)
        {
            writer.Int(parentId);
        }
        writer.EndArray();
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();

    auto* fileUtils = FileUtils::getInstance();
    const std::string fullPath = fileUtils->getWritablePath() + relativePath;
    fileUtils->createDirectory(fileUtils->getWritablePath() + "layouts");

    std::ofstream ofs(fullPath, std::ios::out | std::ios::trunc);
    if (!ofs.is_open())
    {
        GAME_LOG_ERROR("Failed to open custom layout file for writing: %s", fullPath.c_str());
        if (_onStatus) _onStatus(GlobalConfig::getInstance().get("saveLayoutFail"));
        return;
    }

    ofs << buffer.GetString();
    ofs.close();

    GAME_LOG_INFO("Custom layout saved: name=%s file=%s slots=%d",
                  layoutName.c_str(),
                  fullPath.c_str(),
                  static_cast<int>(slots.size()));

    _isDirty = false;
    _isNewLayout = false;
    _currentLayoutPath = relativePath;
    _currentLayoutName = layoutName;
    hideSaveDialog();
    if (_onSaved) _onSaved(relativePath, layoutName);

    auto onContinue = _pendingContinueAction;
    _pendingContinueAction = nullptr;
    if (onContinue)
    {
        onContinue();
    }
}
