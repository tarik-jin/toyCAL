#include "common.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"

char* finName;

int main(int argc, char* argv[]){
	finName = argv[1];
	Scanner scanner(finName);
	Lexer lexer(scanner);
	Parser parser(lexer);
	parser.analyse();
	return 0;
}
