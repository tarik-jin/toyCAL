#include "linker.h"
#include "elf.h"

Block::Block(char* d, unsigned int off, unsigned int s){
	data = d;
	offset = off;
	size = s;
}

Block::~Block(){
	delete []data;
}

SegList::~SegList(){
	ownerList.clear();
	for(int i = 0; i < blocks.size(); i++){
		delete blocks[i];
	}
	blocks.clear();
}

void SegList::allocAddr(string name, unsigned int& base, unsigned int& off){
	begin = off;//record offset befor align

	int align = (name == ".text" ? TEXT_ALIGN : DISC_ALIGN);
	off += (align - off % align) % align;
	base += (MEM_ALIGN - base % MEM_ALIGN) % MEM_ALIGN + off % MEM_ALIGN;

	baseAddr = base;
	offset = off;
	size = 0;

	for(int i = 0; i < ownerList.size(); i++){
		Elf32_Shdr* seg = ownerList[i]->shdrTab[name];
		int sh_align = seg->sh_addralign;
		size += (sh_align - size % sh_align) % sh_align;

		char* buf = new char[seg->sh_size];
		ownerList[i]->getData(buf, seg->sh_offset, seg->sh_size);
		blocks.push_back(new Block(buf, size, seg->sh_size));

		seg->sh_addr = base + size;
		size += seg->sh_size;
	}
	base += size;
	off += size;
}

void SegList::relocAddr(unsigned int relAddr, unsigned char type, unsigned int symAddr){
	//find relocate location
	unsigned int  relOffset = relAddr - baseAddr;
	Block* block = NULL;

	int idx = 0;
	unsigned int start = blocks[idx]->offset;
	unsigned int end = start + blocks[idx]->size;
	while(start > relOffset || relOffset >= end){
		idx++;
		start = blocks[idx]->offset;
		end = start + blocks[idx]->size;
	}
	block = blocks[idx];

	int* pAddr = (int*)(block->data + relOffset - block->offset);
	if(type == R_386_32){
		if(showLink){
			printf("absolute relocation: orginAddr val=%08x\t", *pAddr);
		}
		else{}
		*pAddr = symAddr;
		if(showLink){
			printf("after relocate addrVal=%08x\n", *pAddr);
		}
		else{}
	}
	else if(type == R_386_PC32){
		if(showLink){
			printf("relative relocation: orginAddr val=%08x\t", *pAddr);
		}
		else{}
		*pAddr = symAddr - relAddr + *pAddr;
		if(showLink){
			printf("after relocate addrVal=%08x\n", *pAddr);
		}
		else{}
	}
	else{}
}

Linker::Linker(){
	segNames.push_back(".text");
	segNames.push_back(".data");
	for(int i = 0; i < segNames.size(); i++){
		segLists[segNames[i]] = new SegList();
	}
}

Linker::~Linker(){
	for(unordered_map<string, SegList*, string_hash>::iterator i = segLists.begin();
			i != segLists.end(); i++){
		delete i->second;
	}
	segLists.clear();
	for(vector<SymLink*>::iterator i = symLinks.begin(); i != symLinks.end(); i++){
		delete *i;
	}
	symLinks.clear();
	for(vector<SymLink*>::iterator i = symDef.begin(); i != symDef.end(); i++){
		delete *i;
	}
	symDef.clear();
	for(int i = 0; i < elfs.size(); i++){
		delete elfs[i];
	}
	elfs.clear();
}

void Linker::allocAddr(){
	unsigned int curAddr = BASE_ADDR;
	unsigned int curOff = 52 + sizeof(Elf32_Phdr) * segNames.size();
	if(showLink){
		printf("-------address allocate-------\n");
	}
	else{}
	for(int i = 0; i < segNames.size(); i++){
		segLists[segNames[i]]->allocAddr(segNames[i], curAddr, curOff);
		if(showLink){
			printf("%s\taddr=%08x\toff=%08x\tsize=%08x(%d)\n", segNames[i].c_str(),
					segLists[segNames[i]]->baseAddr, segLists[segNames[i]]->offset,
					segLists[segNames[i]]->size, segLists[segNames[i]]->size);
		}
		else{}
	}
}

void Linker::addElf(const char* dir){
	Elf_file* elf = new Elf_file();
	elf->readElf(dir);
	elfs.push_back(elf);
}

void Linker::collectInfo(){
	for(int i = 0; i < elfs.size(); i++){
		Elf_file* elf = elfs[i];
		//record section table info
		for(int i = 0; i < segNames.size(); i++){
			if(elf->shdrTab.find(segNames[i]) != elf->shdrTab.end()){
				segLists[segNames[i]]->ownerList.push_back(elf);
			}
			else{}
		}
		//record symbol reference info
		for(unordered_map<string, Elf32_Sym*, string_hash>::iterator symIt = elf->symTab.begin();
				symIt != elf->symTab.end(); symIt++){
			if(ELF32_ST_BIND(symIt->second->st_info) == STB_GLOBAL){
				SymLink* symLink = new SymLink();
				symLink->name = symIt->first;
				if(symIt->second->st_shndx == STN_UNDEF){
					symLink->recv = elf;
					symLink->prov = NULL;
					symLinks.push_back(symLink);
				}
				else{
					symLink->recv = NULL;
					symLink->prov = elf;
					symDef.push_back(symLink);
				}
			}
			else{}//only handle global symbol
		}
	}

}

bool Linker::symValid(){
	bool flag = true;
	startOwner = NULL;
	for(int i = 0; i < symDef.size(); i++){
		//search for start entry
		if(symDef[i]->name == START){
			startOwner = symDef[i]->prov;
		}
		else{}
		for(int j = i + 1; j < symDef.size(); j++){
			//symbol redefine
			if(symDef[i]->name == symDef[j]->name){
				printf("symbol %s redefinition in %s and %s.\n", symDef[i]->name.c_str(),
						symDef[i]->prov->elf_dir.c_str(), symDef[j]->prov->elf_dir.c_str());
				flag = false;
			}
			else{}//only search for symbol redefine
		}

	}
	if(startOwner == NULL){
		printf("can not find entrypoint symbol %s.\n", START);
		flag = false;
	}
	else{}//find START symbol location
	for(int i = 0; i < symLinks.size(); ++i){
		int idx = 0;//search for symbol def
		while(idx < symDef.size() && symLinks[i]->name != symDef[idx]->name){
			idx++;
		}
		if(idx == symDef.size()){
			printf("undefined symbol %s in %s.\n", symDef[i]->name.c_str(), symLinks[i]->recv->elf_dir.c_str());
			flag = false;
		}
		else{
			symLinks[i]->prov = symDef[idx]->prov;
		}
	}
	return flag;
}

void Linker::symParser(){
	if(showLink){
		printf("-------local symbol parser-------\n");
	}
	else{}
	for(int i = 0; i < symDef.size(); i++){
		Elf32_Sym* sym = symDef[i]->prov->symTab[symDef[i]->name];
		string segName = symDef[i]->prov->shdrNames[sym->st_shndx];
		sym->st_value += symDef[i]->prov->shdrTab[segName]->sh_addr;
		if(showLink){
			printf("%s\t%08x\t%s\n", symDef[i]->name.c_str(), sym->st_value, symDef[i]->prov->elf_dir.c_str());
		}
		else{}
	}
	if(showLink){
		printf("-------global symbo parser-------\n");
	}
	else{}
	for(int i = 0; i < symLinks.size(); i++){
		string name = symLinks[i]->name;
		Elf32_Sym* provsym = symLinks[i]->prov->symTab[name];
		Elf32_Sym* recvsym = symLinks[i]->recv->symTab[name];
		recvsym->st_value = provsym->st_value;
		if(showLink){
			printf("%s\t%08x\t%s\n", name.c_str(), recvsym->st_value, symLinks[i]->recv->elf_dir.c_str());
		}
		else{}
	}
}

bool Linker::link(const char* dir){
	collectInfo();
	if(!symValid()){
		return false;
	}
	else{
		allocAddr();
		symParser();
		relocate();
		Elf_file* elf = new Elf_file();
		elf->assemObj(this);
		elf->writeElf(this, dir);
		return true;
	}
}

void Linker::relocate(){
	if(showLink){
		printf("-------relocate-------\n");
	}
	else{}
	for(int i = 0; i < elfs.size(); i++){
		vector<RelItem*> tab = elfs[i]->relTab;
		for(int j = 0; j < tab.size(); j++){
			//relocate symbol
			string relName = tab[j]->relName;
			Elf32_Sym* sym = elfs[i]->symTab[relName];

			//relocate section
			string segName = tab[j]->segName;
			Elf32_Shdr* seg = elfs[i]->shdrTab[segName];

			//relocate symbol addr
			unsigned int symAddr = sym->st_value;

			//relocate location
			unsigned int offset = tab[j]->rel->r_offset;
			unsigned int relAddr = seg->sh_addr + offset;

			//relocate type
			unsigned char type = ELF32_R_TYPE(tab[j]->rel->r_info);

			if(showLink){
				printf("%s\trelAddr=%08x\tsymAddr=%08x\n", tab[j]->relName.c_str(), relAddr, symAddr);
			}
			segLists[segName]->relocAddr(relAddr, type, symAddr);
		}
	}
}

vector<string> Linker::getSegNames(){
	return segNames;
}

vector<SymLink*> Linker::getSymDef(){
	return symDef;
}
unordered_map<string, SegList*, string_hash> Linker::getSegLists(){
	return segLists;
}
