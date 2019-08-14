#pragma once
#include "common.h"

class Block{
public:
	Block(vector<InterInst*>& codes);
	list<InterInst*> insts;
	list<Block*> prevs;
	list<Block*> succs;
	bool visited;
	bool canReach;

	vector<double> inVals;
	vector<double> outVals;
	void toString();
};

class DFG{

	void createBlocks();
	void linkBlocks();

	void release(Block* block);
	bool reachable(Block* block);
	bool __reachable(Block* block);
	void resetVisit();

public:

	DFG(InterCode& code);

	vector<InterInst*> codeList;
	vector<Block*> blocks;
	void delLink(Block* begin, Block* end);

	void toString();
	void toCode(list<InterInst*>& opt);
};
