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
	Var* varArrayDef(bool ext, Tag t, bool ptr, string name);
	Var* init(bool ext, Tag t, bool ptr, string name);
	void defList(bool ext, Tag t);
	Var* defData(bool ext, Tag t);

	//fun
	void para(vector<Var*>& list);
	Var* paraData(Tag t);
	void paraList(vector<Var*>& list);
	Var* paraDataTail(Tag t, string name);//array args
	void funTail(Fun* f);
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
	Var* expr();
	Var* assExpr();
	Var* assTail(Var* lval);
	Var* orExpr();
	Var* orTail(Var* lval);
	Var* andExpr();
	Var* andTail(Var* lval);
	Var* cmpExpr();
	Var* cmpTail(Var* lval);
	Tag cmps();
	Var* aloExpr();
	Var* aloTail(Var* lval);
	Tag adds();
	Var* item();
	Var* itemTail(Var* lval);
	Tag muls();
	Var* factor();
	Tag lop();
	Var* val();
	Tag rop();
	Var* elem();
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
