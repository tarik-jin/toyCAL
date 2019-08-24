#pragma once
#include "common.h"
#include "elf.h"

struct RelItem{
	string segName;
	Elf32_Rel* rel;
	string relName;
	RelItem(string segName, Elf32_Rel* r, string symName);
	~RelItem();
};

class Elf_file{
public:
	Elf32_Ehdr ehdr;
	string elf_dir;
	vector<Elf32_Phdr*> phdrTab;
	vector<string> shdrNames;
	unordered_map<string, Elf32_Shdr*, string_hash> shdrTab;
	vector<string> symNames;
	unordered_map<string, Elf32_Sym*, string_hash> symTab;
	vector<RelItem*> relTab;

public:
	void readElf(const string dir);
	void getData(char* buf, Elf32_Off offset, Elf32_Word size);
};
