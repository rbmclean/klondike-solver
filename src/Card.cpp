#include "Card.h"
#include <cassert>

Card::Rank Card::getRank() const {
	return rank;
}

Card::Suit Card::getSuit() const {
	return suit;
}

Card::Card(Rank r, Suit s) : rank(r), suit(s) {}

//this constructor shouldn't be used, as what would a default card be?
Card::Card() : rank(Rank::RankMax), suit(Suit::SuitMax) {}

bool Card::isSameColor(const Card &c) const {
	return suit % 2 == c.suit % 2;
}

bool Card::operator<(const Card &c) const {
	return suit * (Rank::RankMax + 1) + rank <
		c.suit * (Rank::RankMax + 1) + c.rank;
}

std::string Card::getString() const {
	std::string ret;
	switch(suit){
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
	switch(rank){
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
		default:
			ret += '?';
	}
	return ret;
}
