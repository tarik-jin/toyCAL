#pragma once
#include "common.h"

class Args{
public:
	static bool showHelp;
	static bool showChar;
	static bool showToken;
	static bool showSym;
	static bool showIr;
	static bool showBlock;
	static bool opt;
	static bool showOr;
};

class Compiler{
public:
	void compile(char *file);
};
