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
