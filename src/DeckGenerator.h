#pragma once

#include <random>
#include "Card.h"
#include "GameState.h"

class DeckGenerator {

private:
	Card prevDeck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)];
	std::mt19937 randomGenerator;
	bool done;
	bool genRandom;
public:
	DeckGenerator(bool random = true);
	//note: no checks are done to see if this permuatation would have been generated naturally
	DeckGenerator(std::string str);

	//these two are split up for easier initialization
	//(though initialization to a certain state can probably be improved)
	GameState getStart();
	void permuteNext();

	bool donePermuting() const;
	std::string getString() const;
};
