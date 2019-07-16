#include"lexer.h"

Scanner::Scanner(char* name){
	file = fopen(name, "r");
	if(!file){
		cout<<file<<"open fail"<<endl;
	}
	fileName = name;
	//init scanner state
	lineLen = 0;
	readPos = -1;
	lastCh = 0;
	lineNum = 1;
	colNum = 0;
}

Scanner::~Scanner(){
	if(file){
		cout<<file<<"scan finish"<<endl;
		fclose(file);
	}
}

char* Scanner::scan(){
	if(!file){
		return -1;
	}
	else{
		if(readPos == lineLen - 1){
			lineLen = fread(line, 1, BUFFLEN, file);
			if(lineLen == 0){
				lineLen = 1;
				line[0] = -1;
			}
			else{

			}
			readPos = -1;
		}
		readPos++;
		char ch = line[readPos];
		if(lastch == '\n'){
			lineNum++;
			colNum = 0;
		}
		else{

		}
		if(ch == -1){
			fclose(file);
			file = NULL;
		}
		else if(ch != '\n'){
			colNum++;
		}
		else{
			
		}
		lastCh = ch;
		return ch;
	}
}

char* Scanner::getFile(){
	return fileName;
}

int Scanner::getLine(){
	return lineNum;
}

int Scanner:getCol(){
	return colNum;
}
