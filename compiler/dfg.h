#pragma once
#include "common.h"

class Block{
public:
	Block(vector<InterInst*>& codes);
	list<InterInst*> insts;
	list<Block*> prevs;
	list<Block*> succs;
};

class DFG{
	void createBlocks();
	void linkBlocks();
public:
	vector<InterInst*> codeList;
	vector<Block*> blocks;
};
