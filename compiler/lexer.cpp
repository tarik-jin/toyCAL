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
			if(ch == '\\'){//escape character
				scan();
				if(ch == 'n'){
					c = '\n';
				}
				else if(ch == '\\'){
					c = '\\';
				}
				else if(ch == 't'){
					c = '\t';
				}
				else if(ch == '0'){
					c = '\0';
				}
				else if(ch == '\''){
					c = '\'';
				}
				else if(ch == -1 || ch == '\n'){
					LEXERROR(CHAR_NO_R_QUTION);
					t = new Token(ERR);
				}
				else{
					c = ch;
				}
			}
			else if(ch == '\n' || ch == -1){
				LEXERROR(CHAR_NO_R_QUTION);
				t = new Token(ERR);
			}
			else if(ch == '\''){//empty character
				LEXERROR(CHAR_NO_DATA);
				t = new Token(ERR);
				scan();
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
			switch(ch){
				case '#':
					while(ch != '\n' && ch != -1){
						scan();
					}
					t = new Token(ERR);
					break;
				case '+':
					t = new Token(scan('+') ? INC : ADD);
					break;
				case '-':
					t = new Token(scan('-') ? DEC : SUB);
					break;
				case '*':
					t = new Token(MUL);
					scan();
					break;
				case '/':
					scan();
					if(ch == '/'){//single line comment
						do{
							scan();
						}while(ch != '\n' && ch != -1);
						t = new Token(ERR);
					}
					else if(ch == '*'){//multi-line comment
						char pre;
						do{
							scan();
							if(ch == '*'){
								while(scan('*'));
								pre = ch;
							}else{
								//
							}
						}while((ch != -1) &&
								(pre != '/'));
						if(ch == -1){
							LEXERROR(COMMENT_NO_END);
							t = new Token(ERR);
						}
						else{//multi-line comment end
							t = new Token(ERR);
						}
					}
					else{
						t = new Token(DIV);
					}
					break;
				case '%':
					t = new Token(MOD);
					scan();
					break;
				case '>':
					t = new Token(scan('=') ? GE : GT);
					break;
				case '<':
					t = new Token(scan('=') ? LE : LT);
					break;
				case '=':
					t = new Token(scan('=') ? EQU : ASSIGN);
					break;
				case '&':
					t = new Token(scan('&') ? AND : LEA);
					break;
				case '|':
					t = new Token(scan('|') ? OR : ERR);
					if(t->tag == ERR){
						LEXERROR(OR_NO_PAIR);
					}
					else{
					}
					break;
				case '!':
					t = new Token(scan('=') ? NEQU : NOT);
					break;
				case ',':
					t = new Token(COMMA);
					scan();
					break;
				case ':':
					t = new Token(COLON);
					scan();
					break;
				case ';':
					t = new Token(SEMICON);
					scan();
					break;
				case '(':
					t = new Token(LPAREN);
					scan();
					break;
				case ')':
					t = new Token(RPAREN);
					scan();
					break;
				case '[':
					t = new Token(LBRACK);
					scan();
					break;
				case ']':
					t = new Token(RBRACK);
					scan();
					break;
				case '{':
					t = new Token(LBRACE);
					scan();
					break;
				case '}':
					t = new Token(RBRACE);
					scan();
					break;
				case -1:
					t = new Token(END);
					scan();
					break;
				default:
					t = new Token(ERR);
					LEXERROR(TOKEN_NO_EXIST);
					scan();
			}
		}
		if(token){
			delete token;
		}
		else{
		}
		token = t;
		if(token && token->tag != ERR){
			return token;
		}
		else{
			continue;//skip error token()
		}
	}
}

Keywords Lexer::keywords;

Lexer::Lexer(Scanner& sc):scanner(sc){
	token = NULL;
	ch = ' ';
}

Lexer::~Lexer(){
	if(!token){
		delete token;
	}
	else{
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
