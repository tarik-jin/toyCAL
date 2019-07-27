#pragma once
#include "common.h"

class Parser{

	//start of syntax
	void program();
	void segment();
	Tag type();

	//declare & define
	void def(bool ext, Tag t);
	void idTail(bool ext, Tag t, bool ptr, string name);
	void varArrayDef();
	Var* init(bool ext, Tag t, bool ptr, string name);
	void defList(bool ext, Tag t);
	Var* defData(bool ext, Tag t);

	//fun
	void para(vector<Var*>& list);
	Var* paraData(Tag t);
	void paraList(vector<Var*>& list);
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
	Var* literal();
	Var* idExpr(string name);
	void realArg();
	void argList();
	void arg();

	Lexer &lexer;
	Token* look;

	SymTab &symtab;

	void move();//token by token
	bool match(Tag t);
	void recovery(bool cond, SynError lost, SynError wrong);

public:
	Parser(Lexer& lex, SymTab& tab);
	~Parser();

	void analyse();
};
