#pragma once
#include "common.h"

class GenIR{
	SymTab &symtab;
public:

	Var* genArray(Var* array, Var* index);
	Var* genOneOpRight(Var* val, Tag opt);
	bool genVarInit(Var* var);

};
