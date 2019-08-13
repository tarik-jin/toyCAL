#include "consprop.h"

ConstPropagation::ConstPropagation(DFG* g, SymTab* t, vector<Var*>& paraVar):dfg(g), tab(t){
	vector<Var*> glbVars = tab->getGlbVars();
	int index = 0;
	//glbVars
	for(unsigned int i = 0; i < glbVars.size(); i++){
		Var* var = glbVars[i];
		var->index = index++;
		vars.push_back(var);
		double val;
		if(!var->isBase()){
			val = NAC;
		}
		else if(!var->unInit()){
			val = var->getVal();
		}
		else{
			val = 0;
		}
		boundVals.push_back(val);
	}
	//parameter vars
	for(unsigned int i = 0; i < paraVar.size(); i++){
		Var *var = paraVar[i];
		var->index = index++;
		vars.push_back(var);
		boundVals.push_back(NAC);
	}
	//local vars
	for(unsigned int i = 0; i < dfg->codeList.size(); i++){
		if(dfg->codeList[i]->isDec()){
			Var* var = dfg->codeList[i]->getArg1();
			var->index = index++;
			vars.push_back(var);
			double val;
			if(!var->isBase()){
				val = NAC;
			}
			else if(!var->unInit()){
				val = val->getVal();
			}
			else{
				val = UNDEF;
			}
			boundVals.push_back(val);
		}
	}
	while(index--){
		initVals.push_back(UNDEF);
	}
}

double ConstPropagation::join(double left, double right){
	if(left == NAC || right == NAC){
		return NAC;
	}
	else if(left == UNDEF){
		return right;
	}
	else if(right == UNDEF){
		return left;
	}
	else{
		return NAC;
	}
}

double ConstPropagation::join(Block* block){
	list<Block*>& prevs = block->prevs;
	vector<double>& in = block->inVals;
	for(unsigned int i = 0; i < in.size(); i++){
		double val = UNDEF;
		for(list<Bloc*>::iterator j = prevs.begin(); j != prevs.end(); ++j){
			val = join(val, (*j)->outVals[i]);
		}
		in[i] = val;
	}
}

void ConstPropagation::translate(InterInst* inst, vector<double>& in, vector<double>& out){

	out = in;
	Operator op = inst->getOp();
	Var* result = inst->getResult();
	Var* arg1 = inst->getArg1();
	Var* arg2 = inst->getArg2();
	if(inst->isExpr()){
		double tmp;
		if(op == OP_AS || op == OP_NEG || op == OP_NOT){
			if(arg1->isLiteral()){
				tmp = arg1->getVal();
			}
			else{
				tmp = in[arg1->index];
			}
			if(tmp != UNDEF && tmp != NAC){
				if(op == OP_NEG){
					tmp = -tmp;
				}
				else if(op == OP_NOT){
					tmp = !tmp;
				}
				else{
					//OP_AS
				}
			}
			else{
				//tmp is UNDEF || NAC
			}
		}
		else if(OP >= OP_ADD && OP <= OP_OR){
			double lp, rp;

			if(arg1->isLiteral()){
				lp = arg1->getVal();
			}
			else{
				lp = in[arg1->index];
			}
			if(arg2->isLiteral()){
				rp = arg2->getVal();
			}
			else{
				rp = in[arg2->index];
			}

			if(lp == NAC || rp == NAC){
				tmp = NAC;
			}
			else if(lp == UNDEF || rp == UNDEF){
				tmp = UNDEF;
			}
			else{
				int left = lp, right = rp;
				if(op == OP_ADD) tmp = left + right;
				else if(op == OP_SUB) tmp = left - right;
				else if(op == OP_MUL) tmp = left * right;
				else if(op == OP_DIV) {
					if(!right){
						tmp = NAC;
					}
					else{
						tmp = left / right;
					}
				}
				else if(op == OP_MOD){
					if(!right){
						tmp = NAC;
					}
					else{
						tmp = left % right;
					}
				}
				else if(op == OP_GT) tmp = left > right;
				else if(op == OP_GE) tmp = left >= right;
				else if(op == OP_LT) tmp = left < right;
				else if(op == OP_LE) tmp = left <= right;
				else if(op == OP_EQU) tmp = left == right;
				else if(op == OP_NE) tmp = left != right;
				else if(op == OP_AND) tmp = left && right;
				else if(op == op_OR) tmp = left || right;
				else{
				}
			}
		}
		else if(op == OP_GET){
			tmp = NAC;
		}
		out[result->index] = tmp;
	}
	else if(op == SET || op == OP_ARGS && !arg1->isBase()){
		for(unsigned int i = 0; i < out.size(); i++){
			out[i] = NAC;
		}
	}
	else if(op == OP_PROC){
		for(unsigned int i =0; i < glbVars.size(); i++){
			out[glbVars[i]->index] = NAC;
		}
	}
	else if(op == OP_CALL){
		for(unsigned int i = 0; i < glbVars.size(); i++){
			out[glbVars[i]->index] = NAC;
		}
		out[result->index] = NAC;
	}
	else{
	}

	inst->inVals = in;
	inst->outVals = out;

}

bool ConstPropagation::translate(Block* block){
	vector<double> in = block->inVals;
	vector<double> out = in;
	for(list<InterInst*>::iterator i = block->insts.begin(); i != block->insts.end(); i++){
		InterInst* inst = *i;
		translate(inst, in, out);
		in = out;
	}
	bool flag;
	unsigned int i = 0, size = out.size();
	while((i < size) && (block->outVals[i] == out[i])){i++;}
	flag = i < size ? true : false;
	block->outVals = out;
	return flag;
}

void ConstPropagation::analyse(){
	dfg->blocks[0]->outVals = boundVals;
	for(unsigned int i = 1; i < dfg->blocks.size(); i++){
		dfg->blocks[i]->outVals = initVals;
	}
	bool outChange = true;
	while(outChange){
		outChange = false;
		for(unsigned int i = 1; i < dfg->blocks.size(); i++){
			join(dfg->blocks[i]);
			if(translate(dfg->blocks[i])){
				outChange = true;
			}
			else{
			}
		}
	}
}

void ConstPropagation::algebraSimlify(){
	for(unsigned int j = 0; j < dfg->blocks.size(); j++){
		list<InterInst*>::iterator i;
		for(i = dfg->blocks[j]->insts.begin(); i!= dfg->blocks[j]->insts.end(); i++){
			InterInst* inst = *i;
			Operator op = inst->getOp();
			if(inst->isExpr()){
				double rs;
				Var* result = inst->getResult();
				Var* arg1 = inst->getArg1();
				Var* arg2 = inst->getArg2();
				rs = inst->outVals[result->index];
				if(rs != UNDEF && rs != NAC){
					Var* newVar = new Var((int)rs);
					tab->addVar(new Var);
					inst->replace(OP_AS, result, newVar);
				}
				else if(op >= OP_ADD && OP <= OP_OR && !(OP == OP_AS || op == OP_NEG || op == OP_NOT)){
					double lp, rp;
					if(arg1->isLiteral()){
						lp = arg1->getVal();
					}
					else{
						lp = inst->inVals[arg1->index];
					}
					if(arg2->isLiteral()){
						rp = arg2->getVal();
					}
					else{
						rp = inst->inVals[arg2->index];
					}
					int left, right;
					bool dol = false, dor = false;
					if(lp != UNDEF && lp != NAC){
						left = lp;
						dol = true;
					}
					else if(rp != UNDEF && rp != NAC){
						right = rp;
						dor = true;
					}
					else{
						continue;
					}

					Var* newArg1 = NULL;
					Var* newArg2 = NULL;
					Operator newOp = OP_AS;
					if(op == OP_ADD){
						if(dol && left == 0){newArg1 = arg2;}else{}
						if(dor && right == 0){newArg2 = arg1;}else{}
					}
					else if(op == OP_SUB){
						if(dol && left == 0){newOp = OP_NEG; newArg1 = arg2;}else{}
						if(dor && right == 0){newArg1 = arg1;}else{}
					}
					else if(op == OP_MUL){
						if((dol && left == 0) || (dor && right == 0)){
							newArg1 = SymTab::zero;
						}
						else{}
						if(dol && left == 1){newArg1 = arg2;}else{}
						if(dor && right == 1){newArg2 = arg1;}else{}
					}
					else if(op == OP_DIV){
						if(dol && left == 0){newArg1 = SymTab::zero;}else{}
						if(dor && right == 1){newArg1 = arg1;}else{}
					}
					else if(op == OP_MOD){
						if((dol && left == 0) || (dor && right == 1)){newArg1 = SymTab::zero;}else{}
					}
					else if(op == OP_AND){
						if((dol && left == 0) || (dor && right == 0)){newArg1 = SymTab::zero;}else{}
						if(dol && left != 0){
							newOp = OP_NE;
							newArg1 = arg2;
							newArg2 = SymTab::zero;
						}
						else{}
						if(dor && right != 0){
							newOp = OP_NE;
							newArg1 = arg1;
							newArg2 = SymTab::zero;
						}
						else{}
					}
					else if(op == OP_OR){
						if((dol && left != 0) || (dor && right != 0)){newArg1 = SymTab::one;}else{}
						if(dol && left == 0){
							newOp = OP_NE;
							newArg1 = arg2;
							newArg2 = symTab::zero;
						}
						else{}
						if(dor && right == 0){
							newOp = OP_NE;
							newArg1 = arg1;
							newArg2 = SymTab::zero;
						}
						else{}
					}
					else{}//no other expr operator need to handle
					if(newArg1){
						inst->replace(newOp, result, newArg1, newArg2);
					}
					else{
						if(dol){
							newArg1 = new Var(left);
							tab->addVar(newArg1);
							newArg2 = arg2;
						}
						else if(dor){
							newArg2 = new Var(right);
							tab->addVar(newArg2);
							newArg1 = arg1;
						}
						else{}
						inst->replace(op, result, newArg1, newArg2);
					}
				}
			}
			else if(op == OP_ARG || op == OP_RETV){
				Var* arg1 = inst->getArg1();
				if(!arg1->isLiteral()){
					double rs = inst->outVals[arg1->index];
					if(rs != UNDEF && rs != NAC){
						Var* newVar = new Var((int)rs);
						tab->addVar(newVar);
						inst->setArg1(newVar);
					}
					else{
					}
				}
				else{
					//arg1 is constant
				}
			}
			else{}//no other operator
		}//end of insts loop
	}//end of blocks loop
}

void ConstPropagation::condJmpOpt(){
	for(unsigned int j = 0; j < dfg->blocks.size(); j++;){
		list<InterInst*>::iterator i, k;
		i = dfg->blocks[j]->insts.begin();
		k = i;
		for(; i != dfg->blocks[j]->insts.end(); i = k){
			++k;
			InterInst* inst = *i;
			if(inst->isJcond()){
				Operator op = inst->getOp();
				InterInst* tar = inst->getTarget();
				Var* arg1 = inst->getArg1();
				double cond = arg1->isLiteral ? arg1->getVal() : inst->inVals[arg1->index];
				if(cond != NAC && cond != UNDEF){
					if((op == OP_JT && cond == 0) || (op == OP_JF && cond != 0)){
						inst->block->insts.remove(inst);
						if(dfg->blocks[j + 1] != tar->block){
							dfg->delLink(inst->block, tar->block);
						}
						else{}
					}
					else if((op == OP_JT && cond != 0) || (op == OP_JF && cond == 0)){
						inst->replace(OP_JMP, tar);
						if(dfg->blocks[j + 1] != tar->block){
							dfg->delLink(inst->block, dfg->blocks[j + 1]);
						}
						else{}
					}
				}
				else{}//do nothing, just continue
			}
			else{}//only optimize jmp(cond) inst
		}
	}
}
