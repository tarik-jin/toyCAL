#pragma once
#include "common.h"
#include<unordered_map>
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
