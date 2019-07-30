#pragma once
#include "common.h"

class Error{
	static Scanner *scanner;
public:

	Error(Scanner* sc);
	static int errorNum;
	static int warnNum;

	static int getErrorNum();

	static void lexError(int code);
	static void synError(int code, Token* t);
	static void semError(int code, string name = "");
	static void semWarn(int code, string name = "");
};
