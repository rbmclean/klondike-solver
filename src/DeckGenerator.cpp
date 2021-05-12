#include "DeckGenerator.h"
#include <cstddef>
#include <algorithm>
#include <stdexcept>

//note: it's important the deck is initialized in order of decreasing priority
//otherwise permutations can easily be skipped
//if this constructor is changed for some reason, sort() can just be called at the end
DeckGenerator::DeckGenerator(bool random) : done(false), genRandom(random) {
	size_t i = 0;
	for(int suit = 0; suit <= Card::Suit::SuitMax; suit++)
		for(int rank = 0; rank <= Card::Rank::RankMax; rank++)
			prevDeck[i++] = Card(static_cast<Card::Rank>(rank), static_cast<Card::Suit>(suit));
	if(genRandom){
		std::random_device dev;
		randomGenerator = std::mt19937(dev());
		std::shuffle(prevDeck, prevDeck + (Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1), randomGenerator);
	}
}

DeckGenerator::DeckGenerator(std::string str) : done(false), genRandom(false) {
	if(str.length() != 2 * sizeof(prevDeck) / sizeof(prevDeck[0]))
		throw std::invalid_argument("DeckGenerator constructor");
	for(size_t i = 0; i < str.length() - 1; i += 2)
		prevDeck[i / 2] = Card(str.substr(i, 2));
}

//create the state before permuting, so that the initial state is tested at least once
GameState DeckGenerator::getStart(){
	GameState ret(prevDeck);
	if(genRandom)
		std::shuffle(prevDeck, prevDeck + (Card::Suit::SuitMax + 1) * (Card::Rank::RankMax + 1), randomGenerator);
	else
		permuteNext();
	return ret;
}

void DeckGenerator::permuteNext(){
	//process: find last index that has a card > than it following
	//swap that index with the lowest card following and greater than it
	//sort everything past the index

	//note there's a few special cases done for pruning

	size_t maxIndex = sizeof(prevDeck) / sizeof(prevDeck[0]) - 1;
	size_t maxCardIndex = maxIndex;
	for(ssize_t index = maxIndex - 1; index >= 0; index--){
		if(prevDeck[index] < prevDeck[maxCardIndex]){
			//we've found the right index (unless pruned)

			//two types of pruning are done here
			//First, only one suit needs to be tested in index 0
			//Second, only one suit needs to be tested if it's the first
			//of that color. This means when all preceding cards are one color
			//and "index" is a different color, "index" only needs to be one suit of that color
			//(all possibilities for the same color preceding still must be tested)
			size_t swapIndex = maxCardIndex;
			bool allPruned = true;
			for(size_t i = index + 1; i <= maxIndex; i++){
				if(prevDeck[index] < prevDeck[i] && (swapIndex == maxCardIndex || prevDeck[i] < prevDeck[swapIndex])){
					if(!index && prevDeck[i].getSuit())
						continue;
					if(!prevDeck[i].isSameColor(prevDeck[0])){
						bool prune = true;
						for(ssize_t j = 0; j < index; j++)
							if(prevDeck[j].isSameColor(prevDeck[i])){
								prune = false;
								break;
							}
						if(prune)
							continue;
					}
					swapIndex = i;
					allPruned = false;
				}
			}
			if(!allPruned){
				std::swap(prevDeck[index], prevDeck[swapIndex]);
				std::sort(prevDeck + index + 1, prevDeck + maxIndex + 1);
				return;
			}
		}
		maxCardIndex = index;
	}
	done = true;
}


bool DeckGenerator::donePermuting() const {
	return done;
}

std::string DeckGenerator::getString() const {
	std::string ret;
	for(const auto &i : prevDeck)
		ret += i.getString();
	return ret;
}
