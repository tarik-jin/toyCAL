#include "parser.h"
#include "token.h"
#include "lexer.h"
#include "table.h"

string curSeg = "";
int dataLen = 0;
Parser::Parser(Lexer& lex):lexer(lex){

}

int scanLop = 0;
void Parser::analyse(){
	if(++scanLop <= 2){
		move();
		program();
		reset();//two-pass analyse
		analyse();
	}
}

void Parser::program(){
	if(look->tag == END && scanLop == 2){
		printf("parse success!\n");
		table.exportSyms();
		obj.printAll();
		return;
	}
	else{
		string lbName = "";
		switch(look->tag){
			case END:
				table.switchSeg(look);
				return;
			case KW_SEC:
				move();
				table.switchSeg(look);
				match(ID);
				break;
			case KW_GLB:
				move();
				match(ID);
				break;
			case ID:
				lbName += ((Id*)look)->name;
				lbTail(lbName);
				break;
			default:
				inst();
		}
		program();
	}
}

bool Parser::match(Tag need){
	if(look->tag == need){
		move();
		return true;
	}
	else{
		return false;
	}
}

void Parser::move(){
	look= lexer.tokenize();
#ifdef SHOWtoken
	printf("%s\n", look->toString().c_str());
#endif
}

void Parser::lbTail(string lbName){
	move();
	switch(look->tag){
		case KW_TIMES:
			move();
			baseTail(lbName, ((Num*)look)->val);
			match(NUM);
			break;
		case KW_EQU:
			move();
			table.addlb(new lb_record(lbName, ((Num*)look)->val));
			match(NUM);
			break;
		case COLON:
			table.addlb(new lb_record(lbName));
			move();
			break;
		default:
			baseTail(lbName, 1);
	}
}

void Parser::baseTail(string lbName, int times){
	int l = len();
	value(lbName, times, l);
}

int Parser::len(){
	switch(look->tag){
		case KW_DB:	return 1;
		case KW_DW:	return 2;
		case KW_DD:	return 4;
		default: printf("len err![line:%d]\n", lexer.getLine()); return 0;
	}
}

void Parser::value(string lbName, int times, int len){
	list<int> cont;
	type(cont, len);
	valTail(cont, len);
	table.addlb(new lb_record(lbName, times, len, cont));
}

void Parser::type(list<int>& cont, int len){
	switch(look->tag){
		case NUM:
			cont.push_back(((Num*)look)->val);
			move();
			break;
		case STR:
			for(int i = 0; i < ((Str*)look)->str.size(); i++){
				cont.push_back(((Str*)look)->str[i]);
			}
			move();
			break;
		case ID:
			cont.push_back(table.getlb(((Str*)look)->str)->addr);
			move();
			break;
		default:
			printf("type err![line:%d]\n", lexer.getLine());
	}
}

void Parser::valTail(list<int>& cont, int len){
	switch(look->tag){
		case COMMA:
			type(cont, len);
			valTail(cont, len);
			break;
		default:
			return;
	}
}

void Parser::off(){
}

void Parser::inst(){
	Tag op = look->tag;
	if(op >= I_MOV && op <= I_LEA){
		oprand();
		//leave comma match in oprand();
		oprand();
	}
	else if(op >= I_CALL && op <= I_POP){
		oprand();
	}
	else if(op == I_RET){
		//todo
		move();
	}
	else{
		printf("opcode err[line:%d]\n", lexer.getLine());
	}
}

void Parser::oprand(){
	move();
	switch(look->tag){
		case NUM:
			move();
			break;
		case ID:
			move();
			break;
		case LBRACK:
			mem();
			break;
		case SUB:
			move();
			match(NUM);
			break;
		default:
			reg();
	}
}

void Parser::reg(){
	move();
}

void Parser::mem(){
	match(LBRACK);
	addr();
	match(RBRACK);
}

void Parser::addr(){
	switch(look->tag){
		case NUM:
			move();
			break;
		case ID:
			move();
			break;
		default:
			reg();
			regAddr();
	}
}

void Parser::regAddr(){
	Tag sign = look->tag;
	if(sign == SUB || sign == ADD){
		off();
		regAddrTail();
	}
	else{
	}
}

void Parser::regAddrTail(){
	move();
	switch(look->tag){
		case NUM:
			move();
			break;
		default:
			reg();
	}
}

void Parser::reset(){
	lexer.reset();
}
