#pragma once
#include "common.h"

class Parser{
	Lexer& lexer;
	Token* look;

	void program();
	bool match(Tag t);
	void move();

	void lbTail();
	void baseTail();
	int len();
	void value();
	void type();
	void valTail();
	void off();
	void inst();
	void oprand();
	void reg();
	void mem();
	void addr();
	void regAddr();
	void regAddrTail();
public:
	Parser();
};
