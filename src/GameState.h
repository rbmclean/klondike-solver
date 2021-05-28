#pragma once
#include "Card.h"
#include <vector>

#define TABLEAU_STACKS 7
#define MOVED_TO_WASTE 3
#define LOG_EVERY_X_STATES 10000

class GameState {
private:
	//names taken from https://en.wikipedia.org/wiki/Klondike_(solitaire)
	//all vectors topmost card can be accessed with back()
	std::vector<Card> tableau[TABLEAU_STACKS];

	//using unsigned chars instead of size_t
	//reduced sizeof(GameState) by 56 bytes (~14% at the time)
	//since it's impossible for any of them to exceed ~30, I did it

	//minimum index of visible card
	unsigned char visibleIndex[TABLEAU_STACKS];
	unsigned char cardsOnWaste;
	std::vector<Card> stock;
	std::vector<Card> foundations[Card::Suit::SuitMax + 1];
	std::vector<Card> moveSequence;

	//used to make operator== return true more often,
	//when a state is functionally identical, though maybe some tableau stacks
	//are in a different spot
	//also used to autoplay moves that are guarenteed to never produce
	//an unsolvable state from a solvable one and vice versa
	void standardize();
public:
	GameState(Card const deck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)]);
	size_t getHash() const;
	std::vector<GameState> generateMoves() const;
	bool isTriviallySolvable() const;
	bool isDeadEnd() const;
	std::string getHowToSolve() const;
	bool operator==(const GameState& other) const;
	std::string getString() const;
};
