#include "solver.h"
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

Trie* dict = new Trie();

int duplicates = 0;
int wordsParsed = 0;
int progress = 0;
int onePercentage;
int * board;
int board_size;
int puzzle_size;
int cols;
int * SEARCHED;
int * children;

vector<char*> found;

void randomBoard(int size, string iFile);

int main(int argc, char* argv[]) {

	char boggleFile[100] = "boggle.txt";
	char dictFile[100] = "mydictionary.txt";
	char resultsFile[100] = "results.txt";

	cout << "*********************************************" << endl
		<< "Welcome to Bungie Boggle!" << endl
		<< "*********************************************" << endl << endl;

	if (argc > 1)
		strcpy(boggleFile, argv[1]);
	if (argc > 2)
		strcpy(dictFile, argv[2]);

	int option = 0;
	cout << "If you would like to build a random board file press [1]" << endl
		<< "Or if you would like to use the current file you want to use press any other key" << endl;

	cin >> option;

	if (option == 1) {
		int size;
		do {
			cout << endl << endl << "Enter one number for the size of the rows and columns" << endl
				<< "(5 would mean a 5 x 5 grid)" << endl
				<< "Enter a number 4 or above" << endl;

			cin >> size;
			randomBoard(size, boggleFile);
		} while (size < 4);
	}
	buildBoard(boggleFile);
	buildTrie(dictFile);

	cout << wordsParsed << " words parsed in " << dictFile << endl
		<< puzzle_size << " cubes on the board -> " << sqrt(puzzle_size) << " X " 
		<< sqrt(puzzle_size) << " board" << endl;

	traverseBoard();
	getFoundWords(dict);

	cout << found.size() << " words found" << endl
		<< duplicates << " duplicate words found" << endl;
	cout << "Results of board and all found words saved to " << resultsFile << endl
		<< "_____________________________________________" << endl << endl;
	
	saveResults(resultsFile);
}

void randomBoard(int size, string iFile) {
	int board_size = size * size;
	char letter;
	ofstream file;

	file.open(iFile, 'w');

	srand(time(NULL));

	for (int i = 0; i < board_size; i++) {
		letter = 97 + rand() % 26;	//Using ASCII table to pick a random lowercase letter
		file << letter;
	}
	file.close();

}