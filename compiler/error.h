#pragma once
#include "common.h"

class Error{
	static Scanner *scanner;
public:
	
	Error(Scanner* sc);
	static int errorNum;

	static void lexError(int code);
	static void synError(int code, Token* t);
};
