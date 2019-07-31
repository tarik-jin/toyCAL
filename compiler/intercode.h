#pragma once
#include "common.h"

class InterInst{
private:
public:
	InterInst(Operator op, Var *arg1 = NULL);


};

class InterCode{
	vector<InterInst*> code;
public:
	~InterCode();

	void addInst(InterInst* inst);
};
