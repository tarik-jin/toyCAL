#pragma once
#include "common.h"
#include "set.h"

class CopyPropagation{

	DFG* dfg;
	list<InterInst*> optCode;
	vector<InterInst*> copyExpr;
	SymTab* tab;
	Set U;
	Set E;
	Set G;

	bool translate(Block* block);
	void analyse();
	Var* find(Set& in, Var* var);
public:
	CopyPropagation(DFG* g, SymTab* t);
	void propagate();

};
