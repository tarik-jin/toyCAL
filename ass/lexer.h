#pragma once
#include "common.h"

class Scanner{
	char* fileName;
	FILE* file;

	static const int BUFLEN = 80;
	char line[BUFLEN];
	int lineLen;
	int readPos;
	char lastch;

	int lineNum;
	void showChar(char ch);
public:
	Scanner(char* name);
	~Scanner();
	int scan();
	int getLine();
};

class Keywords{
	struct string_hash{
       size_t operator()(const string& str) const{
           std::hash<std::string> hash_fn;
           return hash_fn(str);
       }
   };
   unordered_map<string, Tag, string_hash> keywords;
public:
	Tag getTag(string name);
};

class Lexer{
	static Keywords keywords;
	Scanner &scanner;
	char ch;
	Token* token;
public:
	Lexer(Scanner& sc);
	~Lexer();
	Token* tokenize();
};
