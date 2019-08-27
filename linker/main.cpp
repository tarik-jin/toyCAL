#include "linker.h"
bool showLink = false;

int main(int argc, char* argv[]){
	if(argc < 2){//tareq-linker [objFile] [exeFile]
		printf("please input objFile or outFile!\n");
		return -1;
	}
	else{
		Linker linker;
		string objFile;
		string desFileName;
		int idx = 1;//index for parser option
		objFile = argv[idx];
		while(objFile.rfind(".o") == objFile.length() - 2){
			linker.addElf(objFile.c_str());
			idx = idx + 1;
			if(idx == argc){
				printf("please input outputFile\n");
				return 0;
			}
			else{
				objFile = argv[idx];
			}
		}
		if(idx == 1){
			printf("please input object file!\n");
			return 0;
		}
		else{
			desFileName = objFile;
			if(argc == idx + 1){
				showLink = false;
			}
			else{
				showLink = (argv[idx + 1][0] == 'y');
			}
			linker.link(desFileName.c_str());
			return 0;
		}
	}
}
