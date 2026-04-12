#include "PokerCardView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

// 静态成员定义
float PokerCardView::s_cardScale = 0.5f;
cocos2d::Size s_detectedCardSourceSize = cocos2d::Size::ZERO;
cocos2d::Size s_detectedBackgroundSize = cocos2d::Size::ZERO;

namespace
{
// 卡牌布局比例值（基于原始卡牌尺寸）
constexpr float kSuitTargetWidthRatio = 0.18f;
constexpr float kSmallRankTargetHeightRatio = 0.14f;
constexpr float kBigRankTargetHeightRatio = 0.40f;
constexpr float kCornerInsetXRatio = 0.10f;
constexpr float kCornerInsetYRatio = 0.06f;
}

// 创建单张牌视图。
PokerCardView* PokerCardView::create(const PokerCard& card, bool faceUp)
{
    auto* view = new (std::nothrow) PokerCardView();
    if (view != nullptr && view->initWithCard(card, faceUp))
    {
        view->autorelease();
        return view;
    }
    delete view;
    return nullptr;
}

// 初始化牌面节点并设置初始朝向。
bool PokerCardView::initWithCard(const PokerCard& card, bool faceUp)
{
    if (!Node::init())
    {
        return false;
    }

    _card = card;
    _faceUp = faceUp;
    _faceNode = nullptr;
    _backNode = nullptr;

    setContentSize(getCardSize());
    setAnchorPoint(Vec2(0.5f, 0.5f));

    buildCardFace();
    buildCardBack();

    _faceNode->setVisible(_faceUp);
    _backNode->setVisible(!_faceUp);

    return true;
}

// 获取当前展示的牌。
const PokerCard& PokerCardView::getCard() const
{
    return _card;
}

// 探测素材原始尺寸，失败时用配置兜底。
Size PokerCardView::getSourceCardSize()
{
    refreshCardMetrics();
    if (s_detectedCardSourceSize.width > 0.0f && s_detectedCardSourceSize.height > 0.0f)
    {
        return s_detectedCardSourceSize;
    }

    // 如果资源尺寸探测失败，则退回配置中的兜底值。
    auto& cfg = GlobalConfig::getInstance();
    return Size(cfg.getCardSourceWidth(), cfg.getCardSourceHeight());
}

// 获取运行时展示尺寸。
Size PokerCardView::getCardSize()
{
    const Size sourceSize = getSourceCardSize();
    return Size(sourceSize.width * s_cardScale, sourceSize.height * s_cardScale);
}

float PokerCardView::getCardWidth()
{
    return getCardSize().width;
}

float PokerCardView::getCardHeight()
{
    return getCardSize().height;
}

// 根据素材尺寸与背景宽度比例重新计算牌尺寸。
void PokerCardView::refreshCardMetrics()
{
    auto& cfg = GlobalConfig::getInstance();

    // 优先根据真实素材尺寸和背景尺寸推导缩放比例，减少硬编码尺寸。
    const Size detectedCardSize = detectTextureSize(cfg.getImage("cardFace"));
    if (detectedCardSize.width > 0.0f && detectedCardSize.height > 0.0f)
    {
        s_detectedCardSourceSize = detectedCardSize;
    }

    const Size detectedBackgroundSize = detectTextureSize(cfg.getImage("background"));
    if (detectedBackgroundSize.width > 0.0f && detectedBackgroundSize.height > 0.0f)
    {
        s_detectedBackgroundSize = detectedBackgroundSize;
    }

    if (s_detectedCardSourceSize.width <= 0.0f || s_detectedBackgroundSize.width <= 0.0f)
    {
        return;
    }

    const float targetCardWidth = s_detectedBackgroundSize.width * cfg.getCardWidthRatioToBackground();
    if (targetCardWidth > 0.0f)
    {
        s_cardScale = targetCardWidth / s_detectedCardSourceSize.width;
    }
}

// 从纹理缓存探测图片尺寸。
Size PokerCardView::detectTextureSize(const std::string& path)
{
    auto* cache = Director::getInstance()->getTextureCache();
    if (cache == nullptr)
    {
        return Size::ZERO;
    }

    auto* texture = cache->addImage(path);
    if (texture == nullptr)
    {
        return Size::ZERO;
    }

    return texture->getContentSize();
}

// 设置正反面可见性。
void PokerCardView::setFaceUp(bool faceUp)
{
    _faceUp = faceUp;
    if (_faceNode) _faceNode->setVisible(_faceUp);
    if (_backNode) _backNode->setVisible(!_faceUp);
}

bool PokerCardView::isFaceUp() const
{
    return _faceUp;
}

// 播放翻面动画，翻到正面。
void PokerCardView::flipToFaceUp(float duration)
{
    if (_faceUp) return;

    auto& cfg = GlobalConfig::getInstance();
    float dur = (duration < 0.0f) ? cfg.getFlipDuration() : duration;

    // 用 X 轴收缩到 0 再展开，模拟最轻量的翻牌效果。
    auto* shrink = ScaleTo::create(dur * 0.5f, 0.0f, getScaleY());
    auto* flip = CallFunc::create([this]() {
        setFaceUp(true);
    });
    auto* expand = ScaleTo::create(dur * 0.5f, 1.0f, getScaleY());
    auto* seq = Sequence::create(shrink, flip, expand, nullptr);
    runAction(seq);
}

// 切换高亮外观。
void PokerCardView::setHighlight(bool highlight)
{
    auto& theme = GlobalConfig::getInstance();
    setColor(highlight ? theme.getColor3B("cardHighlight") : Color3B::WHITE);
    setOpacity(255);
}

static void fitSpriteToSize(cocos2d::Sprite* sprite, const cocos2d::Size& targetSize)
{
    if (sprite == nullptr) return;
    const Size& contentSize = sprite->getContentSize();
    if (contentSize.width <= 0.0f || contentSize.height <= 0.0f) return;

    // 保持等比缩放，避免数字和花色资源被拉伸。
    float scaleX = targetSize.width / contentSize.width;
    float scaleY = targetSize.height / contentSize.height;
    float scale = std::min(scaleX, scaleY);
    sprite->setScale(scale);
}

// 构建正面节点，包含背景、花色与点数字体。
void PokerCardView::buildCardFace()
{
    auto& cfg = GlobalConfig::getInstance();
    const float cw = getCardWidth();
    const float ch = getCardHeight();

    _faceNode = Node::create();
    _faceNode->setAnchorPoint(Vec2(0.0f, 0.0f));
    _faceNode->setPosition(Vec2::ZERO);
    addChild(_faceNode, 1);

    auto* background = Sprite::create(cfg.getImage("cardFace"));
    if (background == nullptr)
    {
        background = Sprite::create();
        background->setTextureRect(Rect(0, 0, cw, ch));
        background->setColor(Color3B::WHITE);
    }
    else if (background->getContentSize().width > 0.0f && background->getContentSize().height > 0.0f)
    {
        background->setScaleX(cw / background->getContentSize().width);
        background->setScaleY(ch / background->getContentSize().height);
    }
    background->setAnchorPoint(Vec2(0.5f, 0.5f));
    background->setPosition(Vec2(cw * 0.5f, ch * 0.5f));
    _faceNode->addChild(background);

    const float insetX = cw * kCornerInsetXRatio;
    const float insetY = ch * kCornerInsetYRatio;

    // 当前牌面资源布局固定为：左上花色、右上小数字、中心大数字。
    const float suitTargetW = cw * kSuitTargetWidthRatio;
    {
        auto* suitSprite = Sprite::create(_card.suitTexturePath());
        if (suitSprite)
        {
            fitSpriteToSize(suitSprite, Size(suitTargetW, suitTargetW));
            suitSprite->setAnchorPoint(Vec2(0.0f, 1.0f));
            suitSprite->setPosition(Vec2(insetX, ch - insetY));
            _faceNode->addChild(suitSprite, 2);
        }
    }

    // --- 右上角：小数字 ---
    const float smallRankTargetH = ch * kSmallRankTargetHeightRatio;
    {
        auto* rankSprite = Sprite::create(_card.smallRankTexturePath());
        if (rankSprite)
        {
            fitSpriteToSize(rankSprite, Size(cw, smallRankTargetH));
            rankSprite->setAnchorPoint(Vec2(1.0f, 1.0f));
            rankSprite->setPosition(Vec2(cw - insetX, ch - insetY));
            _faceNode->addChild(rankSprite, 2);
        }
    }

    // --- 正中间：大数字 ---
    const float bigRankTargetH = ch * kBigRankTargetHeightRatio;
    {
        auto* rankSprite = Sprite::create(_card.bigRankTexturePath());
        if (rankSprite)
        {
            fitSpriteToSize(rankSprite, Size(cw, bigRankTargetH));
            rankSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
            rankSprite->setPosition(Vec2(cw * 0.5f, ch * 0.5f));
            _faceNode->addChild(rankSprite, 2);
        }
    }
}

// 构建背面节点。
void PokerCardView::buildCardBack()
{
    auto& cfg = GlobalConfig::getInstance();

    const float cw = getCardWidth();
    const float ch = getCardHeight();

    _backNode = Node::create();
    _backNode->setAnchorPoint(Vec2(0.0f, 0.0f));
    _backNode->setPosition(Vec2::ZERO);
    addChild(_backNode, 1);

    auto* back = Sprite::create(cfg.getImage("cardBack"));
    if (back == nullptr) return;

    if (back->getContentSize().width > 0.0f && back->getContentSize().height > 0.0f)
    {
        back->setScaleX(cw / back->getContentSize().width);
        back->setScaleY(ch / back->getContentSize().height);
    }
    back->setAnchorPoint(Vec2(0.5f, 0.5f));
    back->setPosition(Vec2(cw * 0.5f, ch * 0.5f));
    _backNode->addChild(back);
}
