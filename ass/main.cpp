#include "common.h"
#include "lexer.h"

char* finName;
bool showAsm = false;

int main(int argc, char* argv[]){
	finName = argv[1];
	showAsm = (argv[2][0] == 'y');
	Scanner scanner(finName);
	Lexer lexer(scanner);
	return 0;
}
