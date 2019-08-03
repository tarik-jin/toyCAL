#include "parser.h"
#include "token.h"
#include "lexer.h"
#include "error.h"
#include "compiler.h"
#include "symtab.h"
#include "symbol.h"
#include "genir.h"

Parser::Parser(Lexer& lex, SymTab& tab, GenIR& inter)
	:lexer(lex), symtab(tab), ir(inter){
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
	def(ext, t);
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
void Parser::def(bool ext, Tag t){
	string name = "";
	if(match(MUL)){//pointer
		if(F(ID)){
			name = ((Id*)look)->name;
			move();
		}
		else{
			recovery(F(SEMICON)_(COMMA)_(ASSIGN), ID_LOST, ID_WRONG);
		}
		symtab.addVar(init(ext, t, true, name));
		defList(ext, t);
	}
	else{
		if(F(ID)){
			move();
		}
		else{
			recovery(F(SEMICON)_(COMMA)_(ASSIGN)_(LPAREN)_(LBRACK),
					ID_LOST, ID_WRONG);
		}
		idTail(ext, t, false, name);
	}
	return;
}

/*
   variable or function
   <idtail> -> <varArrayDef> <defList> | LPAREN <para> RPAREN <funTail>
*/
void Parser::idTail(bool ext, Tag t, bool ptr, string name){
	if(match(LPAREN)){//function
		symtab.enter();
		vector<Var*> paraList;
		para(paraList);
		if(!match(RPAREN)){
			recovery(F(LBRACK)_(SEMICON), RPAREN_LOST, RPAREN_WRONG);
		}
		else{
		}
		Fun* fun = new Fun(ext, t, name, paraList);
		funTail(fun);
		symtab.leave();
	}
	else{
		symtab.addVar(varArrayDef(ext, t, false, name));
		defList(ext, t);
	}
	return;
}

/*
   <varArrayDef> -> LBRACK NUM RBRACK | <init>
*/
Var* Parser::varArrayDef(bool ext, Tag t, bool ptr, string name){
	if(match(LBRACK)){
		int len = 0;
		if(F(NUM)){
			len = ((Num*)look)->val;
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
		return new Var(symtab.getScopePath(), ext, t, name, len);
	}
	else{
		return init(ext, t, ptr, name);
	}
}

/*
   <init> -> ASSIGN <expr> | ^
*/
Var* Parser::init(bool ext, Tag t, bool ptr, string name){
	Var* initVal = NULL;
	if(match(ASSIGN)){
		initVal = expr();
	}
	return new Var(symtab.getScopePath(), ext, t, ptr, name, initVal);
}

/*
   <defList> -> COMMA <defData> <defList> | SEMICON
*/
void Parser::defList(bool ext, Tag t){
	if(match(COMMA)){
		symtab.addVar(defData(ext, t));
		defList(ext, t);
	}
	else if(match(SEMICON)){
		return;
	}
	else{
		if(F(ID)_(MUL)){
			recovery(1, COMMA_LOST, COMMA_WRONG);
			symtab.addVar(defData(ext, t));
			defList(ext, t);
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
Var* Parser::defData(bool ext, Tag t){
	string name = "";
	if(F(ID)){
		name = ((Id*)look)->name;
		move();
		return varArrayDef(ext, t, false, name);
	}
	else if(match(MUL)){
		if(F(ID)){
			name = ((Id*)look)->name;
			move();
		}
		else{
			recovery(F(SEMICON)_(COMMA)_(ASSIGN),
					ID_LOST, ID_WRONG);
		}
		return init(ext, t, true, name);
	}
	else{
		recovery(F(SEMICON)_(COMMA)_(ASSIGN)_(LBRACK),
				ID_LOST, ID_WRONG);
		return varArrayDef(ext, t, false, name);
	}
}

/*
   <para> -> <type> <paraData> <paraList> | ^
 */
void Parser::para(vector<Var*>& list){
	if(F(RPAREN)){//empty argus
		return;
	}
	else{
		Tag t = type();
		Var* v = paraData(t);
		symtab.addVar(v);
		list.push_back(v);
		paraList(list);
		return;
	}
}

/*
   <paraData> -> mul ident | ident <paraDataTail>
 */
Var* Parser::paraData(Tag t){
	string name = "";
	if(match(MUL)){
		if(F(ID)){
			name = ((Id*)look)->name;
			move();
		}
		else{
			recovery(F(COMMA)_(RPAREN), ID_LOST, ID_WRONG);
		}
		return new Var(symtab.getScopePath(), false, t, true, name);
	}
	else if(F(ID)){
		name = ((Id*)look)->name;
		move();
		return paraDataTail(t, name);
	}
	else{
		recovery(F(LBRACK)_(COMMA)_(RPAREN), ID_LOST, ID_WRONG);
		return new Var(symtab.getScopePath(), false, t, false, name);
	}
}

/*
   <paraDataTail> -> lbrack rbrack | lbrack num rbrack | ^
 */
Var* Parser::paraDataTail(Tag t, string name){
	if(match(LBRACK)){
		int len = 1;
		if(F(NUM)){
			len = ((Num*)look)->val;
			move();
		}
		else{
			//args ignore array len
		}
		if(!match(RBRACK)){
			recovery(F(COMMA)_(RPAREN), RBRACK_LOST, RBRACK_WRONG);
		}
		else{
		}
		return new Var(symtab.getScopePath(), false, t, false, name);
	}
	else{
	}
	return new Var(symtab.getScopePath(), false, t, false, name);
}

/*
   <paraList> -> COMMA <type> <paraData> <paraList> | ^
 */
void Parser::paraList(vector<Var*>& list){
	if(match(COMMA)){
		Tag t = type();
		Var* v = paraData(t);
		symtab.addVar(v);
		list.push_back(v);
		paraList(list);
	}
	else{
	}
	return;
}

/*
   <funTail> -> SEMICON | <block>
 */
void Parser::funTail(Fun* f){
	if(match(SEMICON)){
		symtab.decFun(f);
	}
	else{
		symtab.decFun(f);
		block();
		symtab.endDefFun();
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
	Tag t = type();
	symtab.addVar(defData(false, t));
	defList(false, t);
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
			ir.genReturn(altExpr());
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
	symtab.enter();
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
	symtab.leave();
	return;
}

/*
   <doWhileStat> -> rsv_do <block> rsv_while lparen <altExpr> rparen semicon
 */
void Parser::doWhileStat(){
	symtab.enter();
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
	symtab.leave();
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
	symtab.enter();
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
	symtab.leave();
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
	symtab.enter();
	InterInst* _else;
    InterInst* _exit;
	match(KW_IF);
	if(!match(LPAREN)){
		recovery(EXPR_FIRST, LPAREN_LOST, LPAREN_WRONG);
	}
	else{
	}
	Var* cond = expr();
	ir.genIfHead(cond, _else);
	if(!match(RPAREN)){
		recovery(F(LBRACE), RPAREN_LOST, RPAREN_WRONG);
	}
	else{
	}
	block();
	symtab.leave();
	if(F(KW_ELSE)){
		ir.genElseHead(_else, _exit);
		elseStat();
		ir.genElseTail(_exit);
	}
	else{
		ir.genIfTail(_else);
	}
	return;
}

/*
   <elseStat> -> rsv_else <block> | ^
 */
void Parser::elseStat(){
	if(match(KW_ELSE)){
		symtab.enter();
		block();
	}
	else{
	}
	symtab.leave();
	return;
}

/*
   <switchStat> -> rsv_switch lparen <expr> rparen lbrac <caseStat>  rbrac
 */
void Parser::switchStat(){
	symtab.enter();
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
	symtab.leave();
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
		symtab.enter();
		subProgram();
		symtab.leave();
		caseStat();
	}
	else if(match(KW_DEFAULT)){
		if(!match(COLON)){
			recovery(TYPE_FIRST || STATEMENT_FIRST, COLON_LOST, COLON_WRONG);
		}
		else{
		}
		symtab.enter();
		subProgram();
		symtab.leave();
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
Var* Parser::altExpr(){
	if(EXPR_FIRST){
		return expr();
	}
	else{
		return Var::getVoid();
	}
}

/*
   <expr> -> <assExpr>
 */
Var* Parser::expr(){
	return assExpr();
}

/*
   <assExpr> -> <orExpr> <assTail>
 */
Var* Parser::assExpr(){
	Var* lval = orExpr();
	return assTail(lval);
}

/*
   <assTail> -> assign <orExpr> <assTail> | ^
 */
Var* Parser::assTail(Var* lval){
	if(match(ASSIGN)){
		Var* rval = orExpr();
		Var* result = ir.genTwoOp(lval, ASSIGN, rval);
		return assTail(result);
	}
	else{
		return lval;
	}
}

/*
   <orExpr> -> <andExpr> <orTail>
 */
Var* Parser::orExpr(){
	Var* lval = andExpr();
    return orTail(lval);
}

/*
	<orTail> -> or <andExpr> <orTail> | ^
 */
Var* Parser::orTail(Var* lval){
	if(match(OR)){
		Var* rval = andExpr();
		Var* result = ir.genTwoOp(lval, OR, rval);
		return orTail(result);
	}
	else{
		return lval;
	}
}

/*
   <andExpr> -> <cmpExpr> <andTail>
 */
Var* Parser::andExpr(){
	Var* lval = cmpExpr();
	return andTail(lval);
}

/*
   <andTail> -> and <cmpExpr> <andTail> | ^
 */
Var* Parser::andTail(Var* lval){
	if(match(AND)){
		Var* rval = cmpExpr();
		Var* result = ir.genTwoOp(lval, AND, rval);
		return andTail(result);
	}
	else{
		return lval;
	}
}

/*
	<cmpExpr> -> <aloExpr> <cmpTail>
 */
Var* Parser::cmpExpr(){
	Var* lval = aloExpr();
	return cmpTail(lval);
}

/*
   <cmpTail> -> <cmps> <aloExpr> <cmpTail> | ^
 */
Var* Parser::cmpTail(Var* lval){
	if(F(GT)_(GE)_(LT)_(LE)_(EQU)_(NEQU)){
		Tag opt = cmps();
		Var* rval = aloExpr();
		Var* result = ir.genTwoOp(lval, opt, rval);
		return cmpTail(result);
	}
	else{
		return lval;
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
Var* Parser::aloExpr(){
	Var* lval = item();
	return aloTail(lval);
}

/*
   <aloTail> -> <adds> <item> <aloTail> | ^
 */
Var* Parser::aloTail(Var* lval){
	if(F(ADD)_(SUB)){
		Tag opt = adds();
		Var* rval = item();
		Var* result = ir.genTwoOp(lval, opt, rval);
		return aloTail(result);
	}
	else{
		return lval;
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
Var* Parser::item(){
	Var* lval = factor();
	return itemTail(lval);
}

/*
   <itemTail> -> <muls> <factor> <itemTail> | ^
 */
Var* Parser::itemTail(Var* lval){
	if(F(MUL)_(DIV)_(MOD)){
		Tag opt = muls();
		Var* rval = factor();
		Var* result = ir.genTwoOp(lval, opt, rval);
		return itemTail(result);
	}
	else{
		return lval;
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
Var* Parser::factor(){
	if(F(NOT)_(SUB)_(LEA)_(MUL)_(INC)_(DEC)){
		Tag opt = lop();
		Var* v = factor();
		return ir.genOneOpLeft(opt, v);
	}
	else{
		return val();
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
Var* Parser::val(){
	Var* v = elem();
	if(F(INC)_(DEC)){
		Tag opt = rop();
		v = ir.genOneOpRight(v, opt);
	}
	else{
		// rop can derive empty
	}
	return v;
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
Var* Parser::elem(){
	Var* v = NULL;
	if(F(ID)){
		string name = ((Id*)look)->name;
		move();
		v = idExpr(name);
	}
	else if(match(LPAREN)){
		v = expr();
		if(!match(RPAREN)){
			recovery(LVAL_OPR, RPAREN_LOST, RPAREN_WRONG);
		}
		else{
		}
	}
	else{
		v = literal();
	}
	return v;
}

/*
   <literal> -> number | string | character
 */
Var* Parser::literal(){
	Var *v = NULL;
	if(F(NUM)_(STR)_(CH)){
		v = new Var(look);
		if(F(STR)){
			symtab.addStr(v);
		}
		else{
			symtab.addVar(v);
		}
		move();
	}
	else{
		recovery(RVAL_OPR, LITERAL_LOST, LITERAL_WRONG);
	}
	return v;
}

/*
   <idExpr> -> lbrack <expr> rbrack | lparen <realArg> rparen | ^
 */
Var* Parser::idExpr(string name){
	Var* v = NULL;
	if(match(LBRACK)){
		Var* index = expr();
		if(!match(RBRACK)){
			recovery(LVAL_OPR, LBRACK_LOST, LBRACK_WRONG);
		}
		else{
		}
		Var* array = symtab.getVar(name);
		v = ir.genArray(array, index);
	}
	else if(match(LPAREN)){
		vector<Var*> args;
		realArg(args);
		if(!match(RPAREN)){
			recovery(RVAL_OPR, RPAREN_LOST, RPAREN_WRONG);
		}
		else{
		}
		Fun* function = symtab.getFun(name, args);
		v = ir.genCall(function, args);
	}
	else{
		v = symtab.getVar(name);
	}
	return v;
}

/*
   <realArg> -> <arg> <argList> | ^
 */
void Parser::realArg(vector<Var*>& args){
	if(EXPR_FIRST){
		args.push_back(arg());
		argList(args);
	}
	else{
	}
	return;
}

/*
   <argList> -> comma <arg> <argList> | ^
 */
void Parser::argList(vector<Var*>& args){
	if(match(COMMA)){
		args.push_back(arg());
		argList(args);
	}
	else{
	}
	return;
}

/*
   <arg> -> <expr>
 */
Var* Parser::arg(){
	return expr();
}
