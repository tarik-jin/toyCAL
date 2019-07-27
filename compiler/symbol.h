#pragma once
#include "common.h"

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
		int intVa;
		char charVal;
	};
	string strVal;
	string ptrVal;
	Var* ptr;

	int size;
	int offset;

	void setType(Tag t);
	void clear();

public:
	Var();
	Var(Token* lt);

	void setLeft(bool lf);
};

class Fun{
	
	bool externed;
	Tag type;
	string name;
	vector<Var*> paraVar;

	int maxDepth;
	int curEsp;
	
	vector<int> scopeEsp;
public:
	Fun(bool ext, Tag t, string n, vector<Var*>& paraList);
	~Fun();

	void enterScope();
	void leaveScope();
	void locate(Var* var);

};
