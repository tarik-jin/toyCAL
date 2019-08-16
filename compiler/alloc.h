#pragma once
#include "common.h"
#include "set.h"
#include "symbol.h"

struct Node{
	Var* var;
	int degree;
	int color;
	Set exColors;
	vector<Node*> links;

	Node(Var* v, Set& E);
	void addLink(Node* node);
	void paint(Set& colorBox);
	void addExColor(int color);
};

class CoGraph{
	struct node_less{
		bool operator()(Node* left, Node* right){
			return left->degree <= right->degree;
		}
	};
	vector<Node*> nodes;
	vector<Var*> varList;
	Fun* fun;
	list<InterInst*> optCode;
	LiveVar* lv;
	Set U;
	Set E;
	static const int regNum = 8;
public:

	CoGraph(list<InterInst*>& optCode, vector<Var*>& para, LiveVar* lv, Fun* f);
	~CoGraph();

	void regAlloc();
	Node* pickNode();
};
