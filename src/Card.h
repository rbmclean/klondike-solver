#pragma once

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
	bool isSameColor(const Card &c) const;
};
