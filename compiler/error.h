#pragma once
#include "common.h"

class Error{
	static Scanner *scanner;
public:

	static int errorNum;

	static void lexError(int code);
};
