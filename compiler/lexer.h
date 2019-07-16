#include"common.h"
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

public:
	Scanner(char* name);
	~Scanner();

	char scan();

	char* getFile();
	int getLine();
	int getCol();
};

class Lexer{
	char ch;//cur character
	void scan();
	Scanner &scanner;
public:
	Lexer(Scanner& sc);
	Token* tokenize();
};
