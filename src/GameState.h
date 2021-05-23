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
	//minimum index of visible card
	size_t visibleIndex[TABLEAU_STACKS];
	std::vector<Card> stock;
	size_t cardsOnWaste;
	std::vector<Card> foundations[Card::Suit::SuitMax + 1];
	//<cardMoved>(<cardMovedOn>|"T"|"F")*
	//"T" for tableau, "F" for foundation
	std::string moveSequence;
public:
	GameState();
	GameState(Card const deck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)]);
	size_t getHash() const;
	std::vector<GameState> generateMoves() const;
	bool isTriviallySolvable() const;
	bool isDeadEnd() const;
	std::string getHowToSolve() const;
	bool operator==(const GameState& other) const;
	std::string getString() const;
};
