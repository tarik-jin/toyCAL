#include "genir.h"
#include "symtab.h"
#include "error.h"
#include "symbol.h"
#include "compiler.h"

#define SEMERROR(code, name) Error::semError(code, name)

Var* SymTab::voidVar = NULL;
Var* SymTab::zero = NULL;
Var* SymTab::one = NULL;
Var* SymTab::four = NULL;

SymTab::SymTab(){

	voidVar = new Var();
	zero = new Var(0);
	one = new Var(1);
	four = new Var(4);
	addVar(voidVar);
	addVar(zero);
	addVar(one);
	addVar(four);

	scopeId = 0;
	curFun = NULL;
	ir = NULL;
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
		varList.push_back((var->getName()));
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

void SymTab::decFun(Fun* fun){
	fun->setExtern(true);
	if(funTab.find(fun->getName()) == funTab.end()){
		funTab[fun->getName()] = fun;
	}
	else{
		Fun* last = funTab[fun->getName()];
		if(!last->match(fun)){
			SEMERROR(FUN_DEC_ERR, fun->getName());
		}
		else{
		}
		delete fun;
	}
	return;
}

void SymTab::defFun(Fun* fun){
	if(fun->getExtern()){
		SEMERROR(EXTERN_FUN_DEF, fun->getName());
		fun->setExtern(false);
	}
	else{
	}
	if(funTab.find(fun->getName()) == funTab.end()){
		funTab[fun->getName()] = fun;
		funList.push_back(fun->getName());
	}
	else{
		Fun* last = funTab[fun->getName()];
		if(last->getExtern()){
			if(!last->match(fun)){
				SEMERROR(FUN_DEC_ERR, fun->getName());
			}
			else{
			}
			last->define(fun);
		}
		else{
			SEMERROR(FUN_RE_DEF, fun->getName());
		}
		delete fun;
		fun = last;
	}
	curFun = fun;
	ir->genFunHead(curFun);
	return;
}

void SymTab::endDefFun(){
	ir->genFunTail(curFun);
	curFun = NULL;
	return;
}

Fun* SymTab::getFun(string name, vector<Var*>& args){
	if(funTab.find(name) != funTab.end()){
		Fun* last = funTab[name];
		if(!last->match(args)){
			SEMERROR(FUN_CALL_ERR, name);
			return NULL;
		}
		else{
		}
		return last;
	}
	else{
		SEMERROR(FUN_UN_DEC, name);
		return NULL;
	}
}

void SymTab::addInst(InterInst* inst){
	if(curFun){
		curFun->addInst(inst);
	}
	else{
		delete inst;
	}
	return;
}

Fun* SymTab::getCurFun(){
	return curFun;
}

void SymTab::setIr(GenIR* ir){
	this->ir = ir;
	return;
}

void SymTab::toString(){

	printf("-------var table size:%d---\n", (int)(varList.size()));
	for(int i = 0; i < varList.size(); i++){
		string varName = varList[i];
		vector<Var*>& list = *varTab[varName];
		printf("[%d]%s:\n", i, varName.c_str());
		for(int j = 0; j < list.size(); j++){
			printf("\t");
			list[j]->toString();
			printf("\n");
		}
	}
	printf("-------var table end-------\n");
	printf("-------string table begin--\n");
	unordered_map<string, Var*, string_hash>::iterator strIt, strEnd;
	strIt = strTab.begin();
   	strEnd = strTab.end();
	for(; strIt != strEnd; ++strIt){
		printf("%s=%s\n", strIt->second->getName().c_str(), strIt->second->getStrVal().c_str());
	}
	printf("-------string table end---\n");
	printf("-------fun table begin----\n");
	for(int i = 0; i < funList.size(); i++){
		funTab[funList[i]]->toString();
	}
	printf("-------fun table end------\n");
}

void SymTab::printInterCode(){
	for(int i = 0; i < funList.size(); i++){
		funTab[funList[i]]->printInterCode();
	}
}

vector<Var*> SymTab::getGlbVars(){
	vector<Var*> glbVars;
	for(int i = 0; i < varList.size(); i++){
		string varName = varList[i];
		if(varName[0] != '<'){
			vector<Var*>& list = *varTab[varName];
			int j = 0;
			while((j < list.size()) && (list[j]->getPath().size() != 1)){
				j++;
			}
			if(j != list.size()){
				glbVars.push_back(list[j]);
			}
			else{
				//not find glbVar
			}
		}
		else{
			//const var, don't care
		}
	}
	return glbVars;
}

void SymTab::genData(FILE* file){
	vector<Var*> glbVars = getGlbVars();
	for(unsigned int i = 0; i < glbVars.size(); i++){
		Var* var = glbVars[i];
		fprintf(file, "global %s\n", var->getName().c_str());
		fprintf(file, "\t%s ", var->getName().c_str());
		int typeSize = var->getType() == KW_CHAR ? 1 : 4;
		if(var->getArray()){
			fprintf(file, "times %d ", var->getSize() / typeSize);
		}
		else{
		}
		const char* type = var->getType() == KW_CHAR && !var->getPtr() ? "db" : "dd";
		fprintf(file, "%s ", type);
		if(!var->unInit()){
			if(var->isBase()){
				fprintf(file, "%d\n", var->getVal());
			}
			else{
				fprintf(file, "%s\n", var->getPtrVal().c_str());
			}
		}
		else{//can be optimize to place in bss section
			fprintf(file, "0\n");
		}
	}

	unordered_map<string, Var*, string_hash>::iterator strIt, strEnd;
	strIt = strTab.begin();
	strEnd = strTab.end();
	for(; strIt != strEnd; ++strIt){
		Var* str = strIt->second;
		fprintf(file, "\t%s db %s", str->getName().c_str(), str->getRawStr().c_str());
	}
}

void SymTab::genAsm(char* fileName){
	string newName = fileName;
	int pos = newName.find(".c");
	if(pos > 0 && pos == newName.length() - 2){
		newName.replace(pos, 2, ".s");
	}
	else{
		newName = newName + ".s";
	}
	FILE* file = fopen(newName.c_str(), "w");
	//InterInst::file = stdout; //for debug
	InterInst::file = file;

	fprintf(file, "section .data\n");
	genData(file);
	fprintf(file, "section .text\n");
	for(int i = 0; i < funList.size(); i++){
		Fun* fun = funTab[funList[i]];
		fprintf(file, "#fun %s code\n", fun->getName().c_str());
		fprintf(file, "\tglobal %s\n", fun->getName().c_str());
		fprintf(file, "%s:\n", fun->getName().c_str());
		vector<InterInst*> code;
		if(Args::opt){
			list<InterInst*>& optCode = fun->getOptCode();
			for(list<InterInst*>::iterator it = optCode.begin(); it != optCode.end(); it++){
				code.push_back(*it);
			}
		}
		else{
			 code = fun->getInterCode();
		}
		vector<InterInst*>::iterator instIt, instEnd;
		for(instIt = code.begin(), instEnd = code.end(); instIt != instEnd; ++instIt){
			(*instIt)->toX86();
		}
	}
	fclose(file);
}

void SymTab::optimize(){
	for(int i = 0; i < funList.size(); i++){
		funTab[funList[i]]->optimize(this);
	}
}

void SymTab::printOptCode(){
	for(int i = 0; i < funList.size(); i++){
		funTab[funList[i]]->printOptCode();
	}
}
