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

void  GenIR::genReturn(Var* ret){
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
