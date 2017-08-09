#include "solver.h"

Trie::Trie() {
	count = 0;
	word = NULL;
	found = false;
	int i;

	for (i = 0; i < MAX_LETTERS; i++) {
		child[i] = NULL;
	}

}

extern Trie* dict;

extern int duplicates;
extern int wordsParsed;
extern int progress;
extern int onePercentage;
extern int * board;
extern int board_size;
extern int puzzle_size;
extern int cols;
extern int * SEARCHED;
extern int * children;

extern vector<char*> found;

//A function for reading the files into the buffer
char* readF(char fname[]) {
	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;

	pFile = fopen(fname, "r");
	if (pFile == NULL) { fputs("File error", stderr); exit(1); }

	//get file size
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	//allocate memory to hold file
	buffer = (char*)malloc(sizeof(char)*lSize);
	if (buffer == NULL) { fputs("Memory error", stderr); exit(2); }

	//load file into the buffer
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) { fputs("Reading error", stderr); exit(3); }

	fclose(pFile);

	return buffer;

}

//Saves the results to a file
void saveResults(char fname[]) {
	FILE * file;
	file = fopen(fname, "w");

	char c;

	//print board
	for (int i = 0; i < board_size; i++) {
		c = (char)(board[i] + 'a');
		if ('q' == c) {
			fputs("Qu", file);
			cout << "Qu";
		}

		else if (BORDER == board[i]) {
			fputs("* ", file);
			cout << "* ";
		}

		else {
			fputc(c, file);
			cout << c;
			fputs(" ", file);
			cout << " ";
		}

		if ((i + 1) % cols == 0) {
			fputs("\n", file);
			cout << endl;
		}
	}
	fputs("\n", file);
	cout << endl;

	//print found word vector
	sort(found.begin(), found.end());

	for (vector<char*>::iterator it = found.begin(); it != found.end(); ++it) {
		fputs(*it, file);
		fputs("\n", file);
	}
	fclose(file);
}

//Creates the board and boarders
void * buildBoard(char boggleFile[]) {
	char * buffer = readF(boggleFile);

	//find the dimensions
	int len = strlen(buffer);
	cols = sqrt(len) + 2;
	children = new int[MAX_NEIGHBORS] {-1 - cols, -cols, 1 - cols, -1, 1, cols - 1, cols, cols + 1};
	board_size = cols * cols;
	puzzle_size = (cols - 2) * (cols - 2);
	onePercentage = double(1) / 100 * puzzle_size;
	board = new int[board_size];

	//add border
	int j = 0;
	for (int i = 0; i < board_size; i++) {
		if ((i < cols) ||           //top
			((i + 1) % cols == 0) ||      //right
			(i > cols * (cols - 1)) ||   //bot
			(i % cols == 0)) {             //left

			board[i] = BORDER;
		}
		else {
			board[i] = buffer[j] - 'a';
			j++;
		}
	}
	return 0;
}

//Recursively traverse the trie and push all found words into vector 'found'
void getFoundWords(Trie* p) {
	int i;
	if (p) {
		if (p->found)
			found.push_back(p->word);

		for (i = 0; i < MAX_LETTERS; i++) {
			getFoundWords(p->child[i]);
		}
	}
}

//Loads the dictionary into the Trie
void buildTrie(char dictFile[])
{
	char * buffer = readF(dictFile);
	char * word;
	int len, i;

	dict = new Trie;

	word = strtok(buffer, "\n\t");
	while (word != NULL) {
		len = strlen(word);
		if (len >= 3) { //Word length in Boggle rules is at least 3
			insertWord(word, len);
			wordsParsed++;
		}

		word = strtok(NULL, "\n\t");
	}
}

//Add a word to the Trie
void insertWord(char word[], const int len) {
	Trie* p = dict;
	int i;
	
	for (i = 0; word[i]; i++) {
		int letter = word[i] - 'a'; //convert characters to ints: a=0 z=25
		
		if (letter == -100)		/*******Included to cover a bug that comes up when reading in the file*******/
			break;
		if (('q' == word[i]) && ('u' == word[i + 1])) //combine 'qu' into single cube represented by 'q'
			i++;
		
		p->count++; //track how many words use this prefix

		if (!p->child[letter]) //if the child isnt initialized, initialize it
			p->child[letter] = new Trie;

		p->child[letter]->root = p;
		p = p->child[letter];
	}
	
	p->word = word; //the last node completes the word, save it here
}


//Returns a Trie* to the next prefix or NULL and keeps track of which words have been found
Trie* lookup(const int i, Trie* p) {
	p = p->child[i];
	if (p && p->word) {
		if (p->found) {
			duplicates++;
		}
		else {
			p->found = true;
			p->root->count--; //decrement how many words are left that use this prefix
		}
	}
	return p;
}

//Recursive DFS function that returns the number of NEW words found in children + self
//Words that have already been found do not count
void descend(int cubeIndex, Trie* p, vector<bool> searched) {
	p = lookup(board[cubeIndex], p);
	if (p && p->count) { //is this a valid prefix? Are there any remaining words that use it?
		searched[cubeIndex] = true; //mark this cube as used

		for (int i = 0; i < MAX_NEIGHBORS; i++) { //descend to each neighboring cube
			int child = cubeIndex + children[i];
			if ((board[child] != BORDER) && !searched[child]) //faster to check here
				descend(child, p, searched);
		}
	}
}

//For each space on the board, perform a depth first search using descend()
void traverseBoard() {
	Trie* p = dict;
	vector<bool> searched; //spaces should be used only once per word
	int i, j = 0;

	//initialize searched to false for all cubes on the board
	for (int i = 0; i < board_size; i++) {
		searched.push_back(false);
	}

	for (int i = 0; i < board_size; i++) { //for each cube
		if (board[i] != BORDER) {
			descend(i, p, searched); //DFS
			j++;
		}
	}
}
