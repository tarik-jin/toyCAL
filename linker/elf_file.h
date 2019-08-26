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
	string shstrtab;
	string strtab;

public:
	void readElf(const string dir);
	void getData(char* buf, Elf32_Off offset, Elf32_Word size);
	void assemObj(Linker* linker);
	void addSym(string st_name, Elf32_Sym*);
	void addPhdr(Elf32_Word type, Elf32_Off off, Elf32_Addr vaddr, Elf32_Word filesz,
			Elf32_Word memsz, Elf32_Word flags, Elf32_Word align);
	void addShdr(
        string sh_name,
        Elf32_Word sh_type,
        Elf32_Word sh_flags,
        Elf32_Addr sh_addr,
        Elf32_Off sh_offset,
        Elf32_Word sh_size,
        Elf32_Word sh_link,
        Elf32_Word sh_info,
        Elf32_Word sh_addralign,
        Elf32_Word sh_entsize);
	void writeElf(Linker* linker, const char* dir);

};
