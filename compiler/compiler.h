#pragma once
#include "common.h"

class Args{
public:
	static bool showChar;
	static bool showToken;
	static bool showHelp;
};

class Compiler{
public:
	void compile(char *file);
};
