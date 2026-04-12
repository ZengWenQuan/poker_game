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

// 自定义布局编辑器：在场景中提供拖拽摆牌、输入名称并保存布局 JSON 的工作流。
class CustomLayoutEditor
{
public:
    // host: 承载编辑 UI 的父节点（通常是 GameScene）。
    explicit CustomLayoutEditor(cocos2d::Node* host);
    ~CustomLayoutEditor();

    // 自定义布局编辑器负责在场景内提供“拖卡摆布局 -> 保存 JSON”的轻量工作流。
    // cb: 状态文本回调，用于提示用户。
    void setStatusCallback(const std::function<void(const std::string&)>& cb);
    // cb: 保存成功回调，参数为相对路径与布局名称。
    void setSavedCallback(const std::function<void(const std::string&, const std::string&)>& cb);

    // 是否处于激活状态。
    bool isActive() const;
    // 进入编辑模式（显示编辑 UI）。
    void enter();
    // 退出编辑模式并清理 UI。
    void exit();
    // 激活/退出的开关。
    void toggle();
    // 打开保存对话框。
    void showSaveDialog();

private:
    // 下面这些辅助函数分别负责：生成编辑用牌组、计算编辑区域、保存布局元数据。
    // 生成完整的编辑器牌堆（52 张）。
    std::vector<PokerCard> buildEditorDeck() const;
    // 获取自定义托盘区域矩形。
    cocos2d::Rect getCustomTrayRect() const;
    // 获取主编辑区域矩形。
    cocos2d::Rect getCustomMainAreaRect() const;
    // 计算托盘中某张牌的默认位置。
    // index: 牌在托盘中的序号。
    cocos2d::Vec2 getCustomTrayPosition(int index) const;
    // 创建牌面视图并放入托盘。
    void buildEditorDeckView();
    // 清空编辑器层与状态。
    void clearEditor();
    // 将指定牌提到最上层以便拖动。
    // cardIndex: 牌在 _cards 中的索引。
    void bringCardToFront(int cardIndex);
    // 隐藏保存对话框层。
    void hideSaveDialog();
    // 将当前布局保存到磁盘。
    // layoutName: 用户输入的布局名称。
    void saveLayout(const std::string& layoutName);
    // 根据牌索引序列生成布局元数据。
    // slots: 输出槽位列表；cardIndices: 排序后的牌索引。
    void rebuildLayoutMetadata(std::vector<SlotLayout>& slots,
                               const std::vector<int>& cardIndices) const;
    // 清理文件名（去掉非法字符、空白）。
    static std::string sanitizeLayoutFileName(const std::string& rawName);

    cocos2d::Node* _host; // 编辑器宿主，一般为 GameScene
    bool _active = false;
    cocos2d::LayerColor* _editorLayer = nullptr;     // 编辑层
    cocos2d::LayerColor* _saveDialogLayer = nullptr; // 保存对话框层
    cocos2d::ui::EditBox* _saveNameEditBox = nullptr;
    std::vector<PokerCard> _cards;                   // 编辑器里的整副牌
    std::vector<PokerCardView*> _cardViews;          // 对应牌面视图
    std::vector<cocos2d::Vec2> _trayPositions;       // 牌库托盘中的默认位置
    std::vector<bool> _cardInMainArea;               // 标记哪些牌已被拖入主编辑区
    int _dragCardIndex = -1;                         // 当前拖动的牌索引
    cocos2d::Vec2 _dragOffset;                       // 触点到牌中心的偏移
    int _zCounter = 100;                             // 用于把最近操作的牌提到最上层
    std::function<void(const std::string&)> _onStatus;
    std::function<void(const std::string&, const std::string&)> _onSaved;
};

#endif
