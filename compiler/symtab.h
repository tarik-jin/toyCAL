#pragma once
#include "common.h"

class SymTab{

	//hash function
	struct string_hash{
		 operator()(const string& str) const{
			hash<string> hash_fn;
			return hash_fn(str);
		}
	};

	hash_map<string, vector<Var*>*, string_hash> varTab;
	hash_map<string, Var*, string_hash> strTab;
	hash_map<string, Fun*, string_hash> funTab;

	Fun* curFun;
	int scopeId;
	vector<int> scopePath;
public:
	symTab();
	~symTab();

	void enter();
	void leave();
};
