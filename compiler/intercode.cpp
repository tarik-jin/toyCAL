#include "intercode.h"
#include "genir.h"
#include "symbol.h"

#define emit(fmt, args...) fprintf(file, "\t" fmt "\n", ##args)

FILE* InterInst::file = NULL;

void InterInst::init(){
	op = OP_NOP;
	this->result = NULL;
	this->target = NULL;
	this->arg1 = NULL;
	this->fun = NULL;
	this->arg2 = NULL;
	first = false;
	isDead = false;
}

InterInst::InterInst(string prefix){
	init();
	label = GenIR::genLb(prefix);
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

void InterInst::loadVar(string reg32, string reg8, Var* var){
	if(!var){
		return;
	}
	else{
		const char* reg = var->isChar() ? reg8.c_str() : reg32.c_str();
		if(var->isChar()){
			emit("mov %s, 0", reg32.c_str());
		}
		else{
		}
		const char* name = var->getName().c_str();
		if(var->notConst()){
			int off = var->getOffset();
			if(!off){//glb var
				if(!var->getArray()){
					emit("mov %s, [%s]", reg, name);
				}
				else{
					emit("mov %s, %s", reg, name);
				}
			}
			else{//local var
				if(!var->getArray()){
					emit("mov %s, [ebp%+d]", reg, off);
				}
				else{
					emit("lea %s, [ebp%+d]", reg, off);
				}
			}
		}
		else{//const
			if(var->isBase()){
				emit("mov %s, %d", reg, var->getVal());
			}
			else{
				emit("mov %s, %s", reg, name);
			}
		}
		return;
	}
}

void InterInst::leaVar(string reg32, Var* var){
	if(!var){
		return;
	}
	else{
		const char* reg = reg32.c_str();
		const char* name = var->getName().c_str();
		int off = var->getOffset();
		if(!off){
			emit("mov %s, %s", reg, name);
		}
		else{
			emit("lea %s, [ebp%+d]", reg, off);
		}
		return;
	}
}

void InterInst::storeVar(string reg32, string reg8, Var* var){
	if(!var){
		return;
	}
	else{
		const char* reg = var->isChar() ? reg8.c_str() : reg32.c_str();
		const char* name = var->getName().c_str();
		int off = var->getOffset();
		if(!off){
			emit("mov [%s], %s", name, reg);
		}
		else{
			emit("mov [ebp%+d], %s", off, reg);
		}
		return;
	}
}

void InterInst::initVar(Var* var){
	if(!var){
		return;
	}
	else{
		if(!var->unInit()){
			if(var->isBase()){
				emit("mov eax, %d", var->getVal());
			}
			else{
				emit("mov eax, %s", var->getPtrVal().c_str());
			}
		}
		else{
		}
		return;
	}
}

void InterInst::toX86(){
	if(label != ""){
		fprintf(file, "%s:\n", label.c_str());
		return;
	}
	else{
		switch(op){
			case OP_DEC:
				initVar(arg1);
				break;
			case OP_ENTRY:
				emit("push ebp");
				emit("mov ebp, esp");
				emit("sub esp, %d", getFun()->getMaxDep());
				break;
			case OP_EXIT:
				emit("mov esp, ebp");
				emit("pop ebp");
				emit("ret");
				break;
			case OP_AS:
				loadVar("eax", "al", arg1);
				storeVar("eax", "al", result);
				break;
			case OP_ADD:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("add eax, ebx");
				storeVar("eax", "al", result);
				break;
			case OP_SUB:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("sub eax, ebx");
				storeVar("eax", "al", result);
				break;
			case OP_MUL:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("imul ebx");
				storeVar("eax", "al", result);
				break;
			case OP_DIV:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("idiv ebx");
				storeVar("eax", "al", result);
				break;
			case OP_MOD:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("idiv ebx");
				storeVar("edx", "dl", result);
				break;
			case OP_NEG:
				loadVar("eax", "al", arg1);
				emit("neg eax");
				storeVar("eax", "al", result);
				break;
			case OP_GT:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("mov ecx, 0");
				emit("cmp eax, ebx");
				emit("setg cl");
				storeVar("ecx", "cl", result);
				break;
			case OP_GE:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("mov ecx, 0");
				emit("cmp eax, ebx");
				emit("setge cl");
				storeVar("ecx", "cl", result);
				break;
			case OP_LT:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("mov ecx, 0");
				emit("cmp eax, ebx");
				emit("setl cl");
				storeVar("ecx", "cl", result);
				break;
			case OP_LE:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("mov ecx, 0");
				emit("cmp eax, ebx");
				emit("setle cl");
				storeVar("ecx", "cl", result);
				break;
			case OP_EQU:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("mov ecx, 0");
				emit("cmp eax, ebx");
				emit("sete cl");
				storeVar("ecx", "cl", result);
				break;
			case OP_NE:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("mov ecx, 0");
				emit("cmp eax, ebx");
				emit("setne cl");
				storeVar("ecx", "cl", result);
				break;
			case OP_NOT:
				loadVar("eax", "al", arg1);
				emit("mov ebx, 0");
				emit("cmp eax, 0");
				emit("sete bl");
				storeVar("ebx", "bl", result);
				break;
			case OP_AND:
				loadVar("eax", "al", arg1);
				emit("cmp eax, 0");
				emit("setne al");
				loadVar("ebx", "bl", arg2);
				emit("cmp ebx, 0");
				emit("setne bl");
				emit("and eax, ebx");
				storeVar("eax", "al", result);
				break;
			case OP_OR:
				loadVar("eax", "al", arg1);
				emit("cmp eax, 0");
				emit("setne al");
				loadVar("ebx", "bl", arg2);
				emit("cmp ebx, 0");
				emit("setne bl");
				emit("or eax, ebx");
				storeVar("eax", "al", result);
				break;
			case OP_JMP:
				emit("jmp %s", target->label.c_str());
				break;
			case OP_JT:
				loadVar("eax", "al", arg1);
				emit("cmp eax, 0");
				emit("jne %s", target->label.c_str());
				break;
			case OP_JF:
				loadVar("eax", "al", arg1);
				emit("cmp eax, 0");
				emit("je %s", target->label.c_str());
				break;
			case OP_JNE:
				loadVar("eax", "al", arg1);
				loadVar("ebx", "bl", arg2);
				emit("cmp eax, ebx");
				emit("jne %s", target->label.c_str());
				break;
			case OP_ARG:
				loadVar("eax", "al", arg1);
				emit("push eax");
				break;
			case OP_PROC:
				emit("call %s", fun->getName().c_str());
				emit("add esp, %d", (int)(fun->getParaVar().size() * 4));
				break;
			case OP_CALL:
				emit("call %s", fun->getName().c_str());
				emit("add esp, %d", (int)(fun->getParaVar().size() * 4));
				storeVar("eax", "al", result);
				break;
			case OP_RET:
				emit("jmp %s", target->label.c_str());
				break;
			case OP_RETV:
				loadVar("eax", "al", arg1);
				emit("jmp %s", target->label.c_str());
				break;
			case OP_LEA:
				leaVar("eax", arg1);
				storeVar("eax", "al", result);
				break;
			case OP_SET:
				loadVar("eax", "al", result);
				loadVar("ebx", "bl", arg1);
				emit("mov [ebx], eax");
				break;
			case OP_GET:
				loadVar("eax", "al", arg1);
				emit("mov eax, [eax]");
				storeVar("eax", "al", result);
				break;
		}
		return;
	}
}

Fun* InterInst::getFun(){
	return fun;
}

bool InterInst::isFirst(){
	return first;
}

bool InterInst::isJcond(){
	return op >= OP_JT && op <= OP_JNE;
}

bool InterInst::isJmp(){
	return op == OP_JMP || op == OP_RET || op == OP_RETV;
}

InterInst* InterInst::getTarget(){
	return target;
}

void InterInst::setFirst(){
	first = true;
}

bool InterInst::isDec(){
	return op == OP_DEC;
}

Operator InterInst::getOp(){
	return op;
}

Var* InterInst::getArg1(){
	return arg1;
}

Var* InterInst::getArg2(){
	return arg2;
}

Var* InterInst::getResult(){
	return result;
}

bool InterInst::isExpr(){
	return ((op >= OP_AS) && (op <= OP_OR) || (op == OP_GET));
}

void InterInst::replace(Operator op, Var* rs, Var* arg1, Var* arg2){
	this->op = op;
	this->result = rs;
	this->arg1 = arg1;
	this->arg2 = arg2;
}

void InterInst::replace(Operator op, InterInst* tar, Var* arg1, Var* arg2){
	this->op = op;
	this->target = tar;
	this->arg1 = arg1;
	this->arg2 = arg2;
}

void InterInst::callToProc(){
	this->result = NULL;
	this->op = OP_PROC;
}

InterCode::~InterCode(){
	for(int i = 0; i < code.size(); i++){
		delete code[i];
	}
}

void InterInst::setArg1(Var* arg1){
	this->arg1 = arg1;
}

void InterCode::addInst(InterInst* inst){
	code.push_back(inst);
}

void InterCode::toString(){
	for(int i = 0; i < code.size(); i++){
		code[i]->toString();
	}
}

vector<InterInst*>& InterCode::getCode(){
	return code;
}

void InterCode::markFirst(){
	unsigned int len = code.size();
	code[0]->setFirst();
	code[len - 1]->setFirst();
	code[1]->setFirst();
	for(unsigned int i = 1; i < len - 1; ++i){
		if(code[i]->isJmp() || code[i]->isJcond()){
			code[i]->getTarget()->setFirst();
			code[i + 1]->setFirst();
		}
		else{
		}
	}
}
