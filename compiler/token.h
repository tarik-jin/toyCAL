#pragma once
#include "common.h"
#include<ext/hash_map>
using namespace __gnu_cxx;
class Token{
public:
	Tag tag;
	Token(Tag t);
};

class Id:public Token{
public:
	string name;
	Id(string n);
};

class Num:public Token{
public:
	int val;
	Num(int v);
};

class Char:public Token{
public:
	char ch;
	Char(char c);
};

class Str:public Token{
public:
	string str;
	Str(string s);
};

class Keywords{
	hash_map<string, Tag, string_hash> keywords;
public:
	Keywords();
	Tag getTag(string name);
};
