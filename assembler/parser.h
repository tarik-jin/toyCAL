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
	void type(string lbName, list<int>& cont, int len);
	void valTail(string lbName, list<int>& cont, int len);
	void off();
	void inst();
	void oprand(int& regNum, op_type& type, int& len);
	int reg();
	void mem();
	void addr();
	void regAddr(Token* baseReg, int type);
	void regAddrTail(Token* baseReg, int type, Token* negative);
	int getRegCode(Tag reg, int len);
public:
	Parser(Lexer& lex);
	void analyse();
	void reset();
};
