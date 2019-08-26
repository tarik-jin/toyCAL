#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <string.h>
using namespace std;

#define BASE_ADDR 0x08048000
#define MEM_ALIGN 4096
#define DISC_ALIGN 4
#define TEXT_ALIGN 16
#define START "@start"

struct string_hash{
	size_t operator()(const string& str) const{
		std::hash<std::string> hash_fn;
		return hash_fn(str);
	}
};

extern bool showLink;

class Linker;

