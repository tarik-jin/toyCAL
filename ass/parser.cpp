#include "parser.h"
#include "token.h"
#include "lexer.h"

Parser::Parser(Lexer& lex):lexer(lex){

}

void Parser::analyse(){
	move();
	program();
}

void Parser::program(){
	if(look->tag == END){
		printf("parse success!\n");
		return;
	}
	else{
		switch(look->tag){
			case END:
				return;
			case KW_SEC:
				move();
				match(ID);
				break;
			case KW_GLB:
				move();
				match(ID);
				break;
			case ID:
				move();
				lbTail();
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

void Parser::lbTail(){
	switch(look->tag){
		case KW_TIMES:
			move();
			match(NUM);
			baseTail();
			break;
		case KW_EQU:
			move();
			match(NUM);
			break;
		case COLON:
			move();
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
			move();
			break;
		case STR:
			move();
			break;
		case ID:
			move();
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
