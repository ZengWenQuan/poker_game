#ifndef POKER_GAME_CUSTOM_LAYOUT_EDITOR_H
#define POKER_GAME_CUSTOM_LAYOUT_EDITOR_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "PokerCard.h"
#include "model/LayoutConfig.h"
#include <functional>
#include <string>
#include <vector>

class PokerCardView;

class CustomLayoutEditor
{
public:
    explicit CustomLayoutEditor(cocos2d::Node* host);
    ~CustomLayoutEditor();

    void setStatusCallback(const std::function<void(const std::string&)>& cb);
    void setSavedCallback(const std::function<void(const std::string&, const std::string&)>& cb);

    bool isActive() const;
    void enter();
    void exit();
    void toggle();
    void showSaveDialog();

private:
    std::vector<PokerCard> buildEditorDeck() const;
    cocos2d::Rect getCustomTrayRect() const;
    cocos2d::Rect getCustomMainAreaRect() const;
    cocos2d::Vec2 getCustomTrayPosition(int index) const;
    void buildEditorDeckView();
    void clearEditor();
    void bringCardToFront(int cardIndex);
    void hideSaveDialog();
    void saveLayout(const std::string& layoutName);
    void rebuildLayoutMetadata(std::vector<SlotLayout>& slots,
                               const std::vector<int>& cardIndices) const;
    static std::string sanitizeLayoutFileName(const std::string& rawName);

    cocos2d::Node* _host;
    bool _active = false;
    cocos2d::LayerColor* _editorLayer = nullptr;
    cocos2d::LayerColor* _saveDialogLayer = nullptr;
    cocos2d::ui::EditBox* _saveNameEditBox = nullptr;
    std::vector<PokerCard> _cards;
    std::vector<PokerCardView*> _cardViews;
    std::vector<cocos2d::Vec2> _trayPositions;
    std::vector<bool> _cardInMainArea;
    int _dragCardIndex = -1;
    cocos2d::Vec2 _dragOffset;
    int _zCounter = 100;
    std::function<void(const std::string&)> _onStatus;
    std::function<void(const std::string&, const std::string&)> _onSaved;
};

#endif
