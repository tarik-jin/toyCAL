#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

enum Tag{
	ERR,
	END,
	ID,
	KW_INT, KW_CHAR, KW_VOID,
	KW_EXTERN,
	NUM, CH, STR,
	NOT, LEA,
	ADD, SUB, MUL, DIV, MOD,
	INC, DEC,
	GT, GE, LT, LE, EQU, NEQU,
	AND, OR,
	LPAREN, RPAREN,
	LBRACK, RBRACK,
	LBRACE, RBRACE,
	COMMA, COLON, SEMICON,
	ASSIGN,
	KW_IF, KW_ELSE,
	KW_SWITCH, KW_CASE, KW_DEFAULT,
	KW_WHILE, KW_DO, KW_FOR,
	KW_BREAK, KW_CONTINUE, KW_RETURN
};

enum LexError{
	STR_NO_R_QUTION,
	NUM_BIN_TYPE,
	NUM_HEX_TYPE,
	CHAR_NO_R_QUTION,
	CHAR_NO_DATA,
	OR_NO_PAIR,
	COMMENT_NO_END,
	TOKEN_NO_EXIST
};

class Scanner;
