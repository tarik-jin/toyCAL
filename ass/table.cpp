#include "table.h"
#include "token.h"

int lb_record::curAddr = 0x00000000;
lb_record::lb_record(string n, bool ex){//L: or(L dd @esp)
	lbName = n;
	addr = lb_record::curAddr;
	externed = ex;
	isEqu = false;
	segName = curSeg;
	times = 0;
	len = 0;
	if(ex){
		addr = 0;
		segName = "";
	}
	else{}
}

lb_record::lb_record(string n, int a){//L equ 1
	lbName = n;
	segName = curSeg;
	addr = a;//sepcial case for macro
	isEqu = false;
	externed = false;
	times = 0;
	len = 0;
}

lb_record::lb_record(string n, int t, int l, list<int>& c){//L times 10 dw , "1234"
	lbName = n;
	addr = lb_record::curAddr;
	segName = curSeg;
	isEqu = false;
	times = t;
	len = l;
	for(list<int>::iterator it = c.begin(); it != c.end(); it++){
		cont.push_back(*it);
	}
	externed = false;
	lb_record::curAddr += t * l * c.size();
}

Table table;
void Table::addlb(lb_record* p_lb){
	if(scanLop != 1){
		delete p_lb;
		return;
	}
	else{
		if(hasName(p_lb->lbName)){
			delete lb_map[p_lb->lbName];
		}
		else{}
		lb_map[p_lb->lbName] = p_lb;
		if(p_lb->times != 0 && p_lb->segName == ".data"){
			defLbs.push_back(p_lb);
		}
		else{}
	}
}

lb_record* Table::getlb(string name){
	lb_record* ret;
	if(hasName(name)){
		ret = lb_map[name];
	}
	else{
		ret = new lb_record(name, true);
		lb_map[name] = ret;
	}
	return ret;
}

bool Table::hasName(string name){
	return(lb_map.find(name) != lb_map.end());
}

void Table::switchSeg(Token* look){
	if(scanLop == 1){
		dataLen += (4 - dataLen % 4) % 4;
		obj.addShdr(curSeg, lb_record::curAddr);
		dataLen += lb_record::curAddr;
	}
	if(look->tag != END){
		curSeg = ((Str*)look)->str;
	}
	else{
		curSeg = "";
	}
	lb_record::curAddr = 0;
}

void Table::exportSyms(){
	unordered_map<string, lb_record*, string_hash>::iterator lb_i, lb_iend;
	lb_i = lb_map.begin();
	lb_iend = lb_map.end();
	for(; lb_i != lb_iend; lb_i++){
		lb_record* lr = lb_i->second;
		if(!lr->isEqu){
			obj.addSym(lr);
		}
		else{}
	}
}
