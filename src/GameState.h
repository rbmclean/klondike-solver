#pragma once
#include "Card.h"
#include <vector>

#define TABLEAU_STACKS 7
#define MOVED_TO_WASTE 3

class GameState {
private:
	//names taken from https://en.wikipedia.org/wiki/Klondike_(solitaire)
	//all vectors topmost card can be accessed with back()
	std::vector<Card> tableau[TABLEAU_STACKS];
	//minimum index of visible card
	size_t visibleIndex[TABLEAU_STACKS];
	std::vector<Card> stock;
	std::vector<Card> waste;
	std::vector<Card> foundations[Card::Suit::SuitMax + 1];
public:
	GameState();
	GameState(Card const deck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)]);
	std::vector<GameState> generateMoves() const;
	bool solvingIsTrivial() const;
	std::string getString() const;
};