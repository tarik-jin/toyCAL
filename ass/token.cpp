#include "token.h"

const char* tokenName[] = {
	"error",
	"end of file",
	"identifier",
	"num",
	"str",
	"al", "cl", "dl", "bl",
	"ah", "ch", "dh", "bh",
	"eax", "ecx", "edx", "ebx",
	"esp", "ebp", "esi", "edi",
	"mov", "cmp", "sub", "add", "and", "or", "lea",
   	"call", "int", "mul", "div", "neg", "inc", "dec",
   	"jmp", "cje", "jne",
   	"sete", "setne", "setg", "setge", "setl", "setle",
   	"push", "pop",
   	"ret",
   	"section", "global", "equ", "times",
   	"db", "dw", "dd",
   	"s_+", "s_-", "s_,", "s_[", "s_]", "s_;"
};

Token::Token(Tag t){
    tag = t;
}

Id::Id(string n):Token(ID), name(n){
}

Num::Num(int v):Token(NUM), val(v){
}

Str::Str(string s):Token(STR), str(s){
}

string Token::toString(){
	return tokenName[tag];
}

string Id::toString(){
	return Token::toString() + "::" + name;
}

string Str::toString(){
	return string("[")+Token::toString()+"]:"+str;
}

string Num::toString(){
	stringstream ss;
	ss<<val;
	return string("[")+Token::toString()+"]:"+ss.str();
}
