#pragma once
#include "common.h"

#define UNDEF 0.5
#define NAC -0.5

class ConstPropagation{

	SymTab* tab;
	DFG* dfg;

	vector<Var*> vars;
	vector<Var*> glbVars;
	vector<double> boundVals;
	vector<double> initVals;

	double join(double left, double right);
	double join(Block* block);
	void translate(InterInst* inst, vector<double>& in, vector<double>& out);
	bool translate(Block* block);
	void analyse();
	void algebraSimlify();
	void condJmpOpt();

public:
	ConstPropagation(DFG* g, SymTab* tab, vector<Var*>& paraVar);
	void propagate();
};
