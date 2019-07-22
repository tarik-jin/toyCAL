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
   variable or function
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
		return;
	}
	else{
		init();
		return;
	}
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
		if(F(ID)_(MUL)){
			recovery(1, COMMA_LOST, COMMA_WRONG);
			defData();
			defList();
		}
		else{
			recovery(TYPE_FIRST || STATEMENT_FIRST ||
					F(KW_EXTERN)_(RBRACE),
					SEMICON_LOST, SEMICON_WRONG);
		}
	}
	return;
}

/*
   <defData> -> ident <varArrayDef> | mul ident <init>
*/
void Parser::defData(){
	if(F(ID)){
		move();
		varArrayDef();
		return;
	}
	else if(match(MUL)){
		if(F(ID)){
			move();
		}
		else{
			recovery(F(SEMICON)_(COMMA)_(ASSIGN),
					ID_LOST, ID_WRONG);
		}
		init();
		return;
	}
	else{
		recovery(F(SEMICON)_(COMMA)_(ASSIGN)_(LBRACK),
				ID_LOST, ID_WRONG);
		varArrayDef();
		return;
	}
}

/*
   <para> -> <type> <paraData> <paraList> | ^
 */
void Parser::para(){
	if(F(RPAREN)){//empty argus
		return;
	}
	else{
		type();
		paraData();
		paraList();
		return;
	}
}

/*
   <paraData> -> mul ident | ident <paraDataTail>
 */
void Parser::paraData(){
	if(match(MUL)){
		if(F(ID)){
			move();
		}
		else{
			recovery(F(COMMA)_(RPAREN), ID_LOST, ID_WRONG);
		}
		return;
	}
	else if(F(ID)){
		move();
		paraDataTail();
		return;
	}
	else{
		recovery(F(LBRACK)_(COMMA)_(RPAREN), ID_LOST, ID_WRONG);
		return;
	}
}

/*
   <paraDataTail> -> lbrack rbrack | lbrack num rbrack | ^
 */
void Parser::paraDataTail(){
	if(match(LBRACK)){
		if(F(NUM)){
			move();
		}
		else{
		}
		if(!match(RBRACK)){
			recovery(F(COMMA)_(RPAREN), RBRACK_LOST, RBRACK_WRONG);
		}
		else{
		}
		return;
	}
	else{
	}
	return;
}

/*
   <paraList> -> COMMA <type> <paraData> <paraList> | ^
 */
void Parser::paraList(){
	if(match(COMMA)){
		type();
		paraData();
		paraList();
	}
	return;
}

/*
   <funTail> -> SEMICON | <block>
 */
void Parser::funTail(){
	if(match(SEMICON)){
	}
	else{
		block();
	}
	return;
}

/*
	<block> -> lbrac <subProgram> rbrac
 */
void Parser::block(){
	if(!match(LBRACE)){
		recovery(TYPE_FIRST || STATEMENT_FIRST || F(RBRACE),
				LBRACE_LOST, LBRACE_WRONG)
	}
	else{
	}
	subProgram();
	if(!match(RBRACE)){
		recovery(TYPE_FIRST || STATEMENT_FIRST ||
				F(KW_EXTERN)_(KW_ELSE)_(KW_CASE)_KW(DEFAULT),
				RBRACE_LOST, RBRACK_WRONG);
	}
	else{
	}
	return;
}

/*
	<subProgram> -> <localDef> <subProgram> |
					<statements> <subProgram> |
					^
 */
