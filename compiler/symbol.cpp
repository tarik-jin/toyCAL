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
	else{
		size = 1;
	}
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

Fun::Fun(bool ext, Tag t, string n, vector<Var*>& paraList){
	externed = ext;
	type = t;
	name = n;
	paraVar = paraList;
	//to do
}

Fun::~Fun(){
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
