#include "token.h"
#include "lexer.h"
#include "error.h"

int Error::errorNum = 0;
Scanner* Error::scanner = NULL;

Error::Error(Scanner* sc){
	scanner = sc;
}

void Error::lexError(int code){
	static const char *lexErrorTable[] = {
		"string lost right quotation marks",
		"binary number is empty", 
		"hexadecimal number is empyt",
		"charcter miss right quotation marks",
		"non-support empty character",
		"error or operator",
		"non terminal multi-line comments",
		"error token"
	};
	errorNum++;
	printf("%s<row:%d, col:%d> lexer error:%s.\n",
			scanner->getFile(), scanner->getLine(),
			scanner->getCol(), lexErrorTable[code]);
}

void Error::synError(int code, Token* t){
	static const char* synErrorTable[] = {
		"type",
		"identifier",
		"array length",
		"constant",
		"comma",
		"semicolon",
		"=",
		"colon",
		"while",
		"(",
		")",
		"[",
		"]",
		"{",
		"}"
	};
	errorNum++;
	if(code % 2 == 0){//lost
		printf("%s<row:%d> syntax Error: lost %s before %s.\n",
				scanner->getFile(), scanner->getLine(),
				synErrorTable[code / 2], t->toString().c_str());
	}
	else{//wrong
		printf("%s<row:%d> syntax Error: wrong match %s at %s.\n",
				scanner->getFile(), scanner->getLine(),
				synErrorTable[code /2], t->toString().c_str());
	}
}

void Error::semError(int code, string name){

	static const char* semErrorTable[] = {
		"var: redef",
		"fun: redef",
		"var: undec",
		"fun: undec",
		"fun: dec&def mismatch",
		"fun-args: real&formal mismatch",
		"var: extern var forbid init when dec", //our simplification
		"fun: forbid extern when def", //our simplification
		"array: len must be pos int",
		"var: init type error",
		"var: extern var must be constant",
		"var: void type error",
		"expr: invalid Lval expr",
		"expr: assign incompatible",
		"expr: operator cannot be basic",
		"expr: operator is not basic",
		"array: invalid index expr type",
		"expr: void type(return) error",
		"break: location error",
		"continue: location error",
		"return: incompatiable return type"
	};
	errorNum++;
	printf("%s<row:%d> semantic error: %s %s.\n", scanner()->getFile(),
			scanner()->getLine(), name.c_str(), semErrorTable[code]);
}

int Error::getErrorNum(){
	return errorNum;
}
