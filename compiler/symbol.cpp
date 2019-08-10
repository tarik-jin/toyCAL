#include "symbol.h"
#include "token.h"
#include "error.h"
#include "genir.h"
#include "symtab.h"

#define SEMERROR(code, name) Error::semError(code, name)

void Var::clear(){
	scopePath.push_back(-1);
	externed = false;
	isPtr = false;
	isArray = false;
	isLeft = true;
	inited = false;
	literal = false;
	size = 0;
	offset = 0;
	ptr = NULL;
	return;
}

Var::Var(){
	clear();
	setName("<void>");
	setLeft(false);
	intVal = 0;
	literal = false;
	type = KW_VOID;
	isPtr = true;
}

Var::Var(int val){
	clear();
	setName("<int>");
	literal = true;
	setLeft(false);
	setType(KW_INT);
	intVal = val;
}

Var::Var(Token* lt){
	clear();
	literal = true;
	setLeft(false);
	switch(lt->tag){
		case NUM:
			setType(KW_INT);
			name = "<int>";
			intVal = ((Num*)lt)->val;
			break;
		case CH:
			setType(KW_CHAR);
			name = "<char>";
			intVal = 0;
			charVal = ((Char*)lt)->ch;
			break;
		case STR:
			setType(KW_CHAR);
			name = GenIR::genLb();
			strVal = ((Str*)lt)->str;
			setArray(strVal.size() + 1);
			break;
	}
}

Var::Var(vector<int>& sp, Var* v){
		clear();
		scopePath = sp;
		setType(v->type);
		setPtr(v->isPtr || v->isArray);
		setName("");
		setLeft(false);
}

Var::Var(vector<int>& sp, Tag t, bool ptr){
	clear();
	scopePath = sp;
	setType(t);
	setPtr(ptr);
	setName("");
	setLeft(false);
}

Var::Var(vector<int>& sp, bool ext, Tag t, string name, int len){
	clear();
	scopePath = sp;
	setExtern(ext);
	setType(t);
	setName(name);
	setArray(len);
}

Var::Var(vector<int>& sp, bool ext, Tag t, bool ptr, string name, Var* init){
	clear();
	scopePath = sp;
	setExtern(ext);
	setType(t);
	setPtr(ptr);
	setName(name);
	initData = init;
}

void Var::setExtern(bool ext){
	externed = ext;
	size = 0;
}

void Var::setType(Tag t){
	type = t;
	if(type == KW_VOID){
		SEMERROR(VOID_VAR, "");
		type = KW_INT;
	}
	else{
	}
	if(!externed && type == KW_INT){
		size = 4;
	}
	else if(!externed && type == KW_CHAR) {
		size = 1;
	}
	else{
	}
	return;
}

void Var::setArray(int len){
	if(len <= 0){
		SEMERROR(ARRAY_LEN_INVALID, name);
		return;
	}
	else{
		isArray = true;
		isLeft = false;
		arraySize = len;
		if(!externed){
			size *= len;
		}
		else{
		}
		return;
	}
}

void Var::setPtr(bool ptr){
	if(!ptr){
		return;
	}
	else{
		isPtr = true;
		if(!externed){
			size = 4;
		}
		else{
		}
		return;
	}
}

void Var::setName(string n){
	if(n == ""){
		n = GenIR::genLb();
	}
	else{
	}
	name = n;
	return;
}

vector<int>& Var::getPath(){
	return scopePath;
}

string Var::getName(){
	return name;
}

string Var::getStrVal(){
	return strVal;
}

void Var::setLeft(bool lf){
	isLeft = lf;
	return;
}

void Var::setOffset(int off){
	offset = off;
}

int Var::getSize(){
	return size;
}

Tag Var::getType(){
	return type;
}

bool Var::setInit(){
	Var* init = initData;
	if(!init){
		return false;
	}
	else{
		inited = false;
		if(externed){
			SEMERROR(DEC_INIT_DENY, name);
		}
		else if(!GenIR::typeCheck(this, init)){//type compatible test
			SEMERROR(VAR_INIT_ERR, name);
		}
		else if(init->literal){
			inited = true;
			if(init->isArray){
				ptrVal = init->name;
			}
			else{
				intVal = init->intVal;
			}
		}
		else{//init val is not literal
			if(scopePath.size() == 1){//global var
				SEMERROR(GLB_INIT_ERR, name);
			}
			else{
				inited = true;
			}
		}
		return inited;
	}
}

Var* Var::getInitData(){
	return initData;
}

bool Var::isVoid(){
	return type == KW_VOID;
}

bool Var::getLeft(){
	return isLeft;
}

bool Var::isBase(){
	return !isArray && !isPtr;
}

bool Var::isRef(){
	return !!ptr;
}

Var* Var::getStep(Var* v){
	if(v->isBase()){
		return SymTab::one;
	}
	else{
		if(v->type == KW_CHAR){
			return SymTab::one;
		}
		else if(v->type == KW_INT){
			return SymTab::four;
		}
		else{
			return NULL;
		}
	}
}

void Var::setPointer(Var* p){
	ptr = p;
}

Var* Var::getTrue(){
	return SymTab::one;
}

Var* Var::getPointer(){
	return ptr;
}

Var* Var::getVoid(){
	return SymTab::voidVar;
}

int Var::getVal(){
	return intVal;
}

void Var::toString(){
	if(externed){
		printf("externed ");
	}
	else{
	}
	printf("%s ", tokenName[type]);

	if(isPtr){
		printf("*");
	}
	else{
	}
	printf("%s", name.c_str());

	if(isArray){
		printf("[%d]", arraySize);
	}
	else{
	}

	if(inited){
		printf(" = ");
		switch(type){
			case KW_INT:
				printf("%d", intVal);
				break;
			case KW_CHAR:
				if(isPtr){
					printf("<%s>", ptrVal.c_str());
				}
				else{
					printf("%c", charVal);
				}
				break;
		}
	}
	printf("; size = %d scope=\"", size);
	for(int i = 0; i < scopePath.size(); i++){
		printf("/%d", scopePath[i]);
	}
	printf("\" ");

	if(offset > 0){
		printf("addr = [ebp + %d]", offset);
	}
	else if(offset < 0){
		printf("addr = [ebp %d]", offset);
	}
	else if(name[0] != '<'){
		printf("addr = <%s>", name.c_str());//glb var
	}
	else{
		printf("value = '%d'", getVal());//constant
	}
	return;
}

void Var::value(){
	if(literal){
		if(type == KW_INT){
			printf("%d", intVal);
		}
		else if(type == KW_CHAR){
			if(isArray){
				printf("%s", name.c_str());
			}
			else{
				printf("%d", charVal);
			}
		}
		else{
		}
	}
	else{
		printf("%s", name.c_str());
	}
}

bool Var::isChar(){
	return (type == KW_CHAR) && isBase();
}

bool Var::notConst(){
	return !literal;
}

int Var::getOffset(){
	return offset;
}

bool Var::getArray(){
	return isArray;
}

bool Var::unInit(){
	return !inited;
}

string Var::getPtrVal(){
	return ptrVal;
}

Fun::Fun(bool ext, Tag t, string n, vector<Var*>& paraList){
	externed = ext;
	type = t;
	name = n;
	paraVar = paraList;
	curEsp = 0;
	maxDepth = 0;
	for(int i = 0, argOff = 8; i < paraVar.size(); i++, argOff += 4){
		paraVar[i]->setOffset(argOff);
	}
}

Fun::~Fun(){
}

#define SEMWARN(code, name) Error::semWarn(code, name)

bool Fun::match(Fun *f){
	if((name != f->name) ||
			(paraVar.size() != f->paraVar.size())){
		return false;
	}
	else{
		int len = paraVar.size();
		for(int i = 0; i < len; i++){
			if(GenIR::typeCheck(paraVar[i], f->paraVar[i])){//compatible
				if(paraVar[i]->getType() != f->paraVar[i]->getType()){
					SEMWARN(FUN_DEC_CONFLICT, name);
				}
				else{
					//var compatible && same type
				}
			}
			else{
				return false;
			}
		}
		if(type != f->type){
			SEMWARN(FUN_RET_CONFLICT, name);
		}
		else{
		}
		return true;
	}
}

bool Fun::match(vector<Var*>& args){
	if(paraVar.size() != args.size()){
		return false;
	}
	else{
		int len = paraVar.size();
		for(int i = 0; i < len; i++){
			if(!GenIR::typeCheck(paraVar[i], args[i])){//type check incompatible
				return false;
			}
			else{
			}
		}
		return true;
	}
}

void Fun::define(Fun* def){
	externed = false;
	paraVar = def->paraVar;
	return;
}

void Fun::enterScope(){
	scopeEsp.push_back(0);
	return;
}

void Fun::leaveScope(){
	maxDepth = (curEsp > maxDepth) ? curEsp : maxDepth;
	curEsp -= scopeEsp.back();
	scopeEsp.pop_back();
	return;
}

void Fun::locate(Var* var){
	int size = var->getSize();
	size += (4 - size % 4) % 4;
	scopeEsp.back() += size;
	curEsp += size;
	var->setOffset(-curEsp);
	return;
}

void Fun::setExtern(bool ext){
	externed = ext;
	return;
}

bool Fun::getExtern(){
	return externed;
}

string& Fun::getName(){
	return name;
}

void Fun::addInst(InterInst* inst){
	interCode.addInst(inst);
	return;
}

Tag Fun::getType(){
	return type;
}

InterInst* Fun::getReturnPoint(){
	return returnPoint;
}

void Fun::setReturnPoint(InterInst* inst){
	returnPoint = inst;
}

void Fun::toString(){
	printf("%s", tokenName[type]);
	printf(" %s", name.c_str());

	printf("(");
	for(int i = 0; i < paraVar.size(); i++){
		printf("<%s>", paraVar[i]->getName().c_str());
		if(i != paraVar.size() - 1){
			printf(",");
		}
		else{
		}
	}
	printf(")");

	if(externed){
		printf(";\n");
	}
	else{
		printf(":\n");
		printf("\t\tmaxDepth = %d\n", maxDepth);
	}
}

void Fun::printInterCode(){
	if(externed){
		return;
	}
	else{
		printf("-------<%s>Start-------\n", name.c_str());
		interCode.toString();
		printf("-------<%s>End-------\n", name.c_str());
	}
}

int Fun::getMaxDep(){
	return maxDepth;
}

vector<Var*>& Fun::getParaVar(){
	return paraVar;
}

void Fun::genAsm(FILE* file){
	if(externed){
		return;
	}
	else{
		vector<InterInst*> code;
		if(Args::opt){
		}
		else{
			code = interCode.getCode();
		}
		const char* pName = name.c_str();
		fprintf(file, "#fun:%s code\n", pName);
		fprintf(file, "\t.global %s\n", pName);
		fprintf(file, "%s:", pName);
	}
}
