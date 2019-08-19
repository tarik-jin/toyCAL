#include "common.h"
#include "lexer.h"
#include "token.h"

char* finName;

int main(int argc, char* argv[]){
	finName = argv[1];
	Scanner scanner(finName);
	Lexer lexer(scanner);
	Token* t = NULL;
	do{
		t = lexer.tokenize();
#ifdef SHOWtoken
		printf("%s\n", t->toString().c_str());
#endif
	}while(t->tag != END);
	return 0;
}
