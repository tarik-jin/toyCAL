#pragma once
#include "common.h"

class Set{
	vector<unsigned int> bmList;
public:
	int count;

	Set();
	Set(int size, bool val);
	void init(int size, bool val);

	Set operator -(Set val);
	Set operator |(Set val);
	Set operator &(Set val);
	bool operator !=(Set& val);
	void set(int i);
	bool get(int i);
};

struct CopyInfo{
	Set in;
	Set out;
	Set gen;
	Set kill;
};

struct LiveInfo{
	Set in;
	Set out;
	Set use;
	Set def;
};
