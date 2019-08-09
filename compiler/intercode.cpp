#include "intercode.h"
#include "genir.h"
#include "symbol.h"

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

void InterInst::toString(){
	if(label != ""){
		printf("%s:\n", label.c_str());
		return;
	}
	else{
		switch(op){
			case OP_DEC: printf("dec "); arg1->value(); break;
			case OP_ENTRY: printf("entry"); break;
			case OP_EXIT: printf("exit"); break;
			case OP_AS: result->value(); printf(" = "); arg1->value(); break;
			case OP_ADD: result->value(); printf(" = "); arg1->value(); printf(" + "); arg2->value(); break;
			case OP_SUB: result->value(); printf(" = "); arg1->value(); printf(" - "); arg2->value(); break;
			case OP_MUL: result->value(); printf(" = "); arg1->value(); printf(" * "); arg2->value(); break;
			case OP_DIV: result->value(); printf(" = "); arg1->value(); printf(" / "); arg2->value(); break;
			case OP_MOD: result->value(); printf(" = "); arg1->value(); printf(" %% "); arg2->value(); break;
			case OP_NEG: result->value(); printf(" = "); printf("-"); arg1->value(); break;
			case OP_GT: result->value(); printf(" = "); arg1->value(); printf(" > "); arg2->value(); break;
			case OP_GE: result->value(); printf(" = "); arg1->value(); printf(" >= "); arg2->value(); break;
			case OP_LT: result->value(); printf(" = "); arg1->value(); printf(" < "); arg2->value(); break;
			case OP_LE: result->value(); printf(" = "); arg1->value(); printf(" <= "); arg2->value(); break;
			case OP_EQU: result->value(); printf(" = "); arg1->value(); printf(" == "); arg2->value(); break;
			case OP_NE: result->value(); printf(" = "); arg1->value(); printf(" != "); arg2->value(); break;
			case OP_NOT: result->value(); printf(" = "); printf("!"); arg1->value(); break;
			case OP_AND: result->value(); printf(" = "); arg1->value(); printf(" && "); arg2->value(); break;
			case OP_OR: result->value(); printf(" = "); arg1->value(); printf(" || "); arg2->value(); break;
			case OP_LEA: result->value(); printf(" = "); printf("&"); arg1->value(); break;
			case OP_SET: printf("*"); arg1->value(); printf(" = "); result->value(); break;
			case OP_GET: result->value(); printf(" = "); printf("*"); arg1->value(); break;
			case OP_JMP: printf("goto %s", target->label.c_str()); break;
			case OP_JT:  printf("if("); arg1->value(); printf(")goto %s", target->label.c_str()); break;
			case OP_JF:  printf("if(!"); arg1->value(); printf(")goto %s", target->label.c_str()); break;
			case OP_JNE: printf("if("); arg1->value(); printf(" != "); arg2->value(); printf(")goto %s",
									 target->label.c_str()); break;
			case OP_ARG: printf("arg "); arg1->value(); break;
			case OP_PROC: printf("%s()", fun->getName().c_str()); break;
			case OP_CALL: result->value(); printf(" = %s()", fun->getName().c_str()); break;
			case OP_RET: printf("return goto %s", target->label.c_str()); break;
			case OP_RETV: printf("return "); arg1->value(); printf(" goto %s", target->label.c_str()); break;
		}
		printf("\n");
	}
}

InterCode::~InterCode(){
	for(int i = 0; i < code.size(); i++){
		delete code[i];
	}
}

void InterCode::addInst(InterInst* inst){
	code.push_back(inst);
}

void InterCode::toString(){
	for(int i = 0; i < code.size(); i++){
		code[i]->toString();
	}
}
