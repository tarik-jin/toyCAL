#include "intercode.h"
#include "genir.h"

void InterInst::init(){
	op = OP_NOP;
	this->result = NULL;
	this->target = NULL;
	this->arg1 = NULL;
	this->fun = NULL;
	this->arg2 = NULL;
}

InterInst::InterInst(){
	init();
	label = GenIR::genLb();
}

InterInst::InterInst(Operator op, Var* arg1){
	init();
	this->op = op;
	this->result = NULL;
	this->arg1 = arg1;
	this->arg2 = NULL;
}

InterInst::InterInst(Operator op, Fun* fun, Var* rs){
	init();
	this->op = op;
	this->result = rs;
	this->fun = fun;
	this->arg2 = NULL;
}

InterInst::InterInst(Operator op, Var* rs, Var* arg1, Var* arg2){
	init();
	this->op = op;
	this->result = rs;
	this->arg1 = arg1;
	this->arg2 = arg2;
}

InterInst::InterInst(Operator op, InterInst* tar, Var* arg1, Var* arg2){
	init();
	this->op = op;
	this->target = tar;
	this->arg1 = arg1;
	this->arg2 = arg2;
}


InterCode::~InterCode(){
	for(int i = 0; i < code.size(); i++){
		delete code[i];
	}
}

void InterCode::addInst(InterInst* inst){
	code.push_back(inst);
}
