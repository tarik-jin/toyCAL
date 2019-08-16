#pragma once
#include "common.h"
#include "set.h"

class LiveVar{
	SymTab* tab;
	DFG* dfg;
	list<InterInst*> optCode;
	vector<Var*> varList;
	Set U;
	Set E;
	Set G;
	bool translate(Block* block);
public:
	void analyse();
	LiveVar(DFG* g, SymTab* t, vector<Var*>& paraVar);
	void elimateDeadCode(bool stop = false);
};
