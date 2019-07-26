#include "symtab.h"
#include "error.h"

#define SEMERROR(code, name) Error::semError(code, name)

SymTab::SymTab(){

	scopeId = 0;
	curFun = NULL;
	scopePath.push_back(0);

}

SymTab::~SymTab(){
	//clear fun
	hash_map<string, Fun*, string_hash>::iterator funIt, funEnd;
	funEnd = funTab.end();
	for(funIt = funTab.begin(); funIt != funEnd; ++funIt){
		delete funIt->second;
	}
	//clear var
	hash_map<string, vector<Var*>*, string_hash>::iterator varIt, varEnd;
	varEnd = varTab.end();
	for(varIt = varTab.begin(); varIt != varEnd; ++varIt){
		vector<Var*>& list = *(varIt->second);
		for(int i = 0; i < lise.size(); i++){
			delete list[i];
		}
		delete &list;
	}
	//clear str
	hash_map<string, Var*, string_hash>::iterator strIt, strEnd;
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
