#include "dfg.h"
#include "intercode.h"

Block::Block(vector<InterInst*>& codes):visited(false), canReach(true){
	for(unsigned int i = 0; i < codes.size(); i++){
		codes[i]->block = this;
		insts.push_back(codes[i]);
	}
}

void Block::toString(){
	printf("-------%p-------\n", (void *)this);
	printf("prevs: ");
	for(list<Block*>::iterator i = prevs.begin(); i != prevs.end(); i++){
		printf("%p ", (void *)(*i));
	}
	printf("\n");
	printf("succs: ");
	for(list<Block*>::iterator i = succs.begin(); i != succs.end(); i++){
		printf("%p ", (void *)(*i));
	}
	printf("\n");
	for(list<InterInst*>::iterator i = insts.begin(); i != insts.end(); i++){
		(*i)->toString();
	}
	printf("----------------\n");
}

DFG::DFG(InterCode& code){
	code.markFirst();
	codeList = code.getCode();
	createBlocks();
	linkBlocks();
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
	if(block == blocks[0]){
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

void DFG::toString(){
	for(unsigned int i = 0; i < blocks.size(); i++){
		blocks[i]->toString();
	}
}

void DFG::toCode(list<InterInst*>& opt){
	opt.clear();
	for(unsigned int i = 0; i < blocks.size(); i++){
		if(reachable(blocks[i])){
			list<InterInst*> tmpInsts;
			for(list<InterInst*>::iterator it = blocks[i]->insts.begin(); it != blocks[i]->insts.end(); it++){
				if(!(*it)->isDead){
					tmpInsts.push_back(*it);
				}
				else{}//skip dead code
			}
			opt.splice(opt.end(), tmpInsts);
		}
		else{
			blocks[i]->canReach = false;
		}
	}
}
