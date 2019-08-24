#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "elf_file.h"

char* finName;
bool showAss = false;
FILE* ftmp = NULL;
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
		ftmp = fopen((fTempname + "t").c_str(), "w");
		fout = fopen((fTempname + "o").c_str(), "w");
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
		obj.assemObj();
		obj.writeElf();
		fclose(fout);
		fclose(ftmp);
		remove((fTempname + "t").c_str());
		return 0;
	}
}
