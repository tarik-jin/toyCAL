#include "copyprop.h"
#include "dfg.h"
#include "intercode.h"
#include "symtab.h"
#include "symbol.h"

CopyPropagation::CopyPropagation(DFG* g, SymTab* t):dfg(g), tab(t){
	dfg->toCode(optCode);
	int j = 0;
	for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
		InterInst* inst = *i;
		Operator op = inst->getOp();
		if(op == OP_AS){
			copyExpr.push_back(inst);
		}
		else{}
	}
	U.init(copyExpr.size(), 1);
	E.init(copyExpr.size(), 0);
	G.init(copyExpr.size(), 0);
	vector<Var*> glbVars = tab->getGlbVars();
	for(unsigned int i = 0; i < glbVars.size(); i++){
		for(unsigned int j = 0; j < copyExpr.size(); j++){
			if(glbVars[i] == copyExpr[j]->getResult() || glbVars[i] == copyExpr[j]->getResult()){
				G.set(i);
			}
			else{}
		}
	}
	for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
		InterInst* inst = *i;
		inst->copyInfo.gen = E;
		inst->copyInfo.kill = E;
		Var* rs = inst->getResult();
		Operator op = inst->getOp();
		if((op == OP_SET) || (op == OP_ARG && !inst->getArg1()->isBase())){
			inst->copyInfo.kill = U;
		}
		else if(op == OP_PROC || op == OP_CALL){
			inst->copyInfo.kill = G;
		}
		else{}
		if((op >= OP_AS && op <= OP_OR) || (op == OP_GET) || (op == OP_CALL)){
			for(unsigned int i = 0; i < copyExpr.size(); i++){
				if(rs == copyExpr[i]->getResult() || rs == copyExpr[i]->getArg1()){
					inst->copyInfo.kill.set(i);
				}
				else{}
				//if inst == copyExpr[i], we say that it kill and gen the expr simultaneously
				if(copyExpr[i] == inst) {
					inst->copyInfo.gen.set(i);
				}
				else{}
			}
		}
		else{}
	}
}

bool CopyPropagation::translate(Block* block){
	Set tmp = block->copyInfo.in;
	for(list<InterInst*>::iterator i = block->insts.begin(); i != block->insts.end(); i++){
		InterInst* inst = *i;
		Set& in = inst->copyInfo.in;
		Set& out = inst->copyInfo.out;
		in = tmp;
		out = (in - inst->copyInfo.kill) | inst->copyInfo.gen;
		tmp = out;
	}
	bool flag = (tmp != block->copyInfo.out);
	block->copyInfo.out = tmp;
	return flag;
}

void CopyPropagation::analyse(){
	dfg->blocks[0]->copyInfo.out = E;
	for(unsigned int i = 1; i < dfg->blocks.size(); i++){
		dfg->blocks[i]->copyInfo.out = U;
	}
	bool change = true;
	while(change){
		change = false;
		for(unsigned int i = 1; i < dfg->blocks.size(); i++){
			if(dfg->blocks[i]->canReach){
				Set tmp = U;
				list<Block*>::iterator j;
				for(j = dfg->blocks[i]->prevs.begin(); j != dfg->blocks[i]->prevs.end(); j++){
					tmp = tmp & (*j)->copyInfo.out;
				}
				dfg->blocks[i]->copyInfo.in = tmp;
				change = translate(dfg->blocks[i]) ? true : change;
			}
			else{}
		}
	}
}

Var* CopyPropagation::find(Set& in, Var* var){
	if(var){
		for(unsigned int i = 0; i < copyExpr.size(); i++){
			if(in.get(i)){
				Var* rs = copyExpr[i]->getResult();
				Var* arg1 = copyExpr[i]->getArg1();
				if(var == rs){
					if(rs == arg1){
						return var;
					}
					else{
						return find(in, arg1);
					}
				}
				else{}//avoid dead loop for find expr like x = x;
			}
			else{}
		}
		return var;
	}
	else{
		return NULL;
	}
}

void CopyPropagation::propagate(){
	analyse();
	for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
		InterInst* inst = *i;
		Var* rs = inst->getResult();
		Operator op = inst->getOp();
		Var* arg1 = inst->getArg1();
		Var* arg2 = inst->getArg2();
		InterInst* tar = inst->getTarget();
		if(op == OP_SET){
			Var* newRs = find(inst->copyInfo.in, rs);
			inst->replace(op, newRs, arg1);
		}
		else if(op >= OP_AS && op <= OP_GET && op != OP_LEA){
			Var* newArg1 = find(inst->copyInfo.in, arg1);
			Var* newArg2 = find(inst->copyInfo.in, arg2);
			inst->replace(op, rs, newArg1, newArg2);
		}
		else if(op == OP_JT || op == OP_JT || op == OP_ARG || op == OP_RETV){
			Var* newArg1 = find(inst->copyInfo.in, arg1);
			inst->setArg1(newArg1);
		}
		else{}
	}
}
