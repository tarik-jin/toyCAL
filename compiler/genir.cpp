#include "genir.h"
#include "symbol.h"
#include "symtab.h"
#include "error.h"

#define SEMERROR(code) Error::semError(code)

GenIR::GenIR(SymTab& tab):symtab(tab){
	symtab.setIr(this);
	lbNum = 0;
	push(NULL, NULL);
}

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
	function->setReturnPoint(new InterInst());
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
	}
	else{
		symtab.addInst(new InterInst(OP_AS, tmp, val));
	}
	return tmp;
}

Var* GenIR::genAssign(Var* lval, Var* rval){
	if(!lval->getLeft()){
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
			symtab.addInst(new InterInst(OP_SET, rval, lval->getPointer()));
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

Var* GenIR::genTwoOp(Var* lval, Tag opt, Var* rval){
	if(!lval || !rval){
		return NULL;
	}
	else if(lval->isVoid() || rval->isVoid()){
		SEMERROR(EXPR_IS_VOID);
		return NULL;
	}
	else if(opt == ASSIGN){
		return genAssign(lval, rval);
	}
	else{
		if(lval->isRef()){
			lval = genAssign(lval);
		}
		else{
		}
		if(rval->isRef()){
			rval = genAssign(rval);
		}
		else{
		}
		if(opt == OR){
			return genOr(lval, rval);
		}
		else if(opt == AND){
			return genAnd(lval, rval);
		}
		else if(opt == EQU){
			return genEqu(lval, rval);
		}
		else if(opt == NEQU){
			return genNequ(lval, rval);
		}
		else if(opt == ADD){
			return genAdd(lval, rval);
		}
		else if(opt == SUB){
			return genSub(lval, rval);
		}
		else if(!lval->isBase() || !rval->isBase()){
			SEMERROR(EXPR_NOT_BASE);
			return lval;
		}
		else if(opt == GT){
			return genGt(lval, rval);
		}
		else if(opt == GE){
			return genGe(lval, rval);
		}
		else if(opt == LT){
			return genLt(lval, rval);
		}
		else if(opt == LE){
			return genLe(lval, rval);
		}
		else if(opt == MUL){
			return genMul(lval, rval);
		}
		else if(opt == DIV){
			return genDiv(lval, rval);
		}
		else if(opt == MOD){
			return genMod(lval, rval);
		}
		else{
			return lval;
		}
	}
}

Var* GenIR::genOr(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_OR, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genAnd(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_AND, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genEqu(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_EQU, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genNequ(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_NE, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genAdd(Var* lval, Var* rval){
	Var* tmp = NULL;
	if(!lval->isBase() && rval->isBase()){
		tmp = new Var(symtab.getScopePath(), lval);
		rval = genMul(rval, Var::getStep(lval));
	}
	else if(lval->isBase() && !rval->isBase()){
		tmp = new Var(symtab.getScopePath(), rval);
		lval = genMul(lval, Var::getStep(rval));
	}
	else if(lval->isBase() && rval->isBase()){
		tmp = new Var(symtab.getScopePath(), KW_INT, false);
	}
	else{
		SEMERROR(EXPR_NOT_BASE);
		return lval;
	}
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_ADD, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genSub(Var* lval, Var* rval){
	if(!rval->isBase()){
		SEMERROR(EXPR_NOT_BASE);
		return lval;
	}
	else{
		Var* tmp = NULL;
		if(!lval->isBase()){
			tmp = new Var(symtab.getScopePath(), lval);
			rval = genMul(rval, Var::getStep(lval));
		}
		else{
			tmp = new Var(symtab.getScopePath(), KW_INT, false);
		}
		symtab.addVar(tmp);
		symtab.addInst(new InterInst(OP_SUB, tmp, lval, rval));
		return tmp;
	}
}

Var* GenIR::genMul(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_MUL, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genGt(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_GT, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genGe(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_GE, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genLt(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_LT, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genLe(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_LE, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genDiv(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_DIV, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genMod(Var* lval, Var* rval){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_MOD, tmp, lval, rval));
	return tmp;
}

Var* GenIR::genOneOpLeft(Tag opt, Var* val){
	if(!val){
		return NULL;
	}
	else if(val->isVoid()){
		SEMERROR(EXPR_IS_VOID);
		return NULL;
	}
	else{
		//&x *p
		if(opt == LEA){
			return genLea(val);
		}
		else if(opt == MUL){
			return genPtr(val);
		}
		else if(opt == INC){
			return genIncL(val);
		}
		else if(opt == DEC){
			return genDecL(val);
		}
		else{
			if(val->isRef()){
				val == genAssign(val);
			}
			else{
			}
			if(opt == NOT){
				return genNot(val);
			}
			else if(opt == SUB){
				return genMinus(val);
			}
			else{
				return val;
			}
		}
	}
}

Var* GenIR::genIncL(Var* val){
	if(!val->getLeft()){
		SEMERROR(EXPR_NOT_LEFT_VAL);
		return val;
	}
	else{
		if(val->isRef()){
			Var* t1 = genAssign(val);
			Var* t2 = genAdd(t1, SymTab::one);
			return genAssign(val, t2);
		}
		else{
			symtab.addInst(new InterInst(OP_ADD, val, val, Var::getStep(val)));
			return val;
		}
	}
}

Var* GenIR::genDecL(Var* val){
	if(!val->getLeft()){
		SEMERROR(EXPR_NOT_LEFT_VAL);
		return val;
	}
	else{
		if(val->isRef()){
			Var* t1 = genAssign(val);
			Var* t2 = genSub(t1, SymTab::one);
			return genAssign(val, t2);
		}
		else{
			symtab.addInst(new InterInst(OP_SUB, val, val, Var::getStep(val)));
			return val;
		}
	}
}

Var* GenIR::genNot(Var* val){
	Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
	symtab.addVar(tmp);
	symtab.addInst(new InterInst(OP_NOT, tmp, val));
	return tmp;
}

Var* GenIR::genMinus(Var* val){
	if(!val->isBase()){
		SEMERROR(EXPR_NOT_BASE);
		return val;
	}
	else{
		Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
		symtab.addVar(tmp);
		symtab.addInst(new InterInst(OP_NEG, tmp, val));
		return tmp;
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

Var* GenIR::genIncR(Var* val){
	Var* tmp = genAssign(val);
	if(val->isRef()){
		Var* t2 = genAdd(tmp, SymTab::one);
		genAssign(val, t2);
	}
	else{
		symtab.addInst(new InterInst(OP_ADD, val, val, Var::getStep(val)));
	}
	return tmp;
}

Var* GenIR::genDecR(Var* val){
	Var* tmp = genAssign(val);
	if(val->isRef()){
		Var* t2 = genSub(tmp, SymTab::one);
		genAssign(val, t2);
	}
	else{
		symtab.addInst(new InterInst(OP_SUB, val, val, Var::getStep(val)));
	}
	return tmp;
}

void GenIR::genPara(Var* arg){
	if(arg->isRef()){
		arg = genAssign(arg);
	}
	else{
	}
	InterInst* argInst = new InterInst(OP_ARG, arg);
	symtab.addInst(argInst);
	return;
}

Var* GenIR::genCall(Fun* function, vector<Var*>& args){
	if(!function){
		return NULL;
	}
	else{
		for(int i = args.size() - 1; i >= 0; i--){
			genPara(args[i]);
		}
		if(function->getType() == KW_VOID){
			symtab.addInst(new InterInst(OP_PROC, function));
			return Var::getVoid();
		}
		else{
			Var* ret = new Var(symtab.getScopePath(), function->getType(), false);
			symtab.addInst(new InterInst(OP_CALL, function, ret));
			symtab.addVar(ret);
			return ret;
		}
	}
}

void GenIR::genIfHead(Var* cond, InterInst*& _else){
	_else = new InterInst();
	if(cond){
		if(cond->isRef()){
			cond = genAssign(cond);
		}
		else{
		}
		symtab.addInst(new InterInst(OP_JF, _else, cond));
	}
	else{
	}
	return;
}

void GenIR::genIfTail(InterInst*& _else){
	symtab.addInst(_else);
}

void GenIR::genElseHead(InterInst* _else, InterInst*& _exit){
	_exit = new InterInst();
	symtab.addInst(new InterInst(OP_JMP, _exit));
	symtab.addInst(_else);
}

void GenIR::genElseTail(InterInst*& _exit){
	symtab.addInst(_exit);
}

string GenIR::genLb(){
	lbNum++;
	string lb = ".L";
	stringstream ss;
	ss << lbNum;
	return lb + ss.str();
}

void GenIR::genSwitchHead(InterInst*& _exit){
	_exit = new InterInst();
	push(NULL, _exit);
}

void GenIR::genSwitchTail(InterInst*& _exit){
	symtab.addInst(_exit);
	pop();
}

void GenIR::genCaseHead(Var* cond, Var* lb, InterInst*& _case_exit){
	_case_exit = new InterInst();
	if(lb){
		symtab.addInst(new InterInst(OP_JNE, _case_exit, cond, lb));
	}
	else{
	}
}

void GenIR::genCaseTail(InterInst* _case_exit){
	symtab.addInst(_case_exit);
}

void GenIR::genWhileHead(InterInst*& _while, InterInst*& _exit){
	_while = new InterInst();
	symtab.addInst(_while);
	_exit = new InterInst();
	push(_while, _exit);
}

void GenIR::genWhileCond(Var* cond, InterInst* _exit){
	if(cond){
		if(cond->isVoid()){
			cond = Var::getTrue();
		}
		else if(cond->isRef()){
			cond = genAssign(cond);
		}
		else{
		}
		symtab.addInst(new InterInst(OP_JF, _exit, cond));
	}
	else{
		//caller altExpr() would not return empty cond!
	}
	return;
}

void GenIR::genWhileTail(InterInst*& _while, InterInst*& _exit){
	symtab.addInst(new InterInst(OP_JMP, _while));
	symtab.addInst(_exit);
	pop();
}

void GenIR::genDoWhileHead(InterInst*& _do, InterInst*& _exit){
	_do = new InterInst();
	_exit = new InterInst();
	symtab.addInst(_do);
	push(_do, _exit);
}

void GenIR::genDoWhileTail(Var* cond, InterInst* _do, InterInst* _exit){
	if(cond){
		if(cond->isVoid()){
			cond = Var::getTrue();
		}
		else if(cond->isRef()){
			cond = genAssign(cond);
		}
		else{
		}
		symtab.addInst(new InterInst(OP_JT, _do, cond));
	}
	else{
		//assert(0); like while-statement
	}
	symtab.addInst(_exit);
	pop();
}

void GenIR::genForHead(InterInst*& _for, InterInst*& _exit){
	_for = new InterInst();
	_exit = new InterInst();
	symtab.addInst(_for);
}

void GenIR::genForCondBegin(Var* cond, InterInst*& _step, InterInst*& _block, InterInst*& _exit){
	_block = new InterInst();
	_step = new InterInst();
	if(cond){
		if(cond->isVoid()){
			cond = Var::getTrue();
		}
		else if(cond->isRef()){
			cond = genAssign(cond);
		}
		else{
		}
		symtab.addInst(new InterInst(OP_JF, _exit, cond));
		symtab.addInst(new InterInst(OP_JMP, _block));
	}
	else{
		//assert(0); like while-statement
	}
	symtab.addInst(step);
	push(_step, _exit);
	return;
}

void GenIR::genForCondEnd(InterInst* _for, InterInst* _block){
	symtab.addInst(new InterInst(OP_JMP, _for));
	symtab.addInst(_block);
}

void GenIR::genForTail(InterInst*& _step, InterInst*& _exit){
	symtab.addInst(new InterInst(OP_JMP, _step));
	symtab.addInst(_exit);
	pop();
}

void GenIR::push(InterInst* head, InterInst* tails){
	heads.push_back(head);
	tails.push_back(tail);
}

void GenIR::pop(){
	heads.pop_back();
	tails.pop_back();
}

void GenIR::genBreak(){
	InterInst* tail = tails.back();
	if(tail){
		symtab.addInst(new InterInst(OP_JMP, tail));
	}
	else{
		SEMERROR(BREAK_ERR);
	}
}

void GenIR::genContinue(){
	InterInst* head = heads.back();
	if(head){
		symtab.addInst(new InterInst(OP_JMP, head))
	}
	else{
		SEMERROR(CONTINUE_ERR);
	}
}
