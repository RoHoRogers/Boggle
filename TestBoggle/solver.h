#pragma once

#ifndef _SOLVER_H_
#define _SOLVER_H_

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <locale>

using namespace std;

const int MAX_LETTERS = 26; //number of letters in our alphabet
const int MAX_NEIGHBORS = 8; //max number of neighboring cubes
const int BORDER = MAX_LETTERS; //surrounds the Boggle puzzle. Should be 1 > than last int in alphabet

class Trie {
public:
	Trie* child[MAX_LETTERS];
	Trie* root;
	int count; //number of suffixes that share this as a root
	char* word; //if this node completes a word, store it here
	bool found; //if this word been found or not

	Trie();
};

char* readF(char fname[]);
void saveResults(char fname[]);
void * buildBoard(char boggleFile[]);
void getFoundWords(Trie* p);
void buildTrie(char dictFile[]);
void insertWord(char word[], const int len);
Trie* lookup(const int i, Trie* p);
void descend(int cubeIndex, Trie* p, vector<bool> searched);
void traverseBoard();

#endif // !_SOLVER_H_

