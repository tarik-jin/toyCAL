#include "symtab.h"
#include "error.h"
#include "symbol.h"

#define SEMERROR(code, name) Error::semError(code, name)

SymTab::SymTab(){

	scopeId = 0;
	curFun = NULL;
	scopePath.push_back(0);

}

SymTab::~SymTab(){
	//clear fun
	unordered_map<string, Fun*, string_hash>::iterator funIt, funEnd;
	funEnd = funTab.end();
	for(funIt = funTab.begin(); funIt != funEnd; ++funIt){
		delete funIt->second;
	}
	//clear var
	unordered_map<string, vector<Var*>*, string_hash>::iterator varIt, varEnd;
	varEnd = varTab.end();
	for(varIt = varTab.begin(); varIt != varEnd; ++varIt){
		vector<Var*>& list = *(varIt->second);
		for(int i = 0; i < list.size(); i++){
			delete list[i];
		}
		delete &list;
	}
	//clear str
	unordered_map<string, Var*, string_hash>::iterator strIt, strEnd;
	strEnd = strTab.end();
	for(strIt = strTab.begin(); strIt != strEnd; ++strIt){
		delete strIt->second;
	}
}

void SymTab::enter() {
	scopeId++;
	scopePath.push_back(scopeId);
	if(curFun){
		curFun->enterScope();
	}
	else{
	}
	return;
}

void SymTab::leave(){
	scopePath.pop_back();
	if(curFun){
		curFun->leaveScope();
	}
	else{
	}
}

vector<int>& SymTab::getScopePath(){
	return scopePath;
}

void SymTab::addVar(Var* var){
	if(varTab.find(var->getName()) == varTab.end()){
		varTab[var->getName()] = new vector<Var*>;
		varTab[var->getName()]->push_back(var);
	}
	else{
		vector<Var*>& list = *varTab[var->getName()];
		int i = 0;
		while((i < list.size()) &&
				(list[i]->getPath().back() != var->getPath().back())){
			i++;
		}
		if(i == list.size() || var->getName()[0] == '<'){ //constant var
			list.push_back(var);
		}
		else{
			SEMERROR(VAR_RE_DEF, var->getName());
			delete var;
			return;
		}
	}
	/*
	//todo
	if(ir){
		bool flag = ir->genVarInit(var);
		if(curFun && flag){
			curFun->locate(var);
		}
		else{
		}
	}
	else{
	}
	*/
	return;
}

void SymTab::addStr(Var* v){
	unordered_map<string, Var*, string_hash>::iterator strIt, strEnd;
	strEnd = strTab.end();
	for(strIt = strTab.begin(); strIt != strEnd; ++strIt){
		Var* str = strIt->second;
		if(v->getStrVal() == str->getStrVal()){
			delete v;
			v = str;
			return;
		}
		else{
		}
	}
	strTab[v->getName()] = v;
	return;
}

Var* SymTab::getVar(string name){
	Var* select = NULL;
	if(varTab.find(name) != varTab.end()){
		vector<Var*>& list = *varTab[name];
		int pathLen = scopePath.size();
		int maxLen = 0;
		for(int i = 0; i < list.size(); i++){
			int len = list[i]->getPath().size();
			if(len <= pathLen &&
					list[i]->getPath()[len - 1] == scopePath[len - 1]){
				if(len > maxLen){
					maxLen = len;
					select = list[i];
				}
				else{
				}

			}
			else{
			}
		}

	}
	else{
		select = NULL;
	}
	if(!select){
		SEMERROR(VAR_UN_DEC, name);
	}
	else{
	}
	return select;
}
