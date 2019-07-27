#include "symbol.h"
#include "token.h"

Fun::Fun(bool ext, Tag t, string n, vector<Var*>& paraList){
	externed = ext;
	type = t;
	name = n;
	paraVar = paraList;
	//to do
}

void Fun::enterScope(){
	scopeEsp.push_back(0);
	return;
}

void Fun::leaveScope(){
	maxDepth = (curEsp > maxDepth) ? curEsp : maxDepth;
	curEsp -= scopeEsp.back();
	scopeEsp.pop_back();
	return;
}
