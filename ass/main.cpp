#include "common.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"

char* finName;
bool showAss = false;

int main(int argc, char* argv[]){
	finName = argv[1];
	showAss = (argv[2][0] == 'y');
	Scanner scanner(finName);
	Lexer lexer(scanner);
	Parser parser(lexer);
	parser.analyse();
	return 0;
}
