#include "lexer.h"
#include "token.h"

Scanner::Scanner(char* name){
	file = fopen(name, "r");
	if(!file){
		printf("cannot open file %s\n", name);
	}
	fileName = name;
	lineLen = 0;
	readPos = -1;
	lastch = 0;
	lineNum = 1;
}

int Scanner::scan(){
	if(file){
		if(readPos == lineLen - 1){
			lineLen = fread(line, 1, BUFLEN, file);
			if(lineLen == 0){
				lineLen = 1;
				line[0] = -1;
			}
			else{}
			readPos = -1;
		}
		else{}
		readPos++;
		char ch = line[readPos];
		if(lastch = '\n'){
			lineNum++;
		}
		else{}
		if(ch == -1){
			fclose(file);
			file = NULL;
		}
		else{}
		lastch = ch;
#ifdef SCANdbg
		showChar(ch);
#endif
		return ch;
	}
	else{
		return -1;
	}
}

void Scanner::showChar(char ch){
    switch (ch){
        case -1:
            cout << "EOF";
            break;
        case '\n':
            cout << "\\n";
            break;
        case '\t':
            cout << "\\t";
            break;
        case ' ':
            cout << "<blank>";
            break;
        default:
            cout << ch;
    }
    printf("\t\t<%d>\n", ch);
    return;
}

Keywords::Keywords(){
	keywords["al"] = BR_AL;
	keywords["cl"] = BR_CL;
	keywords["dl"] = BR_DL;
	keywords["bl"] = BR_BL;
	keywords["ah"] = BR_AH;
	keywords["ch"] = BR_CH;
	keywords["dh"] = BR_DH;
	keywords["bh"] = BR_BH;
	keywords["eax"] = DR_EAX;
	keywords["ecx"] = DR_ECX;
	keywords["edx"] = DR_EDX;
	keywords["ebx"] = DR_EBX;
	keywords["esp"] = DR_ESP;
	keywords["ebp"] = DR_EBP;
	keywords["esi"] = DR_ESI;
	keywords["edi"] = DR_EDI;
	//2p
	keywords["mov"] = I_MOV;
	keywords["cmp"] = I_CMP;
	keywords["sub"] = I_SUB;
	keywords["add"] = I_ADD;
	keywords["and"] = I_AND;
	keywords["or"] = I_OR;
	keywords["lea"] = I_LEA;
	//1p
	keywords["call"] = I_CALL;
	keywords["int"] = I_INT;
	keywords["imul"] = I_IMUL;
	keywords["idiv"] = I_IDIV;
	keywords["ineg"] = I_INEG;
	keywords["inc"] = I_INC;
	keywords["idec"] = I_DEC;
	keywords["jmp"] = I_JMP;
	keywords["je"] = I_JE;
	keywords["jne"] = I_JNE;
	keywords["sete"] = I_SETE;
	keywords["setne"] = I_SETNE;
	keywords["setg"] = I_SETG;
	keywords["setge"] = I_SETGE;
	keywords["setl"] = I_SETL;
	keywords["setle"] = I_SETLE;
	keywords["push"] = I_PUSH;
	keywords["pop"] = I_POP;
	//op
	keywords["ret"] = I_RET;
	//misc
	keywords["section"] = KW_SEC;
	keywords["global"] = KW_GLB;
	keywords["equ"] = KW_EQU;
	keywords["times"] = KW_TIMES;
	keywords["db"] = KW_DB;
	keywords["dw"] = KW_DW;
	keywords["dd"] = KW_DD;
}

Tag Keywords::getTag(string name){
	return keywords.find(name) != keywords.end() ? keywords[name] : ID;
}

Lexer::Lexer(Scanner& sc):scanner(sc){
	token = NULL;
	ch = ' ';
}

Token* Lexer::tokenize(){
	while(ch == ' ' || ch == 10 || ch == 9){
		scanner.scan();
	}
	Token* t = NULL;
	if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_' || ch == '@' || ch == '.'){//identifier
		string name = "";
		do{
			name.push_back(ch);
			scanner.scan();
		}while(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_' || ch == '@' || ch == '.' || ch >= '0' && ch <= '9');
		Tag tag = keywords.getTag(name);
		if(tag == ID){
			t = new Id(name);
		}
		else{
			t = new Token(tag);
		}
	}
	else if(ch >= '0' && ch <= '9'){//num constant
		int val = 0;
		do{
			val = val * 10 + ch - '0';
			scanner.scan();
		}while(ch >= '0' && ch <= '9');
		t = new Num(val);
	}
	else if(ch == '"'){//str constant
		string str = "";
		ch = scanner.scan();
		if(ch == -1){
			printf("error srt syntax");
			return token = new Token(END);
		}
		else{}
		while(ch != '"'){
			str.push_back(ch);
			ch = scanner.scan();
			if(ch == -1){
				printf("error srt syntax");
				return token = new Token(END);
			}
			else{}
		}
		t = new Str(str);
		ch = scanner.scan();
	}
	else{
		switch(ch){
			case '+': t = new Token(ADD); ch = scanner.scan(); break;
			case '-': t = new Token(SUB); ch = scanner.scan(); break;
			case ',': t = new Token(COMMA); ch = scanner.scan(); break;
			case '[': t = new Token(LBRACK); ch = scanner.scan(); break;
			case ']': t = new Token(RBRACK); ch = scanner.scan(); break;
			case ':': t = new Token(COLON); ch = scanner.scan(); break;
			case ';':
				t = new Token(ERR);
				ch = scanner.scan();
				while(ch != '\n'){
					ch = scanner.scan();
				}
				ch = scanner.scan();
				break;
			default:
				t = new Token(ERR);
				printf("unknown token[line:$%d]\n", scanner.getLine());
				ch = scanner.scan();
		}
	}
	if(token){
		delete token;
	}
	else{}
	token = t;
	if(token && token->tag != ERR){
		return token;
	}
	else{
		return token = new Token(END);
	}
}
