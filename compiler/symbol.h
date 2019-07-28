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
		int intVal;
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
	Var(vector<int>& sp, bool ext, Tag t, bool ptr, string name, Var* init = NULL);
	Var(vector<int>& sp, bool ext, Tag t, string name, int len);

	vector<int>& getPath();
	string getName();
	string getStrVal();
	void setLeft(bool lf);
	void setOffset(int off);
	int getSize();
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

