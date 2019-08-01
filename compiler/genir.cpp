#include "genir.h"
#include "symbol.h"
#include "symtab.h"
#include "error.h"

#define SEMERROR(code) Error::semError(code)

bool GenIR::genVarInit(Var* var){
	if(var->getName()[0] == '<'){
		return false;
	}
	else{
		symtab.addInst(new InterInst(OP_DEC, var));
		if(var->setInit()){
			genTwoOp(var, ASSIGN, var->getInitData());
		}
		else{
		}
		return true;
	}
}

Var* GenIR::genOneOpRight(Var* val, Tag opt){
	if(!val){
		return NULL;
	}
	else if(val->isVoid()){
		SEMERROR(EXPR_IS_VOID);
		return NULL;
	}
	else if(!val->getLeft()){
		SEMERROR(EXPR_NOT_LEFT_VAL);
		return val;
	}
	else{
		if(opt == INC){
			return genIncR(val);
		}
		else if(opt == DEC){
			return genDecR(val);
		}
		else{
			return val;
		}
	}
}

Var* GenIR::genArray(Var* array, Var* index){
	if(!array || !index){
		return NULL;
	}
	else if(array->isVoid() || index->isVoid()){
		SEMERROR(EXPR_IS_VOID);
		return NULL;
	}
	else if(array->isBase() || !index->isBase()){
		SEMERROR(ARR_TYPE_ERR);
		return index;
	}
	else{
		return genPtr(genAdd(array, index));
	}
}

void GenIR::genReturn(Var* ret){
	if(!ret){
		return;
	}
	else{
		Fun* fun = symtab.getCurFun();
		if(ret->isVoid() && fun->getType() != KW_VOID ||
				ret->isBase() && fun->getType() == KW_VOID){
			SEMERROR(RETURN_ERR);
			return;
		}
		else{
			InterInst* returnPoint = fun->getReturnPoint();
			if(ret->isVoid()){
				symtab.addInst(new InterInst(OP_RET, returnPoint));
			}
			else{
				if(ret->isRef()){
					ret = genAssign(ret);
				}
				else{
				}
				symtab.addInst(new InterInst(OP_RETV, returnPoint, ret));
			}
			return;
		}
	}
}

void GenIR::genFunHead(Fun* function){
	function->enterScope();
	symtab.addInst(new InterInst(OP_ENTRY, function));
	function->setReturnPoint(new InterInst);
	return;
}

void GenIR::genFunTail(Fun* function){
	symtab.addInst(function->getReturnPoint());
	symtab.addInst(new InterInst(OP_EXIT, function));
	function->leaveScope();
	return;
}

Var* GenIR::genPtr(Var* val){
	if(val->isBase()){
		SEMERROR(EXPR_IS_BASE);
		return val;
	}
	Var* tmp = new Var(symtab.getScopePath(), val->getType(), false);
	tmp->setLeft(true);
	tmp->setPointer(val);
	symtab.addVar(tmp);
	return tmp;
}

Var* GenIR::genLea(Var* val){
	if(!val->getLeft()){
		SEMERROR(EXPR_NOT_LEFT_VAL);
		return val;
	}
	else{
		if(val->isRef()){
			return val->getPointer();
		}
		else{
			Var* tmp = new Var(symtab.getScopePath(), val->getType(), true);
			symtab.addVar(tmp);
			symtab.addInst(new InterInst(OP_LEA, tmp, val));
			return tmp;
		}
	}
}

Var* GenIR::genAssign(Var* val){
	Var* tmp = new Var(symtab.getScopePath(), val);
	symtab.addVar(tmp);
	if(val->isRef()){
		symtab.addInst(new InterInst(OP_GET, tmp, val->getPointer()));
	else{
		symtab.addInst(new InterInst(OP_AS, tmp, val));
	}
	return tmp;
}

Var* GenIR::genAssign(Var* lval, Var* rval){
	if(!val->getLeft()){
		SEMERROR(EXPR_NOT_LEFT_VAL);
		return rval;
	}
	else if(!typeCheck(lval, rval)){
		SEMERROR(ASSIGN_TYPE_ERR);
		return rval;
	}
	else{
		if(rval->isRef()){
			rval = genAssign(rval);
		}
		else{
		}
		if(lval->isRef()){
			symtab.addInst(new InterInst(OP_SET, rval, lval->getPointer()))
		}
		else{
			symtab.addInst(new InterInst(OP_AS, lval, rval));
		}
		return lval;
	}
}

bool GenIR::typeCheck(Var* lval, Var* rval){
	if(!rval){
		return false;
	}
	else{
		if(lval->isBase() && rval->isBase()){
			return true;
		}
		else if(!lval->isBase() && !rval->isBase()){
			return rval->getType() == lval->getType();
		}
		else{
			return false;
		}
	}
}

