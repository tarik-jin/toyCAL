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

	void release(Block* block);
	bool reachable(Block* block);
	bool __reachable(Block* block);
	void resetVisit();

public:

	vector<InterInst*> codeList;
	vector<Block*> blocks;
	void deLink(Block* begin, Block* end);

};
