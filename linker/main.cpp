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
		showLink = true;//tmp for debug
		int i = 1;//index for parser option
		objFile = argv[i];
		while(objFile.rfind(".o") == objFile.length() - 2){
			linker.addElf(objFile.c_str());
			objFile = argv[++i];
		}
		desFileName = objFile;
		if(argc = i + 1){
			showLink = (argv[i + 1][0] == 'y');
		}
		else{
			showLink = false;
		}


		return 0;
	}
}
