#pragma once
#include <string>
#define SUIT_MASK 0b11
class Card {

public:

	enum Rank : unsigned char {
		ACE,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		TEN,
		JACK,
		QUEEN,
		KING,
		RankMax = KING,
		F = 0b111110,
		T = 0b111111
	};

	// This order is used to allow color comparison by modulus
	// (see isSameColor for example)
	enum Suit : unsigned char {
		CLUBS,
		HEARTS,
		SPADES,
		DIAMONDS,
		SuitMax = DIAMONDS
	};

private:
	//uses rightmost two bits for suit, everything else is rank.
	unsigned char rankSuit;

public:
	Rank getRank() const;
	Suit getSuit() const;
	Card(Rank r, Suit s);
	Card(std::string str);
	Card();
	bool isSameColor(const Card &c) const;
	bool operator<(const Card &c) const;
	bool operator==(const Card &c) const;
	bool canStartTableau() const;
	bool canStartFoundation() const;
	bool canPlayOnTableau(const Card &c) const;
	bool canPlayOnFoundation(const Card &c) const;
	std::string getString() const;

	static const Card FOUNDATION;
	static const Card TABLEAU;
};
