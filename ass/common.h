#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <list>
#include <vector>
using namespace std;

enum Tag{
	ERR,
	END,
	ID,
	NUM, STR,
	BR_AL, BR_CL, BR_DL, BR_BL,
	BR_AH, BR_CH, BR_DH, BR_BH,
	DR_EAX, DR_ECX, DR_EDX, DR_EBX,
	DR_ESP, DR_EBP, DR_ESI, DR_EDI,
	I_MOV, I_CMP, I_SUB, I_ADD, I_AND, I_OR, I_LEA,
	I_CALL, I_INT, I_IMUL, I_IDIV, I_NEG, I_INC, I_DEC,
	I_JMP, I_JE, I_JNE,
	I_SETE, I_SETNE, I_SETG, I_SETGE, I_SETL, I_SETLE,
	I_PUSH, I_POP,
	I_RET,
	KW_SEC, KW_GLB, KW_EQU, KW_TIMES,
	KW_DB, KW_DW, KW_DD,
	ADD, SUB, COMMA, LBRACK, RBRACK, COLON
};

class Token;
class Lexer;
class Table;
struct lb_record;
class Elf_file;
struct ModRM;
struct SIB;
struct Inst;
class Generator;

extern int scanLop;
extern Table table;
extern string curSeg;
extern int dataLen;
extern Elf_file obj;
extern bool showAss;
extern ModRM modrm;
extern SIB sib;
extern Inst instr;
extern lb_record* relLb;
extern Generator generator;
extern FILE* fout;

//#define SHOWchar
#define SHOWtoken

enum op_type{
	NONE,
	IMMEDIATE,
	REGISTER,
	MEMORY,
};

