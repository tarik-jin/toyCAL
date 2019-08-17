#pragma once
#include "common.h"
#include "set.h"

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

struct Scope{
	struct scope_less{
		bool operator()(Scope* left, Scope* right){
			return left->id < right->id;
		}
	};
	int id;
	int esp;
	vector<Scope*> children;
	Scope* parent;

	Scope(int i, int addr);
	~Scope();
	Scope* find(int i);
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
	Scope* scRoot;
	void regAlloc();
	Node* pickNode();
	void stackAlloc();
	int& getEsp(vector<int>& path);
public:

	CoGraph(list<InterInst*>& optCode, vector<Var*>& para, LiveVar* lv, Fun* f);
	~CoGraph();

};

