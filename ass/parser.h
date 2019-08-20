#pragma once
#include "common.h"

class Parser{
	Lexer& lexer;
	Token* look;

	void program();
	bool match(Tag t);
	void move();

	void lbTail(string lbName);
	void baseTail(string lbName, int times);
	int len();
	void value(string lbName, int times, int len);
	void type(list<int>& cont, int len);
	void valTail(list<int>& cont, int len);
	void off();
	void inst();
	void oprand();
	void reg();
	void mem();
	void addr();
	void regAddr();
	void regAddrTail();
public:
	Parser(Lexer& lex);
	void analyse();
	void reset();
};
