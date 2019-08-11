#include "compiler.h"
#include "lexer.h"
#include "error.h"
#include "parser.h"
#include "symtab.h"
#include "genir.h"

bool Args::showChar = false;
bool Args::showToken = false;
bool Args::showSym = false;
bool Args::showIr = false;
bool Args::opt = false;
bool Args::showHelp = false;

void Compiler::compile(char* file){
	//prepare
	Scanner scanner(file);
	Error error(&scanner);
	Lexer lexer(scanner);
	SymTab symtab;
	GenIR ir(symtab);
	Parser parser(lexer, symtab, ir);

	parser.analyse();

	if(Error::getErrorNum() + Error::getWarnNum()){
		cout << "error in lexer or parser" <<endl;
		return;
	}
	else{
		//action after parser analyse correctly
		if(Args::showSym){
			symtab.toString();
		}
		else{
		}
		if(Args::showIr){
			symtab.printInterCode();
		}
		else{
		}
		symtab.genAsm(file);
	}
}
