#pragma once
#include "common.h"

class GenIR{
	SymTab &symtab;

	Var* genIncR(Var* val);
	Var* genDecR(Var* val);
	Var* genPtr(Var* val);
	Var* genLea(Var* val);

	Var* genAdd(Var* lval, Var* rval);
	Var* genAssign(Var* lval, Var* rval);

public:

	Var* genAssign(Var* val);

	Var* genArray(Var* array, Var* index);
	Var* genOneOpRight(Var* val, Tag opt);
	bool genVarInit(Var* var);
	Var* genTwoOp(Var* lval, Tag opt, Var* rval);

	void genReturn(Var* ret);
	void genFunHead(Fun* function);
	void genFunTail(Fun* function);

	static bool typeCheck(Var* lval, Var* rval);
};
