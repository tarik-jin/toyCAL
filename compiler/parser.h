#pragma once
#include "common.h"

class Parser{

	//start of syntax
	void program();
	void segment();
	Tag type();

	//declare & define
	void def();
	void idTail();
	void varArrayDef();
	void init();
	void defList();
	void defData();

	//fun
	void para();
	void paraData();
	void paraList();
	void paraDataTail();//array args
	void funTail();
	void block();
	void subProgram();
	void localDef();

	//statement
	void statement();
	void whileStat();
	void doWhileStat();
	void forStat();
	void forInit();
	void ifStat();
	void elseStat();
	void switchStat();
	void caseStat();
	void caseLabel();

	//expression
	void altExpr();
	void expr();
	void assExpr();
	void assTail();
	void orExpr();
	void orTail();
	void andExpr();
	void andTail();
	void cmpExpr();
	void cmpTail();
	Tag cmps();
	void aloExpr();
	void aloTail();
	Tag adds();
	void item();
	void itemTail();
	Tag muls();
	void factor();
	Tag lop();
	void val(); 
	Tag rop();
	void elem();
	void literal();
	void idExpr();
	void realArg();
	void argList();
	void arg();

	Lexer &lexer;
	Token* look;

	void move();//token by token
	bool match(Tag t);
	void recovery(bool cond, SynError lost, SynError wrong);

public:
	Parser(Lexer& lex);
	~Parser();

	void analyse();
};
