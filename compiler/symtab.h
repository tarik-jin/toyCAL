#pragma once
#include "common.h"

class SymTab{

	vector<string> funList;
	vector<string> varList;

	//hash function
	struct string_hash{
		 size_t operator()(const string& str) const{
			hash<string> hash_fn;
			return hash_fn(str);
		}
	};

	unordered_map<string, vector<Var*>*, string_hash> varTab;
	unordered_map<string, Var*, string_hash> strTab;
	unordered_map<string, Fun*, string_hash> funTab;

	Fun* curFun;
	int scopeId;
	vector<int> scopePath;

	GenIR* ir;

public:
	SymTab();
	~SymTab();

	void enter();
	void leave();

	void addVar(Var* v);
	void addStr(Var* v);
	Var* getVar(string name);

	void decFun(Fun* fun);
	void defFun(Fun* fun);
	void endDefFun();
	Fun* getFun(string name, vector<Var*>& args);

	void addInst(InterInst* inst);

	vector<int>& getScopePath();
};
