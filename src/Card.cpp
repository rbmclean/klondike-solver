#include "Card.h"
#include <cassert>
#include <stdexcept>

const Card Card::FOUNDATION(Card::Rank::F, Card::SuitMax);
const Card Card::TABLEAU(Card::Rank::T, Card::SuitMax);

Card::Rank Card::getRank() const {
	return static_cast<Card::Rank>(rankSuit >> 2);
}

Card::Suit Card::getSuit() const {
	return static_cast<Card::Suit>(rankSuit & SUIT_MASK);
}

Card::Card(Rank r, Suit s){
	rankSuit = static_cast<unsigned char>(r);
	rankSuit <<= 2;
	rankSuit |= static_cast<unsigned char>(s);
}

Card::Card(std::string str){
	if(str.length() != 2)
		throw std::invalid_argument("Card constructor");
	Card::Suit suit;
	Card::Rank rank;
	switch(str[0]){
		case 'H':
			suit = Suit::HEARTS;
			break;
		case 'S':
			suit = Suit::SPADES;
			break;
		case 'C':
			suit = Suit::CLUBS;
			break;
		case 'D':
			suit = Suit::DIAMONDS;
			break;
		default:
			throw std::invalid_argument("Card constructor");
	}
	switch(str[1]){
		case 'A':
			rank = Rank::ACE;
			break;
		case '2':
			rank = Rank::TWO;
			break;
		case '3':
			rank = Rank::THREE;
			break;
		case '4':
			rank = Rank::FOUR;
			break;
		case '5':
			rank = Rank::FIVE;
			break;
		case '6':
			rank = Rank::SIX;
			break;
		case '7':
			rank = Rank::SEVEN;
			break;
		case '8':
			rank = Rank::EIGHT;
			break;
		case '9':
			rank = Rank::NINE;
			break;
		case '0':
			rank = Rank::TEN;
			break;
		case 'J':
			rank = Rank::JACK;
			break;
		case 'Q':
			rank = Rank::QUEEN;
			break;
		case 'K':
			rank = Rank::KING;
			break;
		default:
			throw std::invalid_argument("Card constructor");
	}
	rankSuit = static_cast<unsigned char>(rank);
	rankSuit <<= 2;
	rankSuit |= static_cast<unsigned char>(suit);
}

//this constructor shouldn't be used, as what would a default card be?
Card::Card() : rankSuit(0) {}

bool Card::isSameColor(const Card &c) const {
	return (rankSuit & 1) == (c.rankSuit & 1);
}

bool Card::operator<(const Card &c) const {
	return getSuit() * (Rank::RankMax + 1) + getRank() <
		c.getSuit() * (Rank::RankMax + 1) + c.getRank();
}

bool Card::operator==(const Card &c) const {
	return rankSuit == c.rankSuit;
}

bool Card::canStartTableau() const {
	return getRank() == Rank::KING;
}

bool Card::canStartFoundation() const {
	return getRank() == Rank::ACE;
}

//c is the card this would be played on
bool Card::canPlayOnTableau(const Card &c) const {
	return !isSameColor(c) && getRank() == c.getRank() - 1;
}

bool Card::canPlayOnFoundation(const Card &c) const {
	return getSuit() == c.getSuit() && getRank() == c.getRank() + 1;
}

std::string Card::getString() const {
	std::string ret;
	switch(getSuit()){
		case Suit::HEARTS:
			ret += 'H';
			break;
		case Suit::SPADES:
			ret += 'S';
			break;
		case Suit::CLUBS:
			ret += 'C';
			break;
		case Suit::DIAMONDS:
			ret += 'D';
			break;
		default:
			ret += '?';
	}
	switch(getRank()){
		case Rank::ACE:
			ret += 'A';
			break;
		case Rank::TWO:
			ret += '2';
			break;
		case Rank::THREE:
			ret += '3';
			break;
		case Rank::FOUR:
			ret += '4';
			break;
		case Rank::FIVE:
			ret += '5';
			break;
		case Rank::SIX:
			ret += '6';
			break;
		case Rank::SEVEN:
			ret += '7';
			break;
		case Rank::EIGHT:
			ret += '8';
			break;
		case Rank::NINE:
			ret += '9';
			break;
		case Rank::TEN:
			ret += '0';//I wanted a single char
			break;
		case Rank::JACK:
			ret += 'J';
			break;
		case Rank::QUEEN:
			ret += 'Q';
			break;
		case Rank::KING:
			ret += 'K';
			break;
		case Rank::F:
			ret = 'F';
			break;
		case Rank::T:
			ret = 'T';
			break;
		default:
			ret += '?';
	}
	return ret;
}
