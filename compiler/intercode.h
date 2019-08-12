#pragma once
#include "common.h"

class InterInst{
private:
	string label;
	Operator op;
	Var* result;
	Var* arg1;
	Var* arg2;
	Fun* fun;
	InterInst* target;
	bool first;

	void init();
	void loadVar(string reg32, string reg8, Var* var);
	void leaVar(string reg32, Var* var);
	void storeVar(string reg32, string reg8, Var* var);
	void initVar(Var* var);

public:

	InterInst();
	InterInst(Operator op, Var* arg1 = NULL);
	InterInst(Operator op, Var* rs, Var* arg1, Var* arg2 = NULL);
	InterInst(Operator op, InterInst* tar, Var* arg1 = NULL, Var* arg2 = NULL);
	InterInst(Operator op, Fun* fun, Var* rs = NULL);

	void toString();
	void toX86();

	static FILE* file;
	Block* block;

	Fun* getFun();
	bool isFirst();
	bool isJmp();
	bool isJcond();
	InterInst* getTarget();
	void setFirst();

};

class InterCode{
	vector<InterInst*> code;
public:
	~InterCode();

	void addInst(InterInst* inst);
	void toString();
	vector<InterInst*>& getCode();

	void markFirst();
};
