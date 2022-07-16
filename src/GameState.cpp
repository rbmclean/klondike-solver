#include "GameState.h"
#include "logger.h"
#include <unordered_set>
#include <iostream>

namespace std {
    template<> struct hash<GameState>{
		//TODO: revisit this. It's simple and time efficient right now,
		//but probably could be lengthed and still take a reasonable amount of time
		//and reduce the number of collisions
        std::size_t operator()(GameState const& state) const noexcept {
            return state.getHash();
        }
    };
}

size_t GameState::getHash() const {
    size_t ret = 0;
    size_t mult;
    for(mult = 0; mult < TABLEAU_STACKS; mult++)
        ret += (mult + 1) * tableau[mult].size() * (visibleIndex[mult] + 1);
    ret += (mult++ + 1) * stock.size();
    
    //did basic benchmarking with these removed - slightly faster
    //perf indicated this function was very bad for performance, so I'll take everything I can get
    //ret += cardsOnWaste * mult++; //not using - this will be 0 very often
    /*for(size_t i = 0; i <= Card::Suit::SuitMax; i++)
        ret += (mult++) * foundations[i].size();*/ //between checking the tableau and stock, this doesn't really give any more differentiating info
    
    
    log(3, "GameState hashed to ");
    log(3, std::to_string(ret));
    log(3, "\n");
    return ret;
}

void GameState::standardize(){
    //play moves first so we don't have to sort more than once

    //tableau moves to foundation
    unsigned char minFoundationRank = Card::Rank::RankMax;
    for(unsigned char i = 0; i <= Card::Suit::SuitMax; i++)
        if(foundations[i].size())
            minFoundationRank = std::min(minFoundationRank, static_cast<unsigned char>(foundations[i].back().getRank()));
        else{
            minFoundationRank = Card::Rank::T;//just a flag
            break;
        }
    Card::Rank playIfBelow = (minFoundationRank == Card::Rank::T) ?
            Card::Rank::THREE : static_cast<Card::Rank>(minFoundationRank + 3);
    for(size_t i = 0; i < TABLEAU_STACKS; i++)
        if(tableau[i].size()){
            if(tableau[i].back().getRank() >= playIfBelow)
                continue;
            size_t foundationIndex = static_cast<size_t>(tableau[i].back().getSuit());
            if((!foundations[foundationIndex].size() && tableau[i].back().canStartFoundation()) ||
                    (foundations[foundationIndex].size() && tableau[i].back().canPlayOnFoundation(foundations[foundationIndex].back()))){
                moveSequence.push_back(tableau[i].back());
                moveSequence.push_back(foundations[foundationIndex].size() ? foundations[foundationIndex].back() : Card::FOUNDATION);
                if(tableau[i].size() > 1 && visibleIndex[i] + 1 == static_cast<unsigned char>(tableau[i].size()))
                    visibleIndex[i]--;
                foundations[foundationIndex].push_back(tableau[i].back());
                tableau[i].pop_back();
                standardize();
                return;//tail recursion
            }
        }
	
	//handle stock/waste
	if(!(cardsOnWaste % MOVED_TO_WASTE) || cardsOnWaste == stock.size()){
		cardsOnWaste = 0;
		//if playing the last card in the stock won't change what moves are available for other stock cards,
		//and we can play it to the foundation without ever wanting to undo it,
		//we should play it
		//ex: 0 cards on waste, last card is CA. It should always be played
		size_t foundationIndex = static_cast<size_t>(stock[0].getSuit());
		if(stock.size() && stock[0].getRank() < playIfBelow && (!foundations[foundationIndex].size() && stock[0].canStartFoundation()) ||
				(foundations[foundationIndex].size() && stock[0].canPlayOnFoundation(foundations[foundationIndex].back()))){
			moveSequence.push_back(stock[0]);
            moveSequence.push_back(foundations[foundationIndex].size() ? foundations[foundationIndex].back() : Card::FOUNDATION);
            foundations[foundationIndex].push_back(stock[0]);
            stock.erase(stock.begin());
            standardize();
            return;//tail recursion
		}
	}

    //sort tableau
    //note: tableau is quite likely to already be sorted, or at least close
    //there's also only TABLEAU_STACKS elements to sort
    //so, I implemented insertion sort
    for(size_t i = 1; i < TABLEAU_STACKS; i++)
        for(size_t j = i; j > 0 && tableau[j - 1] > tableau[j]; j--)
            std::swap(tableau[j], tableau[j - 1]), std::swap(visibleIndex[j], visibleIndex[j - 1]);
}

GameState::GameState(Card const deck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)]) : cardsOnWaste(0) {
	size_t deckIndex = 0;
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		//ith stack gets i+1 cards
		tableau[i] = std::vector<Card>(deck + deckIndex, deck + deckIndex + i + 1);
		deckIndex += i + 1;
		visibleIndex[i] = i;
	}
	stock = std::vector<Card>(deck + deckIndex, deck + (Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1));
    standardize();
}

//TODO: filter out more worthless moves
//TODO: better prioritization of GameStates to check
std::vector<GameState> GameState::generateMoves() const {
	std::vector<GameState> moves;

	//1. play a stock card
    std::unordered_set<size_t> stockIndices;

    for(ssize_t i = static_cast<ssize_t>(stock.size()) - MOVED_TO_WASTE; i >= 0; i -= MOVED_TO_WASTE)
        stockIndices.insert(static_cast<size_t>(i)), log(2, stock[i].getString());
    for(ssize_t i = static_cast<ssize_t>(stock.size() - cardsOnWaste) - MOVED_TO_WASTE; i >= 0; i -= MOVED_TO_WASTE)
        stockIndices.insert(static_cast<size_t>(i)), log(2, stock[i].getString());
    if(stock.size())
        stockIndices.insert(0), log(2, stock[0].getString());
    log(2, "\n");
	for(size_t i : stockIndices){
		//1a. play it on a foundation
		size_t foundationIndex = static_cast<size_t>(stock[i].getSuit());
		if((!foundations[foundationIndex].size() && stock[i].canStartFoundation()) ||
				(foundations[foundationIndex].size() && stock[i].canPlayOnFoundation(foundations[foundationIndex].back()))){
			GameState tmp(*this);
			tmp.stock.erase(tmp.stock.begin() + i);
            tmp.cardsOnWaste = stock.size() - i - 1;
			tmp.foundations[foundationIndex].push_back(stock[i]);
            tmp.moveSequence.push_back(stock[i]);
            tmp.moveSequence.push_back(foundations[foundationIndex].size() ? foundations[foundationIndex].back() : Card::FOUNDATION);
            tmp.standardize();
			moves.push_back(tmp);
		}

		//1b. play it on the tableau
		for(size_t j = 0; j < TABLEAU_STACKS; j++){
			if(tableau[j].size() && stock[i].canPlayOnTableau(tableau[j].back())){
				GameState tmp(*this);
				tmp.stock.erase(tmp.stock.begin() + i);
                tmp.cardsOnWaste = stock.size() - i - 1;
				tmp.tableau[j].push_back(stock[i]);
                tmp.moveSequence.push_back(stock[i]);
                tmp.moveSequence.push_back(tableau[j].back());
                tmp.standardize();
				moves.push_back(tmp);
			}else if(!tableau[j].size() && stock[i].canStartTableau()){
				GameState tmp(*this);
				tmp.stock.erase(tmp.stock.begin() + i);
                tmp.cardsOnWaste = stock.size() - i - 1;
				tmp.tableau[j].push_back(stock[i]);
                tmp.moveSequence.push_back(stock[i]);
                tmp.moveSequence.push_back(Card::TABLEAU);
                tmp.standardize();
				moves.push_back(tmp);
			}
		}
	}

	//2. move tableau card(s)
	unsigned char tableausEmptied = 0;
	for(size_t i = 0; i < TABLEAU_STACKS; i++)
		if(!visibleIndex[i] && ((tableau[i].size() && tableau[i][0].getRank() == Card::Rank::KING) || !tableau[i].size()))
			tableausEmptied++;
	
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		for(size_t j = visibleIndex[i]; j < tableau[i].size(); j++){
			//2a. move card(s) to other tableau stack
			//pointless cases: 
			//moving a King from an empty space to another empty space
			//moving the bottom card from a tableau with 4 tableaus empty or starting with a visible king
			
			if(!(!visibleIndex[i] && !j && tableau[i][j].canStartTableau())){
				for(size_t k = 0; k < TABLEAU_STACKS; k++){
					if(k == i)
						continue;
					if(tableau[k].size() && tableau[i][j].canPlayOnTableau(tableau[k].back()) && !(tableausEmptied >= Card::Suit::SuitMax + 1 && !j)){
						GameState tmp(*this);
						tmp.tableau[i].erase(tmp.tableau[i].begin() + j, tmp.tableau[i].end());
						for(size_t l = j; l < tableau[i].size(); l++)
							tmp.tableau[k].push_back(tableau[i][l]);
						if(j == visibleIndex[i] && j)
							tmp.visibleIndex[i]--;
                        tmp.moveSequence.push_back(tableau[i][j]);
                        tmp.moveSequence.push_back(tableau[k].back());
                        tmp.standardize();
						moves.push_back(tmp);
					}else if(!tableau[k].size() && tableau[i][j].canStartTableau()){
						GameState tmp(*this);
						tmp.tableau[i].erase(tmp.tableau[i].begin() + j, tmp.tableau[i].end());
						for(size_t l = j; l < tableau[i].size(); l++)
							tmp.tableau[k].push_back(tableau[i][l]);
						if(j == visibleIndex[i] && j)
							tmp.visibleIndex[i]--;
                        tmp.moveSequence.push_back(tableau[i][j]);
                        tmp.moveSequence.push_back(Card::TABLEAU);
                        tmp.standardize();
						moves.push_back(tmp);
					}
				}
			}
		}
		//2b. move topmost card to foundation
		//pointless for a king if there's room for all kings on the tableau - no card would fill it's space except itself
		if(tableau[i].size()){
			size_t foundationIndex = static_cast<size_t>(tableau[i].back().getSuit());
			if((!foundations[foundationIndex].size() && tableau[i].back().canStartFoundation()) ||
					(foundations[foundationIndex].size() && tableau[i].back().canPlayOnFoundation(foundations[foundationIndex].back()) && 
					(tableau[i].back().getRank() != Card::Rank::KING || tableausEmptied < Card::Suit::SuitMax + 1))){
				GameState tmp(*this);
				tmp.tableau[i].pop_back();
				tmp.foundations[foundationIndex].push_back(tableau[i].back());
				if(tableau[i].size() > 1 && visibleIndex[i] + 1 == static_cast<unsigned char>(tableau[i].size()))
					tmp.visibleIndex[i]--;
                tmp.moveSequence.push_back(tableau[i].back());
                tmp.moveSequence.push_back(foundations[foundationIndex].size() ? foundations[foundationIndex].back() : Card::FOUNDATION);
                tmp.standardize();
				moves.push_back(tmp);
			}
		}
	}

	//3. move card from foundation to tableau
	//anything within two of the lowest stack won't need to move ever
	size_t minFoundation = foundations[0].size();
	for(size_t i = 1; i <= Card::Suit::SuitMax; i++)
		minFoundation = std::min(minFoundation, foundations[i].size());
	for(size_t i = 0; i <= Card::Suit::SuitMax; i++){
		if(foundations[i].size() <= minFoundation + 2)
			continue;
		for(size_t j = 0; j < TABLEAU_STACKS; j++)
			if(tableau[j].size() && foundations[i].back().canPlayOnTableau(tableau[j].back())){
				GameState tmp(*this);
				tmp.foundations[i].pop_back();
				tmp.tableau[j].push_back(foundations[i].back());
                tmp.moveSequence.push_back(foundations[i].back());
                tmp.moveSequence.push_back(tableau[j].back());
                tmp.standardize();
				moves.push_back(tmp);
			}else if(!tableau[j].size() && foundations[i].back().canStartTableau()){
				//TODO: is there actually a case where moving a king off the foundation makes sense?
				//Not sure for now, so it's written.
				//Probably wouldn't prune much anyway.
				GameState tmp(*this);
				tmp.foundations[i].pop_back();
				tmp.tableau[j].push_back(foundations[i].back());
                tmp.moveSequence.push_back(foundations[i].back());
                tmp.moveSequence.push_back(Card::TABLEAU);
                tmp.standardize();
				moves.push_back(tmp);
			}
	}
    log(2, "Generated ");
    log(2, std::to_string(moves.size()));
    log(2, " moves from state\n");
	return moves;
}

bool GameState::isTriviallySolvable() const {
	//TODO: determine if these restrictions can be loosened
	//for now, you must be able to access every card (all visible) to be solvable
	if(MOVED_TO_WASTE != 1 && (stock.size() > 2 || (stock.size() == 2 && cardsOnWaste != 1)))
		return false;
	for(size_t i = 0; i < TABLEAU_STACKS; i++)
		if(tableau[i].size() && visibleIndex[i])
			return false;
	return true;
}

//note: currently no generated move will change this from false to true
//this means it only needs to be called for the first GameState generated from
//the DeckGenerator
bool GameState::isDeadEnd() const {
	//below test is from http://web.engr.oregonstate.edu/%7Eafern/papers/solitaire.pdf (page 7)
	//TODO: possibly implement the second case mentioned
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		//card must be blocking both cards it could be played on and one of same suit and lesser rank
		//that's where the magic number 3 comes from (3 cards must be covered by each card checked)
		for(size_t j = visibleIndex[i]; j >= 3; j--){
			bool blockingLowerRank = false;
			unsigned char blockedPlayableCards = 0;
			for(size_t k = 0; k < j; k++)
				if(tableau[i][j].canPlayOnTableau(tableau[i][k]))
					blockedPlayableCards++;
				else if(tableau[i][j].getSuit() == tableau[i][k].getSuit() && tableau[i][j].getRank() > tableau[i][k].getRank())
					blockingLowerRank = true;
			if(blockingLowerRank && blockedPlayableCards >= 2){
                log(1, "State is a dead end\n");
				return true;
            }
		}
	}
	return false;
}

//TODO: swap to using a priority queue instead of vector?
std::string GameState::getHowToSolve() const {
	//move inside of loop if isDeadEnd() can change
	//(it currently can't)
	if(isDeadEnd())
		return "";
	std::vector<GameState> uncheckedStates;
	std::unordered_set<GameState> seenStates;
	
	//don't need to save this state - there's no move we can do that would bring us back here
	//seenStates.insert(*this);
	
	uncheckedStates.reserve(600);//haven't seen this cross 600 yet - passes 500 on rare occasion
	seenStates.reserve(10000);//harder to say here - I've seen a case use >2GB RAM with >3M states
							  //this at least will save some time resizing in the beginning
	
	uncheckedStates.push_back(*this);
    size_t logAt = LOG_EVERY_X_STATES - 1;
	while(uncheckedStates.size()){
        if(seenStates.size() >= logAt){
            log(1, std::to_string(seenStates.size() + 1) + " states seen so far.\n");
            log(1, std::to_string(uncheckedStates.size()) + " unchecked states currently.\n");
            logAt += LOG_EVERY_X_STATES;
        }
		std::vector<GameState> possibilities = uncheckedStates.back().generateMoves();
        if(wouldLog(3)){
            log(3, "Checking state\n");
            log(3, uncheckedStates.back().getString());
            log(3, "Produced states:\n");
        }
		uncheckedStates.pop_back();
		for(const auto &i : possibilities){
            if(wouldLog(3))
                log(3, i.getString());
			if(i.isTriviallySolvable()){
                log(1, "Solvability check ended with ");
                log(1, std::to_string(seenStates.size()));
                log(1, " states seen.\n");
                std::string ret;
				for(const auto &i : i.moveSequence)
                    ret += i.getString();
                return ret;
            }
			if(seenStates.insert(i).second)
				uncheckedStates.push_back(i);
		}
	}
    log(1, "Solvability check ended with ");
    log(1, std::to_string(seenStates.size()));
    log(1, " states seen.\n");
	return "";
}

bool GameState::operator==(const GameState& other) const {
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		if(tableau[i] != other.tableau[i])
			return false;
		if(visibleIndex[i] != other.visibleIndex[i])
			return false;
	}
	if(stock != other.stock)
		return false;
	if(cardsOnWaste != other.cardsOnWaste)
        return false;
	for(size_t i = 0; i <= Card::Suit::SuitMax; i++)
		if(foundations[i] != other.foundations[i])
			return false;
	return true;
}

std::string GameState::getString() const {
	std::string ret = "Stock (";
    ret += std::to_string(cardsOnWaste);
    ret += " on waste):\n";
	for(ssize_t i = static_cast<ssize_t>(stock.size()) - 1; i >= 0; i--){
		//print backwards so that the top is printed at the top
		ret += stock[i].getString();
		ret += '\n';
	}

	ret += "Foundations:\n";
	size_t maxFoundation = foundations[0].size();
	for(size_t i = 1; i <= Card::Suit::SuitMax; i++)
		maxFoundation = std::max(maxFoundation, foundations[i].size());
	for(ssize_t j = static_cast<ssize_t>(maxFoundation); j >= 0; j--){
		for(size_t i = 0; i <= Card::Suit::SuitMax; i++){
			if(static_cast<size_t>(j) >= foundations[i].size())
				ret += "  ";
			else
				ret += foundations[i][j].getString();
			ret += '\t';
		}
		ret += '\n';
	}

	ret += "Tableau:\n";
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		ret += std::to_string(visibleIndex[i]);
		ret += '\t';
	}
	ret += '\n';
	size_t maxTableau = tableau[0].size();
	for(size_t i = 1; i < TABLEAU_STACKS; i++)
		maxTableau = std::max(maxTableau, tableau[i].size());
	for(ssize_t j = static_cast<ssize_t>(maxTableau); j >= 0; j--){
		for(size_t i = 0; i < TABLEAU_STACKS; i++){
			if(static_cast<size_t>(j) >= tableau[i].size())
				ret += "  ";
			else
				ret += tableau[i][j].getString();
			ret += '\t';
		}
		ret += '\n';
	}
	return ret;
}
