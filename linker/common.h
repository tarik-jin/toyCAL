#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

struct string_hash{
	size_t operator()(const string& str) const{
		std::hash<std::string> hash_fn;
		return hash_fn(str);
	}
};

extern bool showLink;

