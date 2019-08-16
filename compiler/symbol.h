#pragma once
#include "common.h"
#include "intercode.h"

class Var{

	bool literal;
	vector<int> scopePath;

	bool externed;
	Tag type;
	string name;
	bool isPtr;
	bool isArray;
	int arraySize;

	bool isLeft;
	Var* initData;
	bool inited;
	union{
		int intVal;
		char charVal;
	};
	string strVal;
	string ptrVal;
	Var* ptr;

	int size;
	int offset;

	void setType(Tag t);
	void setPtr(bool ptr);
	void setExtern(bool ext);
	void setName(string n);
	void setArray(int len);
	void clear();

public:

	Var();
	Var(int val);
	Var(Token* lt);
	Var(vector<int>& sp, Var* v);
	Var(vector<int>& sp, Tag t, bool ptr);
	Var(vector<int>& sp, bool ext, Tag t, bool ptr, string name, Var* init = NULL);
	Var(vector<int>& sp, bool ext, Tag t, string name, int len);

	static Var* getStep(Var* v);
	static Var* getVoid();
	static Var* getTrue();

	bool setInit();
	Var* getInitData();
	vector<int>& getPath();
	string getName();
	string getStrVal();
	void setLeft(bool lf);
	void setOffset(int off);
	int getSize();
	Tag getType();
	bool isVoid();
	bool getLeft();
	int getOffset();
	bool isBase();
	bool isRef();
	void setPointer(Var* p);
	Var* getPointer();
	void toString();
	void value();//output the ir representation of var
	int getVal();
	bool isChar();
	bool notConst();
	bool getArray();
	bool unInit();
	string getPtrVal();
	string getRawStr();
	bool getPtr();

	bool isLiteral();

	int index;//for opt track var info
	bool live;
};

class Fun{

	bool externed;
	Tag type;
	string name;
	vector<Var*> paraVar;

	int maxDepth;
	int curEsp;

	vector<int> scopeEsp;
	InterCode interCode;
	InterInst* returnPoint;

	DFG* dfg;
	list<InterInst*> optCode;

public:
	Fun(bool ext, Tag t, string n, vector<Var*>& paraList);
	~Fun();

	bool match(Fun* f);
	bool match(vector<Var*>& args);
	void define(Fun* def);

	void enterScope();
	void leaveScope();
	void locate(Var* var);

	void addInst(InterInst* inst);
	void setReturnPoint(InterInst* inst);
	InterInst* getReturnPoint();

	bool getExtern();
	void setExtern(bool ext);
	string& getName();
	Tag getType();

	void toString();
	void printInterCode();
	int getMaxDep();
	vector<Var*>& getParaVar();
	vector<InterInst*>& getInterCode();

	void printOptCode();
	void optimize(SymTab* tab);
};

