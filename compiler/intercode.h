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
public:
	InterInst(Operator op, Var *arg1 = NULL);
	InterInst(Operator op, InterInst* tar, Var* arg1 = NULL, Var* arg2 = NULL);


};

class InterCode{
	vector<InterInst*> code;
public:
	~InterCode();

	void addInst(InterInst* inst);
};
