#include "common.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"

char* finName;
bool showAss = false;
FILE* fout = NULL;

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("please input assembly file\n");
		return 0;
	}
	else{
		finName = argv[1];
		string fTempname(finName);
		fTempname.erase(fTempname.end() - 1);
		fout = fopen((fTempname + "t").c_str(), "w");
		if(argc >= 3){
			showAss = (argv[2][0] == 'y');//show section info
		}
		else{
			showAss = false;
		}
		Scanner scanner(finName);
		Lexer lexer(scanner);
		Parser parser(lexer);
		parser.analyse();
		fclose(fout);
		return 0;
	}
}
