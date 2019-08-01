#pragma once
#include "common.h"

class InterInst{
private:
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
