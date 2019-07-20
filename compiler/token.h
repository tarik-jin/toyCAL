#pragma once
#include "common.h"

class Token{
public:
	Tag tag;
	Token(Tag t);
	virtual string toString();
};

class Id:public Token{
public:
	string name;
	Id(string n);
	virtual string toString();
};

class Num:public Token{
public:
	int val;
	Num(int v);
	virtual string toString();
};

class Char:public Token{
public:
	char ch;
	Char(char c);
	virtual string toString();
};

class Str:public Token{
public:
	string str;
	Str(string s);
	virtual string toString();
};

class Keywords{
	struct string_hash{
		size_t operator()(const string& str) const{
			std::hash<std::string> hash_fn;
			return hash_fn(str);
		}
	};
	unordered_map<string, Tag, string_hash> keywords;
public:
	Keywords();
	Tag getTag(string name);
};
