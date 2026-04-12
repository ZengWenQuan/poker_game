#include "PokerCard.h"
#include "config/GlobalConfig.h"

PokerCard::PokerCard(CardSuit suit, CardRank rank)
    : _suit(suit), _rank(rank)
{
}

bool PokerCard::isRedSuit() const
{
    return _suit == CardSuit::Heart || _suit == CardSuit::Diamond;
}

std::string PokerCard::rankToken() const
{
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

std::string PokerCard::bigRankTexturePath() const
{
    return GlobalConfig::getInstance().getImageFormatted("bigRankFormat", isRedSuit() ? "red" : "black", rankToken());
}

std::string PokerCard::smallRankTexturePath() const
{
    return GlobalConfig::getInstance().getImageFormatted("smallRankFormat", isRedSuit() ? "red" : "black", rankToken());
}

int PokerCard::rankValue() const
{
    return static_cast<int>(_rank);
}

bool PokerCard::operator==(const PokerCard& other) const
{
    return _suit == other._suit && _rank == other._rank;
}

bool PokerCard::operator!=(const PokerCard& other) const
{
    return !(*this == other);
}
