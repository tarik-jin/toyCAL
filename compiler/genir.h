#pragma once
#include "common.h"

class GenIR{
	SymTab &symtab;

	Var* genIncR(Var* val);
	Var* genDecR(Var* val);
	Var* genPtr(Var* val);
	Var* genLea(Var* val);
	Var* genNot(Var* val);
	Var* genMinus(Var* val);
	Var* genIncL(Var* val);
	Var* genDecL(Var* val);


	void genPara(Var* arg);

	Var* genAssign(Var* lval, Var* rval);

	Var* genOr(Var* lval, Var* rval);
	Var* genAnd(Var* lval, Var* rval);
	Var* genEqu(Var* lval, Var* rval);
	Var* genNequ(Var* lval, Var* rval);
	Var* genAdd(Var* lval, Var* rval);
	Var* genSub(Var* lval, Var* rval);
	Var* genGt(Var* lval, Var* rval);
	Var* genGe(Var* lval, Var* rval);
	Var* genLt(Var* lval, Var* rval);
	Var* genLe(Var* lval, Var* rval);
	Var* genMul(Var* lval, Var* rval);
	Var* genDiv(Var* lval, Var* rval);
	Var* genMod(Var* lval, Var* rval);

	vector<InterInst*> heads;
	vector<InterInst*> tails;
	void push(InterInst* head, InterInst* tail);
	void pop();

	static int lbNum;

public:

	GenIR(SymTab& tab);

	Var* genAssign(Var* val);

	Var* genArray(Var* array, Var* index);
	bool genVarInit(Var* var);
	Var* genTwoOp(Var* lval, Tag opt, Var* rval);
	Var* genOneOpLeft(Tag opt, Var* val);
	Var* genOneOpRight(Var* val, Tag opt);

	void genIfHead(Var* cond, InterInst*& _else);
	void genIfTail(InterInst*& _else);
	void genElseHead(InterInst* _else, InterInst*& _exit);
	void genElseTail(InterInst*& _exit);
	void genSwitchHead(InterInst*& _exit);
	void genSwitchTail(InterInst* _exit);
	void genCaseHead(Var* cond, Var* lb, InterInst*& _case_exit);
	void genCaseTail(InterInst* _case_exit);
	void genWhileHead(InterInst*& _while, InterInst*& _exit);
	void genWhileCond(Var* cond, InterInst* _exit);
	void genWhileTail(InterInst*& _while, InterInst*& _exit);
	void genDoWhileHead(InterInst*& _do, InterInst*& _exit);
	void genDoWhileTail(Var* cond, InterInst* _do, InterInst* _exit);
	void genForHead(InterInst*& _for, InterInst*& _exit);
	void genForCondBegin(Var* cond, InterInst*& _step, InterInst*& _block, InterInst* _exit);
	void genForCondEnd(InterInst* _for, InterInst* _block);
	void genForTail(InterInst*& _step, InterInst*& _exit);


	Var* genCall(Fun* function, vector<Var*>& args);
	void genReturn(Var* ret);
	void genFunHead(Fun* function);
	void genFunTail(Fun* function);
	void genBreak();
	void genContinue();

	static bool typeCheck(Var* lval, Var* rval);
	static string genLb();
};


