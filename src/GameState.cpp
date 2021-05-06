#include "GameState.h"
#include <ostream>

//this may be useful for creating new states after dealing
//not sure yet
GameState::GameState(){}

GameState::GameState(Card const deck[(Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1)]){
	size_t deckIndex = 0;
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		//ith stack gets i+1 cards
		tableau[i] = std::vector<Card>(deck + deckIndex, deck + deckIndex + i + 1);
		deckIndex += i + 1;
		visibleIndex[i] = i;
	}
	stock = std::vector<Card>(deck + deckIndex, deck + (Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1));
}

//TODO: filter out more worthless moves
//TODO: better prioritization of GameStates to check
std::vector<GameState> GameState::generateMoves() const {
	std::vector<GameState> moves;

	//1. flip waste back to stock
	if(!stock.size() && waste.size() > MOVED_TO_WASTE){
		GameState tmp(*this);
		tmp.waste.clear();
		tmp.stock = std::vector<Card>(waste.rbegin(), waste.rend());
		moves.push_back(tmp);
	}

	//2. flip the next card(s) onto the waste
	if(stock.size()){
		GameState tmp(*this);
		for(size_t i = 0; i < MOVED_TO_WASTE && tmp.stock.size(); i++){
			tmp.waste.push_back(tmp.stock.back());
			tmp.stock.pop_back();
		}
		moves.push_back(tmp);
	}

	//3. play topmost waste card
	if(waste.size()){
		//3a. play it on a foundation
		size_t foundationIndex = static_cast<size_t>(waste.back().getSuit());
		if((!foundations[foundationIndex].size() && waste.back().canStartFoundation()) ||
				(foundations[foundationIndex].size() && waste.back().canPlayOnFoundation(foundations[foundationIndex].back()))){
			GameState tmp(*this);
			tmp.waste.pop_back();
			tmp.foundations[foundationIndex].push_back(waste.back());
			moves.push_back(tmp);
		}

		//3b. play it on the tableau
		for(size_t i = 0; i < TABLEAU_STACKS; i++){
			if(tableau[i].size() && waste.back().canPlayOnTableau(tableau[i].back())){
				GameState tmp(*this);
				tmp.waste.pop_back();
				tmp.tableau[i].push_back(waste.back());
				moves.push_back(tmp);
			}else if(!tableau[i].size() && waste.back().canStartTableau()){
				GameState tmp(*this);
				tmp.waste.pop_back();
				tmp.tableau[i].push_back(waste.back());
				moves.push_back(tmp);
			}
		}
	}

	//4. move tableau card(s)
	for(size_t i = 0; i < TABLEAU_STACKS; i++){
		for(size_t j = visibleIndex[i]; j < tableau[i].size(); j++){
			//4a. move card(s) to other tableau stack
			//only pointless case I can think of is moving a King
			//from an empty space to another empty space
			if(!(!visibleIndex[i] && !j && tableau[i][j].canStartTableau())){
				for(size_t k = 0; k < TABLEAU_STACKS; k++){
					if(k == i)
						continue;
					if(tableau[k].size() && tableau[i][j].canPlayOnTableau(tableau[k].back())){
						GameState tmp(*this);
						tmp.tableau[i].erase(tmp.tableau[i].begin() + j, tmp.tableau[i].end());
						for(size_t l = j; l < tableau[i].size(); l++)
							tmp.tableau[k].push_back(tableau[i][l]);
						if(j == visibleIndex[i] && j)
							tmp.visibleIndex[i]--;
						moves.push_back(tmp);
					}else if(!tableau[k].size() && tableau[i][j].canStartTableau()){
						GameState tmp(*this);
						tmp.tableau[i].erase(tmp.tableau[i].begin() + j, tmp.tableau[i].end());
						for(size_t l = j; l < tableau[i].size(); l++)
							tmp.tableau[k].push_back(tableau[i][l]);
						if(j == visibleIndex[i] && j)
							tmp.visibleIndex[i]--;
						moves.push_back(tmp);
					}
				}
			}
		}
		//4b. move topmost card to foundation
		if(tableau[i].size()){
			size_t foundationIndex = static_cast<size_t>(tableau[i].back().getSuit());
			if((!foundations[foundationIndex].size() && tableau[i].back().canStartFoundation()) ||
					(foundations[foundationIndex].size() && tableau[i].back().canPlayOnFoundation(foundations[foundationIndex].back()))){
				GameState tmp(*this);
				tmp.tableau[i].pop_back();
				tmp.foundations[foundationIndex].push_back(tableau[i].back());
				if(tableau[i].size() > 1 && visibleIndex[i] + 1 == tableau[i].size())
					tmp.visibleIndex[i]--;
				moves.push_back(tmp);
			}
		}
	}

	//5. move card from foundation to tableau
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
				moves.push_back(tmp);
			}else if(!tableau[j].size() && foundations[i].back().canStartTableau()){
				//TODO: is there actually a case where moving a king off the foundation makes sense?
				//Not sure for now, so it's written.
				//Probably wouldn't prune much anyway.
				GameState tmp(*this);
				tmp.foundations[i].pop_back();
				tmp.tableau[j].push_back(foundations[i].back());
				moves.push_back(tmp);
			}
	}
	return moves;
}

bool GameState::solvingIsTrivial() const {
	//TODO: determine if these restrictions can be loosened
	//for now, if there's <2 cards between the stock and waste
	//and all tableau cards are visible, it can be solved
	if(stock.size() + waste.size() >= 2)
		return false;
	for(size_t i = 0; i < TABLEAU_STACKS; i++)
		if(tableau[i].size() && visibleIndex[i])
			return false;
	return true;
}

std::string GameState::getString() const {
	std::string ret = "Stock:\tWaste:\n";
	for(ssize_t i = static_cast<ssize_t>(std::max(stock.size(), waste.size())); i >= 0; i--){
		//print backwards so that the top is printed at the top
		if(static_cast<size_t>(i) >= stock.size())
			ret += "  ";
		else
			ret += stock[i].getString();
		ret += '\t';
		if(static_cast<size_t>(i) >= waste.size())
			ret += "  ";
		else
			ret += waste[i].getString();
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
