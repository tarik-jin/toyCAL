#include "common.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"

char* finName;
bool showAss = false;

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("please input assembly file\n");
		return 0;
	}
	else{
		finName = argv[1];
		if(argc >= 3){
			showAss = (argv[2][0] == 'y');
		}
		else{
			showAss = false;
		}
		Scanner scanner(finName);
		Lexer lexer(scanner);
		Parser parser(lexer);
		parser.analyse();
		return 0;
	}
}
