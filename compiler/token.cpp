#include "token.h"
#include "error.h"

const char* tokenName[] = {
	"error",
	"end of file",
	"identifier",
	"int", "char", "void",
	"extern",
	"cst_num", "cst_char", "cst_str",
	"!", "&",
	"+", "-", "*", "/", "%",
	"++", "--",
	">", ">=", "<", "<=", "==", "!=",
	"&&", "||",
	"(", ")",
	"[", "]",
	"{", "}",
	",", ":", ";",
	"=",
	"if", "else",
	"switch", "case", "default",
	"while", "do", "for",
	"break", "continue", "return"
};

Token::Token(Tag t){
	tag = t;
}

string Token::toString(){
	return tokenName[tag];
}

Id::Id(string n):Token(ID), name(n){
}

string Id::toString(){
	return Token::toString() + "::" + name;
}

Str::Str(string s):Token(STR), str(s){
}

string Str::toString(){
	return string("[") + Token::toString() + "]:" + str;
}

Num::Num(int v):Token(NUM), val(v){
}

string Num::toString(){
	stringstream ss;
	ss << val;
	return string("[") + Token::toString() + "]:" + ss.str();
}

Char::Char(char c):Token(CH), ch(c){
}

string Char::toString(){
	stringstream ss;
	ss << ch;
	return string("[") + Token::toString() + "]:" + ss.str();
}

