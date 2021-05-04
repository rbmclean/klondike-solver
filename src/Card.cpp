#include "Card.h"
#include <cassert>

Card::Rank Card::getRank() const {
	return rank;
}

Card::Suit Card::getSuit() const {
	return suit;
}

Card::Card(Rank r, Suit s) : rank(r), suit(s) {}

bool Card::isSameColor(const Card &c) const {
	return suit % 2 == c.suit % 2;
}
