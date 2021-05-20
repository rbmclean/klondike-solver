#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "DeckGenerator.h"
#include "logger.h"

// Parses command line arguments and calls the corresponding function
int main(int argc, char* argv[]){
	char* singleDeck = nullptr;
	char* continueDeck = nullptr;
	uint64_t decks = 0;
	uint32_t threads = 1;
	bool random = false;
	for(int i = 1; i < argc; i++){
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			std::cout << "Options:" << std::endl;
			std::cout << "-h or --help:\tPrint this message" << std::endl;
			std::cout << "-1 [DECK]\tDetermine whether DECK is solvable and exit" << std::endl;
			std::cout << "-c [DECK]\tContinue solving decks in order (see below) from DECK" << std::endl;
			std::cout << "-n [UINT64]\tSolve UINT64 decks, unless overridden. Use 0 to run until terminated or out of decks." << std::endl;
			std::cout << "-r or -o\tSolve random decks or ordered decks, respectively" << std::endl;
			std::cout << "-t [UINT32]\tUse UINT32 threads (Unimplemented)" << std::endl;
			std::cout << "-v [0..3]\tSet verbosity level" << std::endl;

			std::cout << std::endl;

			std::cout << "Default arguments are:" << std::endl;
			std::cout << argv[0] << " -n 0 -o -t 1 -v 0" << std::endl;
			std::cout << "A deck is provided in the regex ([CHSD][A234567890JQK]){52}" << std::endl;
			std::cout << "This regex is also given in priority order for permutations when -o is given." << std::endl;
			std::cout << "If -o is given (or used by default) without -c, the starting deck is as mentioned above." << std::endl;
			std::cout << "To be more explicit, it's Clubs from A to K, then Hearts, and so on." << std::endl;
			std::cout << "-o doesn't process all decks. Decks functionally identical to another earlier deck are skipped." << std::endl;
			std::cout << "To see how this is done exactly, see the code." << std::endl;
			return 0;
		}
		else if(!strcmp(argv[i], "-1")){
			if(i + 1 < argc)
				singleDeck = argv[++i];
			else{
				std::cerr << "No deck provided to -1" << std::endl;
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-c")){
			if(i + 1 < argc)
				continueDeck = argv[++i];
			else{
				std::cerr << "No deck provided to -c" << std::endl;
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-n")){
			if(i + 1 < argc){
				decks = strtoull(argv[++i], nullptr, 0);
				if(decks == 0 && argv[i - 1][0] != '0'){
					std::cerr << "Invalid argument provided to -n" << std::endl;
					return 1;
				}
			}else{
				std::cerr << "No argument provided to -n" << std::endl;
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-r"))
			random = true;
		else if(!strcmp(argv[i], "-o"));//already set by default
		else if(!strcmp(argv[i], "-t")){
			if(i + 1 < argc){
				threads = strtoul(argv[++i], nullptr, 0);
				if(threads == 0){
					std::cerr << "At least one thread is required" << std::endl;
					return 1;
				}
			}else{
				std::cerr << "No argument provided to -t" << std::endl;
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-v")){
			if(i + 1 < argc){
				unsigned long tmp = strtoul(argv[++i], nullptr, 0);
				if((tmp == 0 && argv[i - 1][0] != '0') || tmp > 3){
					std::cerr << "Invalid verbosity level" << std::endl;
					return 1;
				}
				setLogLevel(static_cast<unsigned char>(tmp));
			}else{
				std::cerr << "No argument provided to -v" << std::endl;
				return 1;
			}
		}
		else{
			std::cout << "Unrecognized argument: " << argv[i] << std::endl;
			return 1;
		}
	}

	if(singleDeck && continueDeck){
		std::cerr << "-1 and -c are exclusive" << std::endl;
		return 1;
	}
	std::string solveString;
	if(singleDeck){
		DeckGenerator d(singleDeck);
		solveString = d.getStart().getHowToSolve();
		if(solveString.size()){
			log(0, "Solvable");
			log(1, ": ");
			log(1, solveString);
			log(0, "\n");
		}
		else
			log(0, "Unsolvable\n");
	}
	else if(continueDeck){
		if(random)
			std::cout << "Note: ignoring -r since -c was given" << std::endl;
		DeckGenerator d(continueDeck);
		uint64_t checked = 0;
		do{
			solveString = d.getStart().getHowToSolve();
			if(solveString.size()){
				log(0, "Solvable");
				log(1, ": ");
				log(1, solveString);
				log(0, "\n");
			}
			else
				log(0, "Unsolvable\n");
			checked++;
		}while(!d.donePermuting() && (checked < decks || !decks));
	}
	else{
		DeckGenerator d(random);
		uint64_t checked = 0;
		do{
			solveString = d.getStart().getHowToSolve();
			if(solveString.size()){
				log(0, "Solvable");
				log(1, ": ");
				log(1, solveString);
				log(0, "\n");
			}
			else
				log(0, "Unsolvable\n");
			checked++;
		}while(!d.donePermuting() && (checked < decks || !decks));
	}
	return 0;
}
