#include "elf_file.h"
#include "table.h"

Elf_file obj;

Elf_file::Elf_file(){
	shstrtab = "";
	strtab = "";
	addShdr("",0,0,0,0,0,0,0,0,0);//empty section entry
	//add an empty symbol
	Elf32_Sym* sym = new Elf32_Sym();
	string strEmpty = "";
	symTab[strEmpty] = sym;
	sym->st_name = 0;
	sym->st_value = 0;
	sym->st_size = 0;
	sym->st_info = 0;
	sym->st_other = 0;
	sym->st_shndx = 0;
	symNames.push_back(strEmpty);
}

Elf_file::~Elf_file(){
	unordered_map<string, Elf32_Shdr*, string_hash>::iterator i;
	i = shdrTab.begin();
	for(; i != shdrTab.end(); i++){
		delete i->second;
	}
	shdrTab.clear();
	shdrNames.clear();
	unordered_map<string, Elf32_Sym*, string_hash>::iterator ii;
	ii = symTab.begin();
	for(; ii != symTab.end(); ii++){
		delete ii->second;
	}
	symTab.clear();
	vector<RelItem*>::iterator it = relTab.begin();
	for(; it != relTab.end(); it++){
		delete *it;
	}
	relTab.clear();
}

void Elf_file::addShdr(string sh_name, int size){
	int off = 52 + dataLen;//52 is the size of elf Header
	if(sh_name == ".text"){
		addShdr(sh_name, SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR, 0, off, size, 0, 0, 4, 0);
	}
	else if(sh_name == ".data"){
		addShdr(sh_name, SHT_PROGBITS, SHF_ALLOC | SHF_WRITE, 0, off, size, 0, 0, 4, 0);
	}
	else{}
}

void Elf_file::addShdr(
		string sh_name,
		Elf32_Word sh_type,
		Elf32_Word sh_flags,
		Elf32_Addr sh_addr,
		Elf32_Off sh_offset,
		Elf32_Word sh_size,
	   	Elf32_Word sh_link,
		Elf32_Word sh_info,
		Elf32_Word sh_addralign,
		Elf32_Word sh_entsize){
	Elf32_Shdr* sh = new Elf32_Shdr();
	sh->sh_name = 0;
	sh->sh_type = sh_type;
	sh->sh_flags = sh_flags;
	sh->sh_addr = sh_addr;
	sh->sh_offset = sh_offset;
	sh->sh_size = sh_size;
	sh->sh_link = sh_link;
	sh->sh_info = sh_info;
	sh->sh_addralign = sh_addralign;
	sh->sh_entsize = sh_entsize;
	shdrTab[sh_name] = sh;
	shdrNames.push_back(sh_name);
}

void Elf_file::addSym(lb_record* lb){
	Elf32_Sym* sym = new Elf32_Sym();
	sym->st_name = 0;
	sym->st_value = lb->addr;
	sym->st_size =  lb->times * lb->len * lb->cont.size();
	if(lb->global)	{
		sym->st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);
	}
	else{
		sym->st_info = ELF32_ST_INFO(STB_LOCAL, STT_NOTYPE);
	}
	sym->st_other = 0;
	if(lb->externed){
		sym->st_shndx = STN_UNDEF;
	}
	else{
		sym->st_shndx = getSegIndex(lb->segName);
	}
	symTab[lb->lbName] = sym;
	symNames.push_back(lb->lbName);
}

int Elf_file::getSegIndex(string segName){
	int index = 0;
	for(int i = 0; i < shdrNames.size(); i++){
		if(shdrNames[i] == segName)	{
			break;
		}
		else{
			++index;
		}
	}
	return index;
}

void Elf_file::printAll(){
	if(showAss){
		cout << "-------section info-------" << endl;
		for(unordered_map<string, Elf32_Shdr*, string_hash>::iterator i = shdrTab.begin(); i != shdrTab.end(); i++){
			if(i->first != ""){
				cout << "sectionName:" << i->first << "\t";
				cout << "offset:" << i->second->sh_offset << "\t";
				cout << "size:" << i->second->sh_size << endl;
			}
			else{}
		}
		cout << "-------symbol info-------" << endl;
		for(unordered_map<string, Elf32_Sym*, string_hash>::iterator i = symTab.begin(); i != symTab.end(); i++){
			if(i->first != ""){
				cout << "symbolName:" << i->first << "\t";
				if(i->second->st_shndx == 0){
					cout << "external\t";
				}
				else{
					cout << "defHere \t";
				}
				cout << "inSection:" << shdrNames[i->second->st_shndx]  << "\t";
				cout << "secOffset:" << i->second->st_value << "\t";
				if(ELF32_ST_BIND(i->second->st_info) == STB_GLOBAL){
					cout << "global";
				}
				else if(ELF32_ST_BIND(i->second->st_info) == STB_LOCAL){
					cout << "local";
				}
				cout << endl;
			}
			else{}
		}
		cout << "-------relocation info-----" << endl;
		for(vector<RelItem*>::iterator i = relTab.begin(); i != relTab.end(); i++){
			cout << "relSymName:" << (*i)->relName << "\t";
			cout << "relLocSec:" << (*i)->segName << "\t";
			cout << "relSecOffset:" << (*i)->rel->r_offset << endl;
		}
	}
	else{
		return;
	}
}

RelItem* Elf_file::addRel(string seg, int addr, string lb, int type){
	RelItem* rel = new RelItem(seg, addr, lb, type);
	relTab.push_back(rel);
	return rel;
}

RelItem::RelItem(string seg, int addr, string lb, int t){
	segName = seg;
	relName = lb;
	rel = new Elf32_Rel();
	rel->r_offset = addr;
	rel->r_info = t;
}
