#include "token.h"
#include "lexer.h"
#include "error.h"

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

#define LEXERROR(code) Error::lexError(code)

Token* Lexer::tokenize(){
	while(ch != -1)	{
		Token* t = NULL;
		//skip empty char
		while((ch == ' ') || (ch == '\n') || (ch == '\t')){
			scan();
		}
		//identifier
		if((ch >= 'a' && ch <= 'z') ||
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
		}
		else if(ch == '"'){//string
			string str = "";
			while(!scan('"')){
				if(ch == '\\'){//escape character
					scan();
					if(ch == 'n') str.push_back('\n');
					else if(ch == '\\') str.push_back('\\');
					else if(ch == 't') str.push_back('\t');				
					else if(ch == '"') str.push_back('"');
					else if(ch == '0') str.push_back('\0');
					else if(ch == 'n');
					else if(ch == -1){
						LEXERROR(STR_NO_R_QUTION);
						t = new Token(ERR);
						break;
					}
					else{
						str.push_back(ch);
					}
				}
				else if(ch == '\n' || ch == -1){
					LEXERROR(STR_NO_R_QUTION);
					t = new Token(ERR);
					break;
				}
				else{
					str.push_back(ch);
				}
			}
			t = t ? t : (new Str(str));
		}
		else if(ch >= '0' && ch <= '9'){//num
			int val = 0;
			if(ch != 0){//decimal
				do{
					val = val * 10 + (ch - '0');
					scan();
				}while(ch >= '0' && ch <= '9');
			}
			else{
				scan();
				if(ch == 'x'){//hexadecimal
					scan();
					if((ch >= '0' && ch <= '9') ||
							(ch >= 'a' && ch <= 'f') ||
							(ch >= 'A' && ch <= 'F')){
						do{
							val = val * 16;
							if(ch >= '0' && ch <= '9'){
								val += ch - '0';
							}
							else if(ch >= 'a' && ch <= 'f'){
								val += ch - 'a';
							}else if(ch >= 'A' && ch <= 'F'){
								val += ch - 'A';
							}
							else{

							}
							scan();
						}while((ch >= '0' && ch <= '9') ||
							(ch >= 'a' && ch <= 'f') ||
							(ch >= 'A' && ch <= 'F'));
					}
					else{
						LEXERROR(NUM_HEX_TYPE);
						t = new Token(ERR);
					}
				}
				else if(ch == 'b'){//binary
					scan();
					if(ch >= '0' && ch <= '1'){
						do{
							val = val * 2 + ch - '0';
							scan();
						}while(ch >= '0' && ch <= '1');
					}
					else{
						LEXERROR(NUM_BIN_TYPE);
						t = new Token(ERR);
					}
				}
				else if(ch >= '0' && ch <= '7'){//oct
					do{
						val = val * 8 + ch - '0';
						scan();
					}while(ch >= '0' && ch <= '7');
				}
			}
			t = t ? t : (new Num(val));
		}
		else if(ch == '\''){//character
			char c;
			scan();
			if(ch == '\\'){
			}
			else if(ch == '\n' || ch == -1){
			}
			else if(ch == '\''){
			}
			else{
				c = ch;
			}
			if(!t){
				if(scan('\'')){
					t = new Char(c);
				}
				else{
					LEXERROR(CHAR_NO_R_QUTION);
					t = new Token(ERR);
				}
			}
			else{
			}
		}
		else{//delimiter

		}

	}
}

bool Lexer::scan(char need){
	ch = scanner.scan();
	if(need){
		if(ch != need){
			return false;
		}
		else{
			ch = scanner.scan();
			return true;
		}
	}
	else{
		return true;
	}
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
