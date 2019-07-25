#include "parser.h"
#include "token.h"
#include "lexer.h"
#include "error.h"
#include "compiler.h"

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
	if(Args::showToken){
		printf("%s\n", look->toString().c_str());
	}
	else{
	}
	return;
}

#define SYNERROR(code, t) Error::synError(code, t)

#define F(C) look->tag == C
#define _(T) || look->tag == T
#define TYPE_FIRST F(KW_INT)_(KW_CHAR)_(KW_VOID)
#define EXPR_FIRST F(LPAREN)_(NUM)_(CH)_(STR)_(ID)_(NOT)_(SUB)_(LEA)\
_(MUL)_(INC)_(DEC)
#define LVAL_OPR F(ASSIGN)_(OR)_(AND)_(GT)_(GE)_(LT)_(LE)\
_(EQU)_(NEQU)_(ADD)_(SUB)_(MUL)_(DIV)_(MOD)_(INC)_(DEC)
#define RVAL_OPR F(OR)_(AND)_(GT)_(GE)_(LT)_(LE)_(EQU)_(NEQU)\
_(ADD)_(SUB)_(MUL)_(DIV)_(MOD)
#define STATEMENT_FIRST (EXPR_FIRST)_(SEMICON)_(KW_WHILE)_(KW_FOR)\
_(KW_DO)_(KW_IF)_(KW_SWITCH)_(KW_RETURN)_(KW_BREAK)_(KW_CONTINUE)

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
		if(match(RPAREN)){
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
				LBRACE_LOST, LBRACE_WRONG);
	}
	else{
	}
	subProgram();
	if(!match(RBRACE)){
		recovery(TYPE_FIRST || STATEMENT_FIRST ||
				F(KW_EXTERN)_(KW_ELSE)_(KW_CASE)_(KW_DEFAULT),
				RBRACE_LOST, RBRACK_WRONG);
	}
	else{
	}
	return;
}

/*
	<subProgram> -> <localDef> <subProgram> |
					<statement> <subProgram> |
					^
 */
void Parser::subProgram(){
	if(TYPE_FIRST){
		localDef();
		subProgram();
	}
	else if(STATEMENT_FIRST){
		statement();
		subProgram();
	}
	return;
}

/*
   <localDef> -> <type> <defData> <defList>
 */
void Parser::localDef(){
	type();
	defData();
	defList();
	return;
}

/*
   <statement> -> <altExpr> SEMICON |
   				  <whileStat> | <forStat> | <doWhileStat> |
				  <ifStat> | <switchStat> |
				  KW_BREAK SEMICON |
				  KW_CONTINUE SEMICON |
				  KW_RETURN <altExpr> SEMICON
 */
void Parser::statement(){
	switch(look->tag){
		case KW_WHILE:
			whileStat();
			break;
		case KW_FOR:
			forStat();
			break;
		case KW_DO:
			doWhileStat();
			break;
		case KW_IF:
			ifStat();
			break;
		case KW_SWITCH:
			switchStat();
			break;
		case KW_BREAK:
			move();
			if(!match(SEMICON)){
				recovery(TYPE_FIRST || STATEMENT_FIRST || F(RBRACE),
						SEMICON_LOST, SEMICON_WRONG);
			}
			else{
			}
			break;
		case KW_CONTINUE:
			move();
			if(!match(SEMICON)){
				recovery(TYPE_FIRST || STATEMENT_FIRST || F(RBRACE),
						SEMICON_LOST, SEMICON_WRONG);
			}
			else{
			}
			break;
		case KW_RETURN:
			move();
			altExpr();
			if(!match(SEMICON)){
				recovery(TYPE_FIRST || STATEMENT_FIRST || F(RBRACE),
						SEMICON_LOST, SEMICON_WRONG);
			}
			break;
		default: //expr
			altExpr();
			if(!match(SEMICON)){
				recovery(TYPE_FIRST || STATEMENT_FIRST || F(RBRACE),
						SEMICON_LOST, SEMICON_WRONG);
			}	
	}
	return;
}

/*
   <whileStat> -> KW_WHILE LPAREN <altExpr> RPAREN <block>
 */
void Parser::whileStat(){
	match(KW_WHILE);
	if(!match(LPAREN)){
		recovery(EXPR_FIRST || F(RPAREN), LPAREN_LOST, LPAREN_WRONG);
	}
	else{
	}
	altExpr();
	if(!match(RPAREN)){
		recovery(F(LBRACE), RPAREN_LOST, RPAREN_WRONG);
	}
	else{
	}
	block();
	return;
}

/*
   <doWhileStat> -> rsv_do <block> rsv_while lparen <altExpr> rparen semicon
 */
void Parser::doWhileStat(){
	match(KW_DO);
	block();
	if(!match(KW_WHILE)){
		recovery(F(LPAREN), WHILE_LOST, WHILE_WRONG);
	}
	else{
	}
	if(!match(LPAREN)){
		recovery(EXPR_FIRST || F(RPAREN), LPAREN_LOST, LPAREN_WRONG);
	}
	else{
	}
	altExpr();
	if(!match(RPAREN)){
		recovery(F(SEMICON), RPAREN_LOST, RPAREN_WRONG);
	}
	else{
	}
	if(!match(SEMICON)){
		recovery(TYPE_FIRST || STATEMENT_FIRST || F(RBRACE),
				SEMICON_LOST, SEMICON_WRONG);
	}
	else{
	}
	return;
}

/*
   <forStat> -> rsv_for lparen <forInit> <altExpr> semicon <altexpr> rparen <block>
 */
void Parser::forStat(){
	match(KW_FOR);
	if(!match(LPAREN)){
		recovery(TYPE_FIRST || EXPR_FIRST || F(SEMICON), LPAREN_LOST, LPAREN_WRONG);
	}
	else{
	}
	forInit();
	altExpr();
	if(!match(SEMICON)){
		recovery(EXPR_FIRST, SEMICON_LOST, SEMICON_WRONG);
	}
	else{
	}
	altExpr();
	if(!match(RPAREN)){
		recovery(F(LBRACE), RPAREN_LOST, RPAREN_WRONG);
	}
	else{
	}
	block();
	return;
}

/*
	<forInit> -> <localDef> | <altExpr> SEMICON
 */
void Parser::forInit(){
	if(TYPE_FIRST){
		localDef();
	}
	else{
		altExpr();
		if(!match(SEMICON)){
			recovery(EXPR_FIRST, SEMICON_LOST, SEMICON_WRONG);
		}
		else{
		}
	}
	return;
}

/*
   <ifStat> -> rsv_if lparen <expr> rparen <block> <elseStat>
 */
void Parser::ifStat(){
	match(KW_IF);
	if(!match(LPAREN)){
		recovery(EXPR_FIRST, LPAREN_LOST, LPAREN_WRONG);
	}
	else{
	}
	expr();
	if(!match(RPAREN)){
		recovery(F(LBRACE), RPAREN_LOST, RPAREN_WRONG);
	}
	else{
	}
	block();
	elseStat();
	return;
}

/*
   <elseStat> -> rsv_else <block> | ^
 */
void Parser::elseStat(){
	if(match(KW_ELSE)){
		block();
	}
	else{
	}
	return;
}

/*
   <switchStat> -> rsv_switch lparen <expr> rparen lbrac <caseStat>  rbrac
 */
void Parser::switchStat(){
	match(KW_SWITCH);
	if(!match(LPAREN)){
		recovery(EXPR_FIRST, LPAREN_LOST, LPAREN_WRONG);
	}
	else{
	}
	expr();
	if(!match(RPAREN)){
		recovery(F(LBRACE), RPAREN_LOST, RPAREN_WRONG);
	}
	else{
	}
	if(!match(LBRACE)){
		recovery(F(KW_CASE)_(KW_DEFAULT), LBRACE_LOST, LBRACE_WRONG);
	}
	else{
	}
	caseStat();
	if(!match(RBRACE)){
		recovery(TYPE_FIRST || STATEMENT_FIRST, RBRACK_LOST, RBRACK_WRONG);
	}
	else{
	}
	return;
}

/*
   <caseStat> -> rsv_case <caseLabel> colon <subProgram> <caseStat> |
   				 rsv_default colon <subProgram>
 */
void Parser::caseStat(){
	if(match(KW_CASE)){
		caseLabel();
		if(!match(COLON)){
			recovery(TYPE_FIRST || STATEMENT_FIRST, COLON_LOST, COLON_WRONG);
		}
		else{
		}
		subProgram();
		caseStat();
	}
	else if(match(KW_DEFAULT)){
		if(!match(COLON)){
			recovery(TYPE_FIRST || STATEMENT_FIRST, COLON_LOST, COLON_WRONG);
		}
		else{
		}
		subProgram();
	}
	else{
		cout<<"case statment misses case or default rsv_words"<<endl;
	}
	return;
}

/*
   <caseLabel> -> <literal>
 */
void Parser::caseLabel(){
	literal();
	return;
}

/*
   <altExpr> -> <expr> | ^
 */
void Parser::altExpr(){
	if(EXPR_FIRST){
		expr();
		return;
	}
	else{
	}
	return;
}

/*
   <expr> -> <assExpr>
 */
void Parser::expr(){
	assExpr();
	return;
}

/*
   <assExpr> -> <orExpr> <assTail>
 */
void Parser::assExpr(){
	orExpr();
	assTail();
	return;
}

/*
   <assTail> -> assign <assExpr> | ^
 */
void Parser::assTail(){
	if(match(ASSIGN)){
		assExpr();
		return;
	}
	else{
		return;
	}
}

/*
   <orExpr> -> <andExpr> <orTail>
 */
void Parser::orExpr(){
	andExpr();
	orTail();
	return;
}

/*
	<orTail> -> or <andExpr> <orTail> | ^
 */
void Parser::orTail(){
	if(match(OR)){
		andExpr();
		orTail();
		return;
	}
	else{
		return;
	}

}

/*
   <andExpr> -> <cmpExpr> <andTail>
 */
void Parser::andExpr(){
	cmpExpr();
	andTail();
	return;
}

/*
   <andTail> -> and <cmpExpr> <andTail> | ^
 */
void Parser::andTail(){
	if(match(AND)){
		cmpExpr();
		andTail();
		return;
	}
	else{
		return;
	}
}

/*
	<cmpExpr> -> <aloExpr> <cmpTail>
 */
void Parser::cmpExpr(){
	aloExpr();
	cmpTail();
	return;
}

/*
   <cmpTail> -> <cmps> <aloExpr> <cmpTail> | ^
 */
void Parser::cmpTail(){
	if(F(GT)_(GE)_(LT)_(LE)_(EQU)_(NEQU)){
		cmps();
		aloExpr();
		cmpTail();
		return;
	}
	else{
		return;
	}
}

/*
   <cmps> -> gt | ge | lt | le | equ | nequ
 */
Tag Parser::cmps(){
	Tag opt = look->tag;
	move();
	return opt;
}

/*
	<aloExpr> -> <item> <aloTail>
 */
void Parser::aloExpr(){
	item();
	aloTail();
	return;
}

/*
   <aloTail> -> <adds> <item> <aloTail> | ^
 */
void Parser::aloTail(){
	if(F(ADD)_(SUB)){
		adds();
		item();
		aloTail();
		return;
	}
	else{
		return;
	}
}

/*
   <adds> -> add | sub
 */
Tag Parser::adds(){
	Tag opt = look->tag;
	move();
	return opt;
}

/*
   <item> -> <factor> <itemTail>
 */
void Parser::item(){
	factor();
	itemTail();
	return;
}

/*
   <itemTail> -> <muls> <factor> <itemTail> | ^
 */
void Parser::itemTail(){
	if(F(MUL)_(DIV)_(MOD)){
		muls();
		factor();
		itemTail();
		return;
	}
	else{
		return;
	}
}

/*
   <muls> -> mul | div | mod
 */
Tag Parser::muls(){
	Tag opt = look->tag;
	move();
	return opt;
}

/*
   <factor> -> <lop> <factor> | <val>
 */
void Parser::factor(){
	if(F(NOT)_(SUB)_(LEA)_(MUL)_(INC)_(DEC)){
		lop();
		factor();
		return;
	}
	else{
		val();
		return;
	}
}

/*
   <lop> -> not | sub | lea | mul | incr | decr
 */
Tag Parser::lop(){
	Tag opt = look->tag;
	move();
	return opt;
}

/*
   <val> -> <elem> <rop>
 */
void Parser::val(){
	elem();
	if(F(INC)_(DEC)){
		Tag opt = rop();
	}
	return;
}

/*
   <rop> -> incr | decr | ^
   //only tag == incr or tag ==  decr will the function was called
 */
Tag Parser::rop(){
	Tag opt = look->tag;
	move();
	return opt;
}

/*
   <elem> -> ident <idExpr> | lparen <expr> rparen | <literal>
 */
void Parser::elem(){
	if(F(ID)){
		move();
		idExpr();
	}
	else if(match(LPAREN)){
		expr();
		if(!match(RPAREN)){
			recovery(LVAL_OPR, RPAREN_LOST, RPAREN_WRONG);
		}
		else{

		}
	}
	else{
		literal();
	}
	return;
}

/*
   <literal> -> number | string | character
 */
void Parser::literal(){
	if(F(NUM)_(STR)_(CH)){
		move();
	}
	else{
		recovery(RVAL_OPR, LITERAL_LOST, LITERAL_WRONG);
	}
	return;
}

/*
   <idExpr> -> lbrack <expr> rbrack | lparen <realArg> rparen | ^
 */
void Parser::idExpr(){
	if(match(LBRACK)){
		expr();
		if(!match(RBRACK)){
			recovery(LVAL_OPR, LBRACK_LOST, LBRACK_WRONG);
		}
		else{
		}
	}
	else if(match(LPAREN)){
		realArg();
		if(!match(RPAREN)){
			recovery(RVAL_OPR, RPAREN_LOST, RPAREN_WRONG);
		}
		else{
		}	
	}
	else{
		
	}
	return;
}

/*
   <realArg> -> <arg> <argList> | ^
 */
void Parser::realArg(){
	if(EXPR_FIRST){
		arg();
		argList();
	}
	else{
	}
	return;
}

/*
   <argList> -> comma <arg> <argList> | ^
 */
void Parser::argList(){
	if(match(COMMA)){
		arg();
		argList();
	}
	else{
	}
	return;
}

/*
   <arg> -> <expr>
 */
void Parser::arg(){
	expr();
	return;
}
