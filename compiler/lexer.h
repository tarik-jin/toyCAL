#pragma once
#include "common.h"

//scan file character by character
class Scanner{
	//file pointer
	char* fileName;
	FILE* file;
	//internal state
	static const int BUFFLEN = 80;
	char line[BUFFLEN];
	int readPos;
	char lastch;
	int lineLen;//actual read bytes
	//read done state
	int lineNum;
	int colNum;

	void showChar(char ch);

public:
	Scanner(char* name);
	~Scanner();

	char scan();

	char* getFile();
	int getLine();
	int getCol();
};

class Lexer{
	static Keywords keywords;

	char ch;//cur character
	Token* token;
	Scanner &scanner;

	bool scan(char need = 0);
public:
	Lexer(Scanner& sc);
	~Lexer();

	Token* tokenize();
};
