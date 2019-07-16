#include"token.h"
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
	lastch = 0;
	lineNum = 1;
	colNum = 0;
}

Scanner::~Scanner(){
	if(file){
		cout<<file<<"scan finish"<<endl;
		fclose(file);
	}
}

char Scanner::scan(){
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
		lastch = ch;
		return ch;
	}
}

char* Scanner::getFile(){
	return fileName;
}

int Scanner::getLine(){
	return lineNum;
}

int Scanner::getCol(){
	return colNum;
}

Token* Lexer::tokenize(){
	while(ch != -1)	{
		Token* t = NULL;
		while((ch == ' ') || (ch == '\n') || (ch == '\t')){
			scan();
		}
		//identifier
		if((ch >= 'a' && ch <= 'z') ||{{{
			   	(ch >= 'A' && ch <= 'Z') ||
			   	(ch == '_')){
			string name = "";
			do{
				name.push_back(ch);
				scan();
			}while((ch >= 'a' && ch <= 'z') ||
					(ch >= 'A' && ch <= 'Z') ||
					(ch >= '0' && ch <= '9') ||
					(ch == '_'));

			Tag tag = keywords.getTag(name);
			if(tag == ID){
				t = new Id(name);
			}
			else{
				t = new Token(tag);//keyword
			}
		}/*}}}*/
		else if(ch == '"'){//string{{{

		}/*}}}*/
		else if(ch >= '0' && ch <= '9'){{{{


		}/*}}}*/
		else if(ch == '\''){/*{{{*/

		}/*}}}*/
		else{//delimiter{{{

		}/*}}}*/

	}
}

void Lexer::scan(){
	ch = scanner.scan();
	return;
}

Keywords::Keywords(){
	keywords["int"] = KW_INT;
	keywords["char"] = KW_CHAR;
	keywords["void"] = KW_VOID;
	keywords["extern"] = KW_EXTERN;
	keywords["if"] = KW_IF;
    keywords["else"] = KW_ELSE;
    keywords["switch"] = KW_SWITCH;
    keywords["case"] = KW_CASE;
    keywords["default"] = KW_DEFAULT;
    keywords["while"] = KW_WHILE;
	keywords["do"] = KW_DO;
    keywords["for"] = KW_FOR;
    keywords["break"] = KW_BREAK;
    keywords["continue"] = KW_CONTINUE;
    keywords["return"] = KW_RETURN;
}

Tag Keywords::getTag(string name){
	return keywords.find(name) != keywords.end()
		? keywords[name] : ID;
}
