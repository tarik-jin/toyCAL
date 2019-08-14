#include "compiler.h"
#include "error.h"
#include "cstring"

int main(int argc, char* argv[]){
	vector<char*> srcfiles;
	if(argc > 1){
		for(int i = 1; i < argc - 1; i++){
			srcfiles.push_back(argv[i]);
		}
		char* opt = argv[argc - 1];
		if(!strcmp(opt, "-char")) {Args::showChar = true;}
		else if(!strcmp(opt, "-token")) {Args::showToken = true;}
		else if(!strcmp(opt, "-symbol")) {Args::showSym = true;}
		else if(!strcmp(opt, "-ir")) {Args::showIr = true;}
		else if(!strcmp(opt, "-block")) {Args::opt = true, Args::showBlock = true;}
		else if(!strcmp(opt, "-o")) {Args::opt = true;}
		else if(!strcmp(opt, "-or")) {Args::opt = true, Args::showOr = true;}
		else if(!strcmp(opt, "-h")) {Args::showHelp = true;}
		else {srcfiles.push_back(opt);}
		if(srcfiles.size() == 0){
			cout << "pleas input source files" << endl;
			return -1;
		}
		else{}
	}
	else{
		cout << "cmd format error!(type -h for help)" << endl;
		return -1;
	}
	if(Args::showHelp){
		cout <<
			"cmd format: tareq source[source][option]\n"
			"option:\n"
			"\t-char\t\t#show character\n"
			"\t-token\t\t#show token\n"
			"\t-symbol\t\t#show symbol info\n"
			"\t-ir\t\t#show ir\n"
			"\t-block\t\t#show block and dfg\n"
			"\t-o\t\t#do optimization\n"
			"\t-or\t\t#show optimized ir\n"
			"\t-h\t\t#show help\n"
			;
	}
	else{
		Compiler compiler;
		for(int i = 0; i < srcfiles.size(); i++){
			compiler.compile(srcfiles[i]);
		}
		int error = Error::getErrorNum();
		int warn = Error::getWarnNum();
		if(error != 0){
			cout << "compile errorNum=" << error << ", warnNum=" << warn << "." << endl;
		}
		else{
			cout << "^ ^ compile success ^ ^" << endl;
		}
	}
	return 0;
}
