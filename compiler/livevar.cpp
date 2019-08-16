#include "livevar.h"
#include "dfg.h"
#include "symtab.h"
#include "intercode.h"
#include "symbol.h"

LiveVar::LiveVar(DFG* g, SymTab* t, vector<Var*>& paraVar):dfg(g), tab(t){
	varList = tab->getGlbVars();
	int glbNum = varList.size();
	for(unsigned int i = 0; i < paraVar.size(); i++){
		varList.push_back(paraVar[i]);
	}
	dfg->toCode(optCode);
	for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
		InterInst* inst = *i;
		Operator op = inst->getOp();
		if(op == OP_DEC){
			varList.push_back(inst->getArg1());
		}
		else{}
	}
	U.init(varList.size(), 1);
	E.init(varList.size(), 0);
	G = E;
	for(int i = 0; i < glbNum; i++){
		G.set(i);
	}
	for(unsigned int i = 0; i < varList.size(); i++){
		varList[i]->index = i;
	}
	for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
		InterInst* inst = *i;
		inst->liveInfo.use = E;
		inst->liveInfo.def = E;
		Var* rs = inst->getResult();
		Operator op = inst->getOp();
		Var* arg1 = inst->getArg1();
		Var* arg2 = inst->getArg2();
		if(op >= OP_AS && op <= OP_LEA){
			inst->liveInfo.use.set(arg1->index);
			if(arg2){
				inst->liveInfo.use.set(arg2->index);
			}
			else{}
			if(rs != arg1 && rs != arg2){
				inst->liveInfo.def.set(rs->index);
			}
			else{}
		}
		else if(op == OP_SET){
			inst->liveInfo.use.set(rs->index);
		}
		else if(op == OP_GET){
			inst->liveInfo.use = U;
		}
		else if(op == OP_RETV){
			inst->liveInfo.use.set(arg1->index);
		}
		else if(op == OP_ARG){
			if(arg1->isBase()){
				inst->liveInfo.use.set(arg1->index);
			}
			else{
				inst->liveInfo.use = U;
			}
		}
		else if(op == OP_CALL || op == OP_PROC){
			inst->liveInfo.use = G;
			if(rs && rs->getPath().size() > 1){//we have assumed that fun call use all glbvar
				inst->liveInfo.def.set(rs->index);
			}
			else{}
		}
		else if(op == OP_JF || op == OP_JT){
			inst->liveInfo.use.set(arg1->index);
		}
		else{}
	}
}

bool LiveVar::translate(Block* block){
	Set tmp = block->liveInfo.out;
	for(list<InterInst*>::reverse_iterator i = block->insts.rbegin(); i != block->insts.rend(); i++){
		InterInst* inst = *i;
		if(!inst->isDead){
			Set& in = inst->liveInfo.in;
			Set& out = inst->liveInfo.out;
			out = tmp;
			in = inst->liveInfo.use | (out - inst->liveInfo.def);
			tmp = in;
		}
		else{}
	}
	bool flag = (tmp != block->liveInfo.in);
	block->liveInfo.in = tmp;
	return flag;
}

void LiveVar::analyse(){
	dfg->blocks[dfg->blocks.size() - 1]->liveInfo.in = E;
	for(unsigned int i = 0; i < dfg->blocks.size() - 1; i++){
		dfg->blocks[i]->liveInfo.in = E;
	}
	bool change = true;
	while(change){
		change = false;
		for(int i = dfg->blocks.size() - 2; i >= 0; i--){
			if(dfg->blocks[i]->canReach){
				Set tmp = E;
				list<Block*>::iterator j;
				for(j = dfg->blocks[i]->succs.begin(); j != dfg->blocks[i]->succs.end(); j++){
					tmp = tmp | (*j)->liveInfo.in;
				}
				dfg->blocks[i]->liveInfo.out = tmp;
				change = translate(dfg->blocks[i]) ? true : change;
			}
			else{}
		}
	}
}

void LiveVar::elimateDeadCode(bool stop){
	if(stop){
		for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
			InterInst* inst = *i;
			Operator op = inst->getOp();
			if(!inst->isDead && op != OP_DEC){
				Var* rs = inst->getResult();
				Var* arg1 = inst->getArg1();
				Var* arg2 = inst->getArg2();
				if(rs) {rs->live = true;} else{}
				if(arg1) {arg1->live = true;} else{}
				if(arg2) {arg2->live = true;} {}
			}
			else{}//skip dead inst or op_dec
		}
		for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
			InterInst* inst = *i;
			Operator op = inst->getOp();
			if(op == OP_DEC){
				Var* arg1 = inst->getArg1();
				inst->isDead = !arg1->live ? true : inst->isDead;
			}
			else{}//skip other inst
		}
		return;
	}
	else{
		stop = true;
		analyse();
		for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
			InterInst* inst = *i;
			if(!inst->isDead){
				Var* rs = inst->getResult();
				Operator op = inst->getOp();
				Var* arg1 = inst->getArg1();
				Var* arg2 = inst->getArg2();
				if(op >= OP_AS && op <= OP_LEA || op == OP_GET){
					if(rs->getPath().size() != 1){
						if(!inst->liveInfo.out.get(rs->index) || (op == OP_AS && rs == arg1)){
							inst->isDead = true;
							stop = false;
						}
						else{}
					}
					else{}//skip glbVars
				}
				else if(op == OP_CALL){
					if(!inst->liveInfo.out.get(rs->index)){
						inst->callToProc();
					}
					else{}
				}
				else{}
			}
			else{}//skip dead inst
		}
	}
	elimateDeadCode(stop);
}

Set& LiveVar::getE(){
	return E;
}

vector<Var*> LiveVar::getCoVar(Set liveout){
	vector<Var*> coVar;
	for(unsigned int i = 0; i < varList.size(); i++){
		if(liveout.get(i)){
			coVar.push_back(varList[i]);
		}
		else{}
	}
	return coVar;
}
