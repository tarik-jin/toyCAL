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
	Token* numTimes = NULL;
	switch(look->tag){
		case KW_TIMES:
			move();
			numTimes = look;
			match(NUM);
			baseTail(lbName, ((Num*)numTimes)->val);
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
	move();
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
	lb_record* lr = new lb_record(lbName, times, len, cont);
	table.addlb(lr);
}

void Parser::type(list<int>& cont, int len){
	string name;
	lb_record* lr = NULL;
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
			name = ((Id*)look)->name;
			lr = table.getlb(name);
			if(scanLop == 2 && !lr->isEqu){
				obj.addRel(curSeg, lb_record::curAddr + cont.size() * len, name, R_386_32);
			}
			else{}
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
			move();
			type(cont, len);
			valTail(cont, len);
			break;
		default:
			return;
	}
}

void Parser::off(){
	if(look->tag == SUB || look->tag == ADD){
	}
	else{
		printf("addr err![line:%d]\n", lexer.getLine());
	}
}

ModRM modrm;
SIB sib;
Inst instr;

void Parser::inst(){
	Tag op = look->tag;
	int len = 0;
	if(op >= I_MOV && op <= I_LEA){
		int d_type = 0, s_type = 0;
		int regNum = 0;
		oprand(regNum, d_type, len);
		//leave comma match in oprand();
		oprand(regNum, s_type, len);
	}
	else if(op >= I_CALL && op <= I_POP){
		int type = 0, regNum = 0;
		oprand(regNum, type, len);
	}
	else if(op == I_RET){
		//todo
		move();
	}
	else{
		printf("opcode err[line:%d]\n", lexer.getLine());
	}
}

int Parser::getRegCode(Tag reg, int len){
	int code = 0;
	switch(len){
		case 4:
			code = reg - BR_AL;
			break;
		case 8:
			code = reg - DR_EAX;
			break;
	}
	return code;
}

lb_record* relLb = NULL;
void Parser::oprand(int& regNum, int& type, int& len){
	move();//skip inst or comma
	string name;
	lb_record* lr;
	switch(look->tag){
		case NUM:
			type = IMMEDIATE;
			instr.imm32 = ((Num*)look)->val;
			move();
			break;
		case ID:
			type = IMMEDIATE;
			name = ((Id*)look)->name;
			lr = table.getlb(name);
			instr.imm32 = lr->addr;
			if(scanLop == 2 && !lr->isEqu){
				relLb = lr;
			}
			else{}
			move();
			break;
		case LBRACK:
			type = MEMORY;
			mem();
			break;
		case SUB:
			move();
			type = IMMEDIATE;
			instr.imm32 = -((Num*)look)->val;
			match(NUM);
			break;
		default:
			type = REGISTER;
			len = reg();
			int regCode = getRegCode(look->tag, len);
			if(regNum++ != 0){
				modrm.mod = 3;
				modrm.rm = regCode;
			}
			else{
				modrm.reg = regCode;
			}
			move();//pass regToken to get next token
	}
}

int Parser::reg(){//need move token after call this fun
	if(look->tag >= BR_AL && look->tag <= BR_BH){
		return 1;
	}
	else if(look->tag >= DR_EAX && look->tag <= DR_EDI){
		return 4;
	}
	else{
		printf("reg err![line:%d]\n", lexer.getLine());
		return 0;
	}
}

void Parser::mem(){
	match(LBRACK);
	addr();
	match(RBRACK);
}

void Parser::addr(){
	string name;
	lb_record* lr;
	switch(look->tag){
		case NUM:
			modrm.mod = 0;
			modrm.rm = 5;
			instr.setDisp(((Num*)look)->val, 4);
			move();
			break;
		case ID:
			modrm.mod = 0;
			modrm.rm = 5;
			name = ((Id*)look)->name;
			lr = table.getlb(name);
			instr.setDisp(lr->addr, 4);
			if(scanLop == 2 && !lr->isEqu){
				relLb = lr;
			}
			else{}
			move();
			break;
		default:
			int type = reg();
			regAddr(look, type);
	}
}

void Parser::regAddr(Token* baseReg, int type){
	move();//pass regToken
	Token* op = look;
	Tag sign = op->tag;
	if(sign == SUB || sign == ADD){
		off();
		regAddrTail(baseReg, type, op);
	}
	else{// reg indirect addressing
		if(baseReg->tag == DR_ESP){//[esp]
			modrm.mod = 0;
			modrm.rm = 4;
			sib.scale = 4;
			sib.base = 4;
		}
		else if(baseReg->tag == DR_EBP){
			modrm.mod = 1;
			modrm.rm = 5;
			instr.setDisp(0, 1);
		}
		else{// normal reg
			modrm.mod = 0;
			modrm.rm = (baseReg->tag - BR_AL) - (1 - type % 4) * 8;
		}
	}
}

void Parser::regAddrTail(Token* baseReg, int type, Token* op){
	move();
	int disVal;
	switch(look->tag){
		case NUM:
			disVal = ((Num*)look)->val;
			disVal = op->tag == SUB ? -disVal : disVal;
			if(disVal >= -128 && disVal < 128){//disp 8
				modrm.mod = 1;
				instr.setDisp(disVal, 1);
			}
			else{
				modrm.mod = 2;
				instr.setDisp(disVal, 4);
			}
			modrm.rm = (baseReg->tag - BR_AL) - (1 - type % 4) * 8;

			if(baseReg->tag = DR_ESP){//sib
				modrm.rm = 4;
				sib.scale = 0;
				sib.index = 4;
				sib.base = 4;
			}
			else{}
			move();
			break;
		default:
			int typei = reg();
			modrm.mod = 0;
			modrm.rm = 4;
			sib.scale = 0;
			sib.index = (look->tag - BR_AL) - (1 - typei % 4) * 8;
			sib.index = (baseReg->tag - BR_AL) - (1 - type % 4) * 8;
			move();//pass regToken;
	}
}

void Parser::reset(){
	lexer.reset();
}

