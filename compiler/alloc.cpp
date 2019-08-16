#include "alloc.h"
#include "livevar.h"
#include <algorithm>

Node::Node(Var* v, Set& E):var(v), degree(0), color(-1){
	exColors = E;
}

void Node::addLink(Node* node){
	vector<Node*>::iterator pos = lower_bound(links.begin(), links.end(), node);
	if(pos == links.end() || *pos != node){
		links.insert(pos, node);
		degree++;
	}
	else{}
}

void Node::paint(Set& colorBox){
	Set availColors = colorBox - exColors;
	for(int i = 0; i < availColors.count; i++){
		if(availColors.get(i)){
			color = i;
			var->regId = color;
			degree = -1;
			for(int j = 0; j < links.size(); j++){
				links[j]->addExColor(color);
			}
			return;
		}
		else{}//continue
	}
	degree = -1;
}

void Node::addExColor(int color){
	if(degree == -1){
		return;
	}
	else{
		exColors.set(color);
		degree--;
		return;
	}
}

CoGraph::CoGraph(list<InterInst*>& optCode, vector<Var*>& para, LiveVar* lv, Fun* f){
	fun = f;
	this->optCode = optCode;
	this->lv = lv;

	U.init(regNum, 1);
	E.init(regNum, 0);
	for(unsigned int i = 0; i < para.size(); i++){
		varList.push_back(para[i]);
	}
	for(list<InterInst*>::iterator i = optCode.begin(); i != optCode.end(); i++){
		InterInst* inst = *i;
		Operator op = inst->getOp();
		if(op == OP_DEC){
			Var* arg1 = inst->getArg1();
			varList.push_back(arg1);
		}
		else if(op == OP_LEA){
			Var* arg1 = inst->getArg1();
			if(arg1){
				arg1->inMem = true;
			}
			else{}
		}
		else{}//skip other inst
	}
	Set& liveE = lv->getE();
	Set mask = liveE;
	for(unsigned int i = 0; i < varList.size(); i++){
		mask.set(varList[i]->index);
	}
	for(unsigned int i = 0; i < varList.size(); i++){
		Node* node;
		if(varList[i]->getArray() || varList[i]->inMem){
			node = new Node(varList[i], U);
		}
		else{
			node = new Node(varList[i], E);
		}
		varList[i]->index = i;
		nodes.push_back(node);
	}
	Set buf = liveE;
	list<InterInst*>::reverse_iterator i;
	for(i = optCode.rbegin(); i != optCode.rend(); i++){
		Set& liveout = (*i)->liveInfo.out;
		if(liveout != buf){
			buf = liveout;
			vector<Var*> coVar = lv->getCoVar(liveout & mask);
			for(int j = 0; j < coVar.size() - 1; j++){
				for(int k = j + 1; j < coVar.size(); k++){
					nodes[coVar[j]->index]->addLink(nodes[coVar[k]->index]);
					nodes[coVar[k]->index]->addLink(nodes[coVar[j]->index]);
				}
			}
		}
		else{}//avoid duplicate compute for liveout
	}
}

CoGraph::~CoGraph(){
	//todo
}

void CoGraph::regAlloc(){
	Set colorBox = U;
	int nodeNum = nodes.size();
	for(int i = 0; i < nodeNum; i++){
		Node* node = pickNode();
		node->paint(colorBox);
	}
}

Node* CoGraph::pickNode(){
	make_heap(nodes.begin(), nodes.end(), node_less());
	Node* node = nodes.front();
	return node;
}
