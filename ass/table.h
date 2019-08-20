#pragma once
#include "common.h"
#include "elf_file.h"

struct lb_record{
	static int curAddr;
	string segName;
	string lbName;
	bool  isEqu;
	bool externed;
	bool global;
	int addr;
	int times;
	int len;
	list<int> cont;
	lb_record(string n, bool ex = false);
	lb_record(string n, int v);
	lb_record(string n, int t, int l, list<int> c);
	void write();
};

class Table{
	bool hasName(string name);
    struct string_hash{
         size_t operator()(const string& str) const{
             std::hash<std::string> hash_fn;
             return hash_fn(str);
         }
     };
public:
	unordered_map<string, lb_record*, string_hash> lb_map;
	vector<lb_record*> defLbs;

	void addlb(lb_record* p_lb);
	lb_record* getlb(string name);
	void switchSeg(Token* look);
	void exportSyms();
	void write();
};
