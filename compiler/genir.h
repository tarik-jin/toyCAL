#pragma once
#include "common.h"

class GenIR{
	SymTab &symtab;

	Var* genIncR(Var* val);
	Var* genDecR(Var* val);
	Var* genAdd(Var* lval, Var* rval);
	Var* genPtr(Var* val);

public:

	Var* genAssign(Var* val);

	Var* genArray(Var* array, Var* index);
	Var* genOneOpRight(Var* val, Tag opt);
	bool genVarInit(Var* var);
	Var* genTwoOp(Var* lval, Tag opt, Var* rval);

	void genReturn(Var* ret);

};
