#pragma once
#include "common.h"
#include "elf_file.h"

struct Block{
	char* data;//record binary data start address
	unsigned int offset;
	unsigned int size;
	Block(char* d, unsigned int off, unsigned int s);
	~Block();
};

struct SegList{
	unsigned int baseAddr;
	unsigned int begin;
	unsigned int offset;
	unsigned int size;
	vector<Elf_file*> ownerList;
	vector<Block*> blocks;

	void allocAddr(string name, unsigned int& base, unsigned int& off);
	void relocAddr(unsigned int relAddr, unsigned char type, unsigned int symAddr);
};

struct SymLink{
	string name;
	Elf_file* recv;
	Elf_file* prov;
};

#define BASE_ADDR 0x08048000
#define MEM_ALIGN 4096
#define DISC_ALIGN 4
#define TEXT_ALIGN 16

class Linker{
	unordered_map<string, SegList*, string_hash> segList;
	vector<SymLink*> symLinks;
	vector<SymLink*> symDef;
	vector<Elf_file*> elfs;
	vector<string> segNames;
	unordered_map<string, SegList*, string_hash> segLists;
public:
	void allocAddr();
	void addElf(const char* dir);
};

