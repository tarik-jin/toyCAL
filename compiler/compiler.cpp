#include "compiler.h"
#include "lexer.h"
#include "error.h"
#include "parser.h"
#include "symtab.h"

bool Args::showChar = false;
bool Args::showToken = false;
bool Args::showHelp = false;

void Compiler::compile(char* file){
	//prepare
	Scanner scanner(file);
	Error error(&scanner);
	Lexer lexer(scanner);
	SymTab symtab;
	Parser parser(lexer, symtab);

	parser.analyse();

	if(Error::getErrorNum()){
		cout << "error in lexer or parser" <<endl;
		return;
	}
	else{
		//action after parser
	}
}
