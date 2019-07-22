#include "parser.h"
#include "lexer.h"
#include "error.h"

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
	return;
}

#define SYNERROR(code, t) Error::synError(code, t)

#define F(C) look->tag == C
#define _(T) || look->tag == T
#define TYPE_FIRST F(KW_INT)_(KW_CHAR)_(KW_VOID)

void Parser::recovery(bool cond, SynError lost, SynError wrong){
	if(cond){
		SYNERROR(lost, look);
	}
	else{
		SYNERROR(wrong, look);
		move();
	}
	return;
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

void Parser::analyse(){
	move();
	program();
	return;
}

/*
   	^ represents emtpy token
	<program> -> <segment><program> | ^
*/
void Parser::program(){
	if(F(END)){//end of file
		return;
	}
	else{
		segment();
		program();
	}
	return;
}

/*
	<segment> -> rsv_extern<type><def> | <type><def>
*/
void Parser::segment(){
	bool ext = match(KW_EXTERN);
	Tag t = type();
	def();
	return;
}

/*
	<type> -> rsv_int | rsv_char | rsv_void
*/
Tag Parser::type(){
	Tag tmp = KW_INT;
	if(TYPE_FIRST){
		tmp == look->tag;
		move();
	}
	else{
		recovery(F(ID)_(MUL), TYPE_LOST, TYPE_WRONG);
	}
	return tmp;
}

/*
    <def> -> ident <idtail> | mul id <init> <defList>
*/
void Parser::def(){
	if(match(MUL)){//pointer
		if(F(ID)){
			move();
		}
		else{
			recovery(F(SEMICON)_(COMMA)_(ASSIGN), ID_LOST, ID_WRONG);
		}
		init();
		defList();
	}
	else{
		if(F(ID)){
			move();
		}
		else{
			recovery(F(SEMICON)_(COMMA)_(ASSIGN)_(LPAREN)_(LBRACK),
					ID_LOST, ID_WRONG);
		}
		idTail();
	}
	return;
}

/*
   <idtail> -> <varArrayDef> <defList> | LPAREN <para> RPAREN <funTail>
*/
void Parser::idTail(){
	if(match(LPAREN)){//function
		para();
		if(match(LPAREN)){
		}
		else{
			recovery(F(LBRACK)_(SEMICON), RPAREN_LOST, RPAREN_WRONG);
		}
		funTail();
	}
	else{
		varArrayDef();
		defList();
	}
	return;
}

/*
   <varArrayDef> -> LBRACK NUM RBRACK | <init>
*/
void Parser::varArrayDef(){
	if(match(LBRACK)){
		if(F(NUM)){
			move();
		}
		else{
			recovery(F(RBRACK), NUM_LOST, NUM_WRONG);
		}
		if(match(RBRACK)){
		}
		else{
			recovery(F(COMMA)_(SEMICON), RBRACK_LOST, RBRACK_WRONG);
		}
	}
	else{
		init();
	}
	return;
}

/*
   <init> -> ASSIGN <expr> | ^
*/
void Parser::init(){
	if(match(ASSIGN)){
		expr();
	}
	return;
}

/*
   <defList> -> COMMA <defData> <defList> | SEMICON
*/
void Parser::defList(){
	if(match(COMMA)){
		defData();
		defList();
	}
	else if(match(SEMICON)){
		return;
	}
	else{
	}
}
