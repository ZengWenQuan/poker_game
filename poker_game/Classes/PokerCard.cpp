#include "PokerCard.h"
#include "config/GlobalConfig.h"

PokerCard::PokerCard(CardSuit suit, CardRank rank)
    : _suit(suit), _rank(rank)
{
}

// 是否属于红色花色。
bool PokerCard::isRedSuit() const
{
    // 资源命名和显示颜色都以“红/黑”二分，这里统一判断。
    return _suit == CardSuit::Heart || _suit == CardSuit::Diamond;
}

// 返回牌面字符，用于贴图拼装与 UI 显示。
std::string PokerCard::rankToken() const
{
    // 资源文件名使用牌面字符，不直接使用枚举整数。
    switch (_rank)
    {
    case CardRank::Ace:
        return "A";
    case CardRank::Jack:
        return "J";
    case CardRank::Queen:
        return "Q";
    case CardRank::King:
        return "K";
    case CardRank::Ten:
        return "10";
    default:
        return std::to_string(static_cast<int>(_rank));
    }
}

// 获取花色贴图路径。
std::string PokerCard::suitTexturePath() const
{
    auto& cfg = GlobalConfig::getInstance();
    switch (_suit)
    {
    case CardSuit::Spade:   return cfg.getImage("suitSpade");
    case CardSuit::Heart:   return cfg.getImage("suitHeart");
    case CardSuit::Club:    return cfg.getImage("suitClub");
    case CardSuit::Diamond: return cfg.getImage("suitDiamond");
    }
    return cfg.getImage("suitSpade");
}

// 获取大号点数字体贴图路径。
std::string PokerCard::bigRankTexturePath() const
{
    // 大数字、小数字都通过模板路径拼接，避免业务层硬编码资源路径。
    return GlobalConfig::getInstance().getImageFormatted("bigRankFormat", isRedSuit() ? "red" : "black", rankToken());
}

// 获取小号点数字体贴图路径。
std::string PokerCard::smallRankTexturePath() const
{
    return GlobalConfig::getInstance().getImageFormatted("smallRankFormat", isRedSuit() ? "red" : "black", rankToken());
}

// 返回牌点数的整数值。
int PokerCard::rankValue() const
{
    return static_cast<int>(_rank);
}

// 比较两张牌是否完全相同。
bool PokerCard::operator==(const PokerCard& other) const
{
    return _suit == other._suit && _rank == other._rank;
}

bool PokerCard::operator!=(const PokerCard& other) const
{
    return !(*this == other);
}
