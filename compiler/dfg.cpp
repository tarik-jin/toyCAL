#include "dfg.h"
#include "intercode.h"

Block::Block(vector<InterInst*>& codes){
	for(unsigned int i = 0; i < codes.size(); i++){
		codes[i]->block = this;
		insts.push_back(codes[i]);
	}
}

void DFG::createBlocks(){
	vector<InterInst*> tmpList;
	tmpList.push_back(codeList[0]);
	for(unsigned int i = 1; i < codeList.size(); ++i){
		if(codeList[i]->isFirst()){
			blocks.push_back(new Block(tmpList));
			tmpList.clear();
		}
		else{
		}
		tmpList.push_back(codeList[i]);
	}
	blocks.push_back(new Block(tmpList));
}

void DFG::linkBlocks(){
	for(unsigned int i = 0; i < blocks.size(); i++){
		InterInst* last = blocks[i]->insts.back();
		if(last->isJmp() || last->isJcond()){
			Block* tar = last->getTarget()->block;
			blocks[i]->succs.push_back(tar);
			tar->prevs.push_back(blocks[i]);
		}
		else{
		}
		if(!last->isJmp() && i != blocks.size() - 1){
			blocks[i]->succs.push_back(blocks[i + 1]);
			blocks[i + 1]->prevs.push_back(blocks[i]);
		}
		else{
		}
	}
}

void DFG::delLink(Block* begin, Block* end){
	if(begin){
		begin->succs.remove(end);
		end->prevs.remove(begin);
	}
	else{}//no sense;
	release(end);
}

void DFG::release(Block* block){
	if(!reachable(block)){
		list<Block*> delList;
		list<Block*>::iterator i;
		for(i = block->succs.begin(); i != block->succs.end(); i++){
			delList.push_back(*i);
		}
		for(i = delList.begin(); i != delList.end(); i++){
			block->succs.remove(*i);
			(*i)->prevs.remove(block);
		}
		for(i = delList.begin(); i != delList.end(); i++){
			release(*i);
		}
	}
	else{}
}

bool DFG::reachable(Block* block){
	resetVisit();
	return __reachable(block);
}

bool DFG::__reachable(Block *block){
	if(block == block[0]){
		return true;
	}
	else if(block->visited){//this block has been visited, prevent dead loop
		return false;
	}
	else{
		block->visited = true;
		bool flag = false;
		list<Block*>::iterator i;
		for(i = block->prevs.begin(); i != block->prevs.end(); i++){
			Block* prev = *i;
			flag = __reachable(prev);
			if(flag){
				return true;
			}
			else{}
		}
		return flag;
	}
}

void DFG::resetVisit(){
	for(unsigned int i = 0; i < blocks.size(); i++){
		blocks[i]->visited = false;
	}
}
