#pragma once
#include <string>
class Card {

public:

	enum Rank {
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
		RankMax = KING
	};

	// This order is used to allow color comparison by modulus
	// (see isSameColor for example)
	enum Suit {
		CLUBS,
		HEARTS,
		SPADES,
		DIAMONDS,
		SuitMax = DIAMONDS
	};

private:
	enum Rank rank;
	enum Suit suit;

public:
	Rank getRank() const;
	Suit getSuit() const;
	Card(Rank r, Suit s);
	Card(std::string str);
	Card();
	bool isSameColor(const Card &c) const;
	bool operator<(const Card &c) const;
	bool canStartTableau() const;
	bool canStartFoundation() const;
	bool canPlayOnTableau(const Card &c) const;
	bool canPlayOnFoundation(const Card &c) const;
	std::string getString() const;
};
