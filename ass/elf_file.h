#pragma once
#include "common.h"
#include "elf.h"

struct RelItem{
	string segName;
	Elf32_Rel* rel;
	string relName;
	RelItem(string seg, int addr, string lb, int t);
};

class Elf_file{
	struct string_hash{
        size_t operator()(const string& str) const{
            std::hash<std::string> hash_fn;
            return hash_fn(str);
        }
    };
	Elf32_Ehdr ehdr;
	vector<Elf32_Phdr*> phdrTab;
	unordered_map<string, Elf32_Shdr*, string_hash> shdrTab;
	vector<string> shdrNames;
	unordered_map<string, Elf32_Sym*, string_hash> symTab;
	vector<string> symNames;
	vector<RelItem*> relTab;
	string shstrtab;
	string strtab;
public:
	Elf_file();
	~Elf_file();
	void addShdr(string sh_name, int size);
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
	void addSym(lb_record* lb);
	int getSegIndex(string segName);
	void printAll();
	RelItem* addRel(string seg, int addr, string lb, int type);
};
