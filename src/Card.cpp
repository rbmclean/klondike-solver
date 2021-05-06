#include "Card.h"
#include <cassert>
#include <stdexcept>

Card::Rank Card::getRank() const {
	return rank;
}

Card::Suit Card::getSuit() const {
	return suit;
}

Card::Card(Rank r, Suit s) : rank(r), suit(s) {}

Card::Card(std::string str){
	if(str.length() != 2)
		throw std::invalid_argument("Card constructor");
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
}

//this constructor shouldn't be used, as what would a default card be?
Card::Card() : rank(Rank::RankMax), suit(Suit::SuitMax) {}

bool Card::isSameColor(const Card &c) const {
	return suit % 2 == c.suit % 2;
}

bool Card::operator<(const Card &c) const {
	return suit * (Rank::RankMax + 1) + rank <
		c.suit * (Rank::RankMax + 1) + c.rank;
}

bool Card::canStartTableau() const {
	return rank == Rank::KING;
}

bool Card::canStartFoundation() const {
	return rank == Rank::ACE;
}

//c is the card this would be played on
bool Card::canPlayOnTableau(const Card &c) const {
	return !isSameColor(c) && rank == c.rank - 1;
}

bool Card::canPlayOnFoundation(const Card &c) const {
	return suit == c.suit && rank == c.rank + 1;
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
