#include "CustomLayoutEditor.h"
#include "config/GlobalConfig.h"
#include "logging/GameLogger.h"
#include "view/PokerCardView.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <algorithm>
#include <cctype>
#include <fstream>

USING_NS_CC;

namespace
{
float computeOverlapArea(const Rect& lhs, const Rect& rhs)
{
    const float left = std::max(lhs.getMinX(), rhs.getMinX());
    const float right = std::min(lhs.getMaxX(), rhs.getMaxX());
    const float bottom = std::max(lhs.getMinY(), rhs.getMinY());
    const float top = std::min(lhs.getMaxY(), rhs.getMaxY());
    if (right <= left || top <= bottom)
    {
        return 0.0f;
    }
    return (right - left) * (top - bottom);
}
}

CustomLayoutEditor::CustomLayoutEditor(Node* host)
    : _host(host)
{
}

CustomLayoutEditor::~CustomLayoutEditor()
{
    hideSaveDialog();
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
    buildEditorDeckView();
    if (_onStatus) _onStatus(GlobalConfig::getInstance().get("customModeOn"));
    GAME_LOG_INFO("Entered custom layout mode");
}

void CustomLayoutEditor::exit()
{
    if (!_active) return;
    hideSaveDialog();
    clearEditor();
    _active = false;
    if (_onStatus) _onStatus(GlobalConfig::getInstance().get("customModeOff"));
    GAME_LOG_INFO("Exited custom layout mode");
}

void CustomLayoutEditor::toggle()
{
    if (_active)
    {
        exit();
        return;
    }
    enter();
}

std::vector<PokerCard> CustomLayoutEditor::buildEditorDeck() const
{
    std::vector<PokerCard> deck;
    deck.reserve(52);

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

    return deck;
}

Rect CustomLayoutEditor::getCustomTrayRect() const
{
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    return Rect(60.0f, 40.0f, visibleSize.width - 120.0f, visibleSize.height * 0.28f);
}

Rect CustomLayoutEditor::getCustomMainAreaRect() const
{
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    return Rect(80.0f, visibleSize.height * 0.22f, visibleSize.width - 160.0f, visibleSize.height * 0.62f);
}

Vec2 CustomLayoutEditor::getCustomTrayPosition(int index) const
{
    const Rect trayRect = getCustomTrayRect();
    const int columns = 13;
    const int row = index / columns;
    const int col = index % columns;

    const float cardWidth = PokerCardView::getCardWidth();
    const float cardHeight = PokerCardView::getCardHeight();
    const float spacingX = std::min(cardWidth * 1.05f, (trayRect.size.width - cardWidth) / 12.0f);
    const float spacingY = std::min(cardHeight * 1.02f, (trayRect.size.height - cardHeight) / 3.0f);
    const float startX = trayRect.getMinX() + cardWidth * 0.5f;
    const float startY = trayRect.getMinY() + cardHeight * 0.5f;

    return Vec2(startX + spacingX * static_cast<float>(col),
                startY + spacingY * static_cast<float>(row));
}

void CustomLayoutEditor::buildEditorDeckView()
{
    clearEditor();

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _editorLayer = LayerColor::create(Color4B(0, 0, 0, 0), visibleSize.width, visibleSize.height);
    _editorLayer->setIgnoreAnchorPointForPosition(false);
    _editorLayer->setAnchorPoint(Vec2::ZERO);
    _editorLayer->setPosition(origin);
    _host->addChild(_editorLayer, 7);

    auto* mainAreaMask = LayerColor::create(Color4B(255, 255, 255, 18),
                                            getCustomMainAreaRect().size.width,
                                            getCustomMainAreaRect().size.height);
    mainAreaMask->setAnchorPoint(Vec2::ZERO);
    mainAreaMask->setPosition(getCustomMainAreaRect().origin);
    _editorLayer->addChild(mainAreaMask, 0);

    auto* trayMask = LayerColor::create(Color4B(0, 0, 0, 60),
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
            }
            else
            {
                _cardInMainArea[i] = false;
                _cardViews[i]->setPosition(_trayPositions[i]);
            }
            _dragCardIndex = -1;
        };
        listener->onTouchCancelled = listener->onTouchEnded;
        _host->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, cardView);
    }
}

void CustomLayoutEditor::clearEditor()
{
    _cards.clear();
    _cardViews.clear();
    _trayPositions.clear();
    _cardInMainArea.clear();
    _dragCardIndex = -1;

    if (_editorLayer)
    {
        _editorLayer->removeFromParent();
        _editorLayer = nullptr;
    }
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

    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _saveDialogLayer = LayerColor::create(Color4B(0, 0, 0, 150), visibleSize.width, visibleSize.height);
    _saveDialogLayer->setPosition(origin);
    _host->addChild(_saveDialogLayer, 20);

    auto* panel = LayerColor::create(Color4B(30, 45, 32, 235), 520.0f, 220.0f);
    panel->setIgnoreAnchorPointForPosition(false);
    panel->setAnchorPoint(Vec2(0.5f, 0.5f));
    panel->setPosition(origin + visibleSize / 2.0f);
    _saveDialogLayer->addChild(panel);

    auto* title = Label::createWithSystemFont(strings.get("saveLayoutTitle"), theme.getFont(), theme.getFontSize("levelTitle"));
    title->setPosition(Vec2(260.0f, 175.0f));
    panel->addChild(title);

    _saveNameEditBox = ui::EditBox::create(Size(360.0f, 56.0f), ui::Scale9Sprite::create());
    _saveNameEditBox->setPosition(Vec2(260.0f, 110.0f));
    _saveNameEditBox->setPlaceHolder(strings.get("saveLayoutPlaceholder").c_str());
    _saveNameEditBox->setMaxLength(48);
    _saveNameEditBox->setFontColor(Color3B::WHITE);
    _saveNameEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    panel->addChild(_saveNameEditBox);

    auto* confirmLabel = Label::createWithSystemFont(strings.get("saveLayoutConfirm"), theme.getFont(), theme.getFontSize("levelBtn"));
    auto* confirmItem = MenuItemLabel::create(confirmLabel, [this](Ref*) {
        const std::string layoutName = _saveNameEditBox ? _saveNameEditBox->getText() : "";
        if (layoutName.empty())
        {
            if (_onStatus) _onStatus(GlobalConfig::getInstance().get("saveLayoutEmpty"));
            return;
        }
        saveLayout(layoutName);
    });
    confirmItem->setPosition(Vec2(180.0f, 42.0f));

    auto* cancelLabel = Label::createWithSystemFont(strings.get("saveLayoutCancel"), theme.getFont(), theme.getFontSize("levelBtn"));
    auto* cancelItem = MenuItemLabel::create(cancelLabel, [this](Ref*) {
        hideSaveDialog();
    });
    cancelItem->setPosition(Vec2(340.0f, 42.0f));

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

void CustomLayoutEditor::rebuildLayoutMetadata(std::vector<SlotLayout>& slots,
                                               const std::vector<int>& cardIndices) const
{
    const float cardWidth = PokerCardView::getCardWidth();
    const float cardHeight = PokerCardView::getCardHeight();

    std::vector<Rect> bounds;
    std::vector<int> zOrders;
    bounds.reserve(cardIndices.size());
    zOrders.reserve(cardIndices.size());

    for (int cardIndex : cardIndices)
    {
        const Vec2 pos = _cardViews[cardIndex]->getPosition();
        bounds.emplace_back(pos.x - cardWidth * 0.5f,
                            pos.y - cardHeight * 0.5f,
                            cardWidth,
                            cardHeight);
        zOrders.push_back(_cardViews[cardIndex]->getLocalZOrder());
    }

    std::vector<std::vector<int>> overlappingParents(slots.size());
    for (int lower = 0; lower < static_cast<int>(slots.size()); ++lower)
    {
        for (int upper = 0; upper < static_cast<int>(slots.size()); ++upper)
        {
            if (lower == upper || zOrders[upper] <= zOrders[lower]) continue;
            if (computeOverlapArea(bounds[lower], bounds[upper]) > 1.0f)
            {
                overlappingParents[lower].push_back(upper);
            }
        }
    }

    std::vector<int> order(slots.size());
    for (int i = 0; i < static_cast<int>(order.size()); ++i)
    {
        order[i] = i;
    }
    std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
        return zOrders[lhs] > zOrders[rhs];
    });

    for (int index : order)
    {
        int layer = 0;
        for (int parentIndex : overlappingParents[index])
        {
            layer = std::max(layer, slots[parentIndex].layer + 1);
        }
        slots[index].layer = layer;
    }

    for (auto& slot : slots)
    {
        slot.covers.clear();
    }

    for (int lower = 0; lower < static_cast<int>(slots.size()); ++lower)
    {
        for (int upper : overlappingParents[lower])
        {
            if (slots[upper].layer == slots[lower].layer - 1)
            {
                slots[lower].covers.push_back(slots[upper].id);
            }
        }
    }
}

std::string CustomLayoutEditor::sanitizeLayoutFileName(const std::string& rawName)
{
    std::string result;
    result.reserve(rawName.size());
    for (unsigned char ch : rawName)
    {
        if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' || ch == '?' ||
            ch == '"' || ch == '<' || ch == '>' || ch == '|')
        {
            result.push_back('_');
        }
        else if (std::isspace(ch))
        {
            result.push_back('_');
        }
        else
        {
            result.push_back(static_cast<char>(ch));
        }
    }

    while (!result.empty() && result.front() == '_')
    {
        result.erase(result.begin());
    }
    while (!result.empty() && result.back() == '_')
    {
        result.pop_back();
    }

    if (result.empty())
    {
        result = "custom_layout";
    }
    return result;
}

void CustomLayoutEditor::saveLayout(const std::string& layoutName)
{
    std::vector<int> cardIndices;
    for (int i = 0; i < static_cast<int>(_cardViews.size()); ++i)
    {
        if (_cardInMainArea[i])
        {
            cardIndices.push_back(i);
        }
    }

    if (cardIndices.empty())
    {
        if (_onStatus) _onStatus(GlobalConfig::getInstance().get("saveLayoutFail"));
        return;
    }

    std::vector<SlotLayout> slots;
    slots.reserve(cardIndices.size());

    const float centerX = GlobalConfig::getInstance().getDesignWidth() * 0.5f;
    const float centerY = GlobalConfig::getInstance().getDesignHeight() * 0.5f;

    for (int i = 0; i < static_cast<int>(cardIndices.size()); ++i)
    {
        const int cardIndex = cardIndices[i];
        const Vec2 pos = _cardViews[cardIndex]->getPosition();

        SlotLayout slot;
        slot.id = i;
        slot.pileIndex = 0;
        slot.rotation = 0.0f;
        slot.layer = 0;
        slot.x = pos.x - centerX;
        slot.y = pos.y - centerY;
        slots.push_back(slot);
    }

    rebuildLayoutMetadata(slots, cardIndices);

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
    const std::string fileName = sanitizeLayoutFileName(layoutName) + ".json";
    const std::string relativePath = "layouts/" + fileName;
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

    hideSaveDialog();
    exit();
    if (_onSaved) _onSaved(relativePath, layoutName);
}
