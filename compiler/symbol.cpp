#include "symbol.h"
#include "token.h"
#include "error.h"

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
			//todo
			//name = GenIR::genLb();
			//setArray(strVal.size() + 1);
			break;
	}
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
		//todo
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
	return !ptr;
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
