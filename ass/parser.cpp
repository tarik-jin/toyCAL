#include "parser.h"

void Parser:program(){
	switch(look->tag){
		case END:
			return;
		case KW_SEC:
			match(ID);
			break;
		case KW_GLB:
			match(ID);
			break;
		case ID:
			lbTail();
			break;
		default:
			inst();
	}
	program();
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
	printf("%s\n", t->toString().c_str());
#endif
}

void Parser::lbTail(){
	move();
	switch(look->tag){
		case KW_TIMES:
			match(NUM);
			baseTail();
			break;
		case KW_EQU:
			match(NUM);
			break;
		case COLON:
			break;
		default:
			baseTail();
	}
}

void Parser::baseTail(){
	int l = len();
	value();
}

int Parser::len(){
	switch(look->tag){
		case KW_DB:
			return 1;
		case KW_DW:
			return 2;
		case KW_DD:
			return 4;
		default:
			printf("len err![line:%d]\n", lexer.getLine());
			return 0;
	}
}

void Parser::value(){
	type();
	valTail();
}

void Parser::type(){
	switch(look->tag){
		case NUM:
			break;
		case STR:
			break;
		case ID:
			break;
		case ADD:
			off();
			break;
		case SUB:
			off();
			break;
		default:
			printf("type err![line:%d]\n", lexer.getLine());
	}
}

void Parser::valTail(){
	move();
	switch(look->tag){
		case COMMA:
			type();
			valTail();
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
		match(COMMA);
		oprand();
	}
	else if(op >= I_CALL && op <= I_POP){
		oprand();
	}
	else if(op == I_RET){
		//todo
	}
	else{
		printf("opcode err[line:%d]\n", lexer.getLine());
	}
}

void Parser::oprand(){
	switch(look->tag){
		case NUM:
			break;
		case ID:
			break;
		case LBRACK:
			mem();
			break;
		case SUB:
			match(NUM);
			break;
		default:
			reg();
	}
}

void Parser::reg(){
}

void Parser::mem(){
	match(LBRACK);
	addr();
	match(RBRACK);
}

void Parser::addr(){
	switch(look->tag){
		case NUM:
			break;
		case ID:
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
		case Num:
			break;
		default:
			reg();
	}
}
