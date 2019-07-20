#include "parser.h"
#include "lexer.h"

Parser::Parser(Lexer& lex):lexer(lex){
}

Parser::~Parser(){
	if(look){
		delete look;
	}
	else{
	}
}

void Parser::move(){
	look = lexer.tokenize();
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
