#pragma once

#include "Card.h"
#include "GameState.h"

class DeckGenerator {

private:
	Card prevDeck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)];
	bool done;
public:
	DeckGenerator();

	//these two are split up for easier initialization
	//(though initialization to a certain state can probably be improved)
	GameState getStart();
	void permuteNext();

	bool donePermuting() const;
	std::string getString() const;
};
