#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
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

enum SynError{
	TYPE_LOST,
	TYPE_WRONG,
	ID_LOST,
	ID_WRONG,
	NUM_LOST,
	NUM_WRONG,
	LITERAL_LOST,
	LITERAL_WRONG,
	COMMA_LOST,
	COMMA_WRONG,
	SEMICON_LOST,
	SEMICON_WRONG,
	ASSIGN_LOST,
	ASSIGN_WRONG,
	COLON_LOST,
	COLON_WRONG,
	WHILE_LOST,
	WHILE_WRONG,
	LPAREN_LOST,
	LPAREN_WRONG,
	RPAREN_LOST,
	RPAREN_WRONG,
	LBRACK_LOST,
	LBRACK_WRONG,
	RBRACK_LOST,
	RBRACK_WRONG,
	LBRACE_LOST,
	LBRACE_WRONG,
	RBRACE_LOST,
	RBRACE_WRONG
};

enum SymError{
	VAR_RE_DEF,
	FUN_RE_DEF,
	VAR_UN_DEC,
	FUN_UN_DEC,
	FUN_DEC_ERR,
	FUN_CALL_ERR,
	DEC_INIT_DENY,
	EXTERN_FUN_DEF,
	ARRAY_LEN_INVALID,
	VAR_INIT_ERR,
	GLB_INIT_ERR,
	VOID_VAR,
	EXPR_NOT_LEFT_VAL,
	ASSIGN_TYPE_ERR,
	EXPR_IS_BASE,
	EXPR_NOT_BASE,
	ARR_TYPE_ERR,
	EXPR_IS_VOID,
	BREAK_ERR,
	CONTINUE_ERR,
	RETURN_ERR
};

enum SemWarn{
	FUN_DEC_CONFLICT,
	FUN_RET_CONFLICT
};

enum Operator{
	OP_NOP,
	OP_DEC,
	OP_ENTRY, OP_EXIT,
	OP_AS,
	OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
	OP_NEG,
	OP_GT, OP_GE, OP_LT, OP_LE, OP_EQU, OP_NE,
	OP_NOT,
	OP_AND, OP_OR,
	OP_LEA,
	OP_SET,	OP_GET,
	OP_JMP,
	OP_JT, OP_JF, OP_JNE,
	OP_ARG,
	OP_PROC,
	OP_CALL,
	OP_RET,
	OP_RETV
};

class Token;
class Keywords;
class Scanner;
class Lexer;
class SymTab;
class Args;
class Var;
class Fun;
class GenIR;
class InterInst;
class InterCode;
