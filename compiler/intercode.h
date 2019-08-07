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

	void init();

public:

	InterInst();
	InterInst(Operator op, Var* arg1 = NULL);
	InterInst(Operator op, Var* rs, Var* arg1, Var* arg2 = NULL);
	InterInst(Operator op, InterInst* tar, Var* arg1 = NULL, Var* arg2 = NULL);
	InterInst(Operator op, Fun* fun, Var* rs = NULL);

	void toString();

};

class InterCode{
	vector<InterInst*> code;
public:
	~InterCode();

	void addInst(InterInst* inst);
	void toString();
};
