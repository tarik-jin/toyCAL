#include "error.h"
#include "token.h"
#include "lexer.h"

int Error::errorNum = 0;

void Error::lexError(int code){
	static const char *lexErrorTale[] = {
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

Error::Error(Scanner* sc){
	scanner = sc;
}
