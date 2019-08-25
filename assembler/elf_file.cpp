#include "elf_file.h"
#include "table.h"
#include "elf.h"

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
	bool glb = false;
	string name = lb->lbName;
	if(lb->segName == ".text"){
		if(name == "@start"){
			glb = true;
		}
		else{
			glb = (name[0] != '@') ? true : glb;
		}
	}
	else if(lb->segName == ".data"){
		glb = true;
	}
	else if(lb->segName == ""){ //external symbol
		glb = lb->externed;
	}
	else{}
	Elf32_Sym* sym = new Elf32_Sym();
	sym->st_name = 0;
	sym->st_value = lb->addr;
	sym->st_size =  lb->times * lb->len * lb->cont.size();
	if(glb)	{
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
				cout << "\tsymbolName:" << i->first << "\t";
				cout << endl;
			}
			else{}
		}
		cout << "-------relocation info-----" << endl;
		for(vector<RelItem*>::iterator i = relTab.begin(); i != relTab.end(); i++){
			cout << "relLocSec:" << (*i)->segName << "\t";
			cout << "relSecOffset:" << setw(4) << (*i)->rel->r_offset;
			cout << "\trelSymName:" << (*i)->relName << "\t";
			cout <<	endl;
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

void Elf_file::assemObj(){
	//all section name
	vector<string> AllSegNames = shdrNames;
	AllSegNames.push_back(".shstrtab");
	AllSegNames.push_back(".symtab");
	AllSegNames.push_back(".strtab");
	AllSegNames.push_back(".rel.text");
	AllSegNames.push_back(".rel.data");

	//section index
	unordered_map<string, int, string_hash> shIndex;
	//section name index
	unordered_map<string, int, string_hash> shstrIndex;
	//form index;
	for(int i = 0; i < AllSegNames.size(); i++){
		string name = AllSegNames[i];
		shIndex[name] = i;
		shstrIndex[name] = shstrtab.size();
		shstrtab += name; //save section name
		shstrtab.push_back('\0');
	}

	//symbol index
	unordered_map<string, int, string_hash> symIndex;
	//symbol name index;
	unordered_map<string, int, string_hash> strIndex;
	//form index;
	for(int i = 0; i < symNames.size(); i++){
		string name = symNames[i];
		symIndex[name] = i;
		strIndex[name] = strtab.size();
		strtab += name; //save symbol name
		strtab.push_back('\0');
	}

	//update symbol table symbol index
	for(int i = 0; i < symNames.size(); i++){
		string name = symNames[i];
		symTab[name]->st_name = strIndex[name];
	}

	//handle relocation table
	for(int i = 0; i < relTab.size(); i++){
		Elf32_Rel* rel = new Elf32_Rel();
		rel->r_offset = relTab[i]->rel->r_offset;//relocation addr
		rel->r_info = ELF32_R_INFO(
				symIndex[relTab[i]->relName],//reloction symbol
				ELF32_R_TYPE(relTab[i]->rel->r_info));//reloction type

		if(relTab[i]->segName == ".text"){
			relTextTab.push_back(rel);
		}
		else if(relTab[i]->segName == ".data"){
			relDataTab.push_back(rel);
		}
		else{
			delete rel;
		}
	}

	char magic[] = {
		0x7f, 0x45, 0x4c, 0x46,
		0x01, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	memcpy(&ehdr.e_ident, magic, sizeof(magic));
	ehdr.e_type = ET_REL;
	ehdr.e_machine = EM_386;
	ehdr.e_version = EV_CURRENT;
	ehdr.e_entry = 0;
	ehdr.e_phoff = 0;
	ehdr.e_shoff = 0;
	ehdr.e_flags = 0;
	ehdr.e_ehsize = sizeof(Elf32_Ehdr);
	ehdr.e_phentsize = 0;
	ehdr.e_phnum = 0;
	ehdr.e_shentsize = sizeof(Elf32_Shdr);
	ehdr.e_shnum = AllSegNames.size();
	ehdr.e_shstrndx = shIndex[".shstrtab"];

	int curOff = sizeof(ehdr);//ELF header
	dataLen += (4 - dataLen % 4) % 4;
	curOff += dataLen;//align exsiting section

	//add new section table entry
	addShdr(".shstrtab", SHT_STRTAB, 0, 0, curOff, shstrtab.size(), SHN_UNDEF, 0, 1, 0);
	curOff += shstrtab.size();
	curOff += (4 - curOff % 4) % 4;//align

	ehdr.e_shoff = curOff;
	curOff += ehdr.e_shnum * ehdr.e_shentsize;

	addShdr(".symtab", SHT_SYMTAB, 0, 0, curOff, symNames.size() * sizeof(Elf32_Sym),
			shIndex[".strtab"], 0, 1, sizeof(Elf32_Sym));
	curOff += symNames.size() * sizeof(Elf32_Sym);

	addShdr(".strtab", SHT_STRTAB, 0, 0, curOff, strtab.size(), SHN_UNDEF, 0, 1, 0);
	curOff += strtab.size();
	curOff += (4 - curOff % 4) % 4;

	addShdr(".rel.text", SHT_REL, 0, 0, curOff,
			relTextTab.size() * sizeof(Elf32_Rel),
			shIndex[".symtab"], shIndex[".text"], 1,
			sizeof(Elf32_Rel));
	curOff += relTextTab.size() * sizeof(Elf32_Rel);

	addShdr(".rel.data", SHT_REL, 0, 0, curOff,
			relDataTab.size() * sizeof(Elf32_Rel),
			shIndex[".symtab"], shIndex[".data"], 1,
			sizeof(Elf32_Rel));
	curOff += relDataTab.size() * sizeof(Elf32_Rel);

	//update section table sectionName index
	for(int i = 0; i < AllSegNames.size(); i++){
		string name = AllSegNames[i];
		shdrTab[name]->sh_name = shstrIndex[name];
	}

}

void Elf_file::writeElf(){
	int padNum = 0;
	char pad[1] = {0};

	//ELF Header
	fwrite(&ehdr, ehdr.e_ehsize, 1, fout);

	//.text
	char buffer[1024] = {0};
	int count = -1;
	fclose(ftmp);
	string tmpFile(finName);
	tmpFile.erase(tmpFile.end() - 1);
	ftmp = fopen((tmpFile + "t").c_str(), "r");
	while(count){
		count = fread(buffer, 1, 1024, ftmp);
		fwrite(buffer, 1, count, fout);
	}
	shdrTab[".text"]->sh_offset = ehdr.e_ehsize;

	//.data
	padNum = (4 - (ehdr.e_ehsize + shdrTab[".text"]->sh_size) % 4) % 4;
	fwrite(pad, sizeof(pad), padNum, fout);
	table.write(fout);
	shdrTab[".data"]->sh_offset = ehdr.e_ehsize + shdrTab[".text"]->sh_size + padNum;

	//.shstrtab
	padNum = shdrTab[".shstrtab"]->sh_offset
		- shdrTab[".data"]->sh_offset
		- shdrTab[".data"]->sh_size;
	fwrite(pad, sizeof(pad), padNum, fout);
	fwrite(shstrtab.c_str(), shstrtab.size(), 1, fout);

	//section table
	padNum = ehdr.e_shoff
		- shdrTab[".shstrtab"]->sh_offset
		- shdrTab[".shstrtab"]->sh_size;
	fwrite(pad, sizeof(pad), padNum, fout);
	for(int i = 0; i < shdrNames.size(); i++){
		Elf32_Shdr* sh = shdrTab[shdrNames[i]];
		fwrite(sh, ehdr.e_shentsize, 1, fout);
	}

	//symbol table
	for(int i = 0; i < symNames.size(); i++){
		Elf32_Sym* sym = symTab[symNames[i]];
		fwrite(sym, sizeof(Elf32_Sym), 1, fout);
	}

	//.strtab
	fwrite(strtab.c_str(), strtab.size(), 1, fout);

	//.rel.text
	padNum = shdrTab[".rel.text"]->sh_offset
		- shdrTab[".strtab"]->sh_offset
		- shdrTab[".strtab"]->sh_size;
	fwrite(pad, sizeof(pad), padNum, fout);
	for(int i = 0; i < relTextTab.size(); i++){
		Elf32_Rel* rel = relTextTab[i];
		fwrite(rel, sizeof(Elf32_Rel), 1, fout);
	}

	//.rel.data
	for(int i = 0; i < relDataTab.size(); i++){
		Elf32_Rel* rel = relDataTab[i];
		fwrite(rel, sizeof(Elf32_Rel), 1, fout);
	}
}
