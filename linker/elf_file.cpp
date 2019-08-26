#include "elf_file.h"
#include "elf.h"
#include "linker.h"

RelItem::RelItem(string sname, Elf32_Rel* r, string rname){
	segName = sname;
	rel = r;
	relName = rname;
}

RelItem::~RelItem(){
	if(rel){
		delete rel;
	}
	else{}
}

void Elf_file::readElf(const string dir){
	//open object file
	elf_dir = dir;
	FILE* fp = fopen(elf_dir.c_str(), "rb");
	if(fp != NULL){
		//elf header
		rewind(fp);
		fread(&ehdr, sizeof(Elf32_Ehdr), 1, fp);

		//program header table
		if(ehdr.e_type == ET_EXEC){
			fseek(fp, ehdr.e_phoff, 0);
			for(int i = 0; i < ehdr.e_phnum; i++){
				Elf32_Phdr* phdr = new Elf32_Phdr();
				fread(phdr, ehdr.e_phentsize, 1, fp);
				phdrTab.push_back(phdr);
			}
		}
		else{}//our obj file don't have this structure

		//.shstrtab entry
		Elf32_Shdr shstrTab;
		fseek(fp, ehdr.e_shoff + ehdr.e_shentsize * ehdr.e_shstrndx, 0);
		fread(&shstrTab, ehdr.e_shentsize, 1, fp);

		//.shstrtab
		char* shstrTabData = new char[shstrTab.sh_size];
		fseek(fp, shstrTab.sh_offset, 0);
		fread(shstrTabData, shstrTab.sh_size, 1, fp);

		//section table
		fseek(fp, ehdr.e_shoff, 0);
		for(int i = 0; i < ehdr.e_shnum; i++){
			Elf32_Shdr* shdr = new Elf32_Shdr();
			fread(shdr, ehdr.e_shentsize, 1, fp);
			string name(shstrTabData + shdr->sh_name);
			shdrNames.push_back(name);
			shdrTab[name] = shdr;
		}

		//.strtab
		Elf32_Shdr* strTab = shdrTab[".strtab"];
		char* strTabData = new char[strTab->sh_size];
		fseek(fp, strTab->sh_offset, 0);
		fread(strTabData, strTab->sh_size, 1, fp);

		//.symtab
		Elf32_Shdr* sh_symTab = shdrTab[".symtab"];
		fseek(fp, sh_symTab->sh_offset, 0);
		int symNum = sh_symTab->sh_size / sh_symTab->sh_entsize;
		for(int i = 0; i < symNum; i++){
			Elf32_Sym* sym = new Elf32_Sym();
			fread(sym, sh_symTab->sh_entsize, 1, fp);
			string name(strTabData + sym->st_name);
			symNames.push_back(name);
			symTab[name] = sym;
		}

		if(showLink){
			printf("-------%s relocation data:-------\n", elf_dir.c_str());
		}
		else{}
		//.rel.data .rel.text
		for(int i = 0; i < shdrNames.size(); i++){
			string shdrName = shdrNames[i];
			Elf32_Shdr* shdr = shdrTab[shdrName];
			if(shdr->sh_type == SHT_REL){
				fseek(fp, shdr->sh_offset, 0);
				int relNum = shdr->sh_size / shdr->sh_entsize;
				for(int j = 0; j < relNum; j++){
					Elf32_Rel* rel = new Elf32_Rel();
					fread(rel, shdr->sh_entsize, 1, fp);
					string segName = shdrNames[shdr->sh_info];
					string symName = symNames[ELF32_R_SYM(rel->r_info)];
					relTab.push_back(new RelItem(segName, rel, symName));
					if(showLink){
						printf("%s\t%08x\t%s\n", segName.c_str(), rel->r_offset, symName.c_str());
					}
					else{}
				}
			}
			else{}//only care relocation section
		}

		delete []shstrTabData;
		delete []strTabData;
		fclose(fp);
	}
	else{
		cout << "file open error!" << endl;
	}
}

void Elf_file::getData(char* buf, Elf32_Off offset, Elf32_Word size){
	FILE* fp = fopen(elf_dir.c_str(), "rb");
	rewind(fp);
	fseek(fp, offset, 0);
	fread(buf, size, 1, fp);
	fclose(fp);
}

void Elf_file::assemObj(Linker* linker){
	//all segments
	vector<string> AllSegNames;
	AllSegNames.push_back("");
	vector<string> segNames = linker->getSegNames();
	for(int i = 0; i < segNames.size(); i++){
		AllSegNames.push_back(segNames[i]);
	}
	AllSegNames.push_back(".shstrtab");
	AllSegNames.push_back(".symtab");
	AllSegNames.push_back(".strtab");

	//segment index
	unordered_map<string, int, string_hash> shIndex;
	//segment name index
	unordered_map<string, int, string_hash> shstrIndex;
	//form index
	for(int i = 0; i < AllSegNames.size(); i++){
		string name = AllSegNames[i];
		shIndex[name] = i;
		shstrIndex[name] = shstrtab.size();
		shstrtab += name;
		shstrtab.push_back('\0');
	}

	//generate symbol table(symDef only contains global symbol)
	addSym("", NULL);
	vector<SymLink*>symDef = linker->getSymDef();
	for(int i = 0; i < symDef.size(); i++){
		string name = symDef[i]->name;
		Elf_file* prov = symDef[i]->prov;
		Elf32_Sym* sym = prov->symTab[name];
		string segName = prov->shdrNames[sym->st_shndx];
		sym->st_shndx = shIndex[segName];
		addSym(name, sym);
	}

	//symbol index
	unordered_map<string, int, string_hash> symIndex;
	//symbol name index
	unordered_map<string, int, string_hash> strIndex;
	//form index
	for(int i = 0; i < symNames.size(); i++){
		string name = symNames[i];
		symIndex[name] = i;
		strIndex[name] = strtab.size();
		strtab += name;
		strtab.push_back('\0');
	}

	//update symbol table symbol name index
	for(int i = 0; i < symNames.size(); i++){
		string name = symNames[i];
		symTab[name]->st_name = strIndex[name];
	}

	//handle elf header
	char magic[] = {
		0x7f, 0x45, 0x4c, 0x46,
		0x01, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	memcpy(&ehdr.e_ident, magic, sizeof(magic));
	ehdr.e_type = ET_EXEC;
	ehdr.e_machine = EM_386;
	ehdr.e_version = EV_CURRENT;
	ehdr.e_entry = symTab[START]->st_value;
	ehdr.e_phoff = 0;
	ehdr.e_shoff = 0;
	ehdr.e_flags = 0;
	ehdr.e_ehsize = sizeof(Elf32_Ehdr);
	ehdr.e_phentsize = sizeof(Elf32_Phdr);
	ehdr.e_phnum = segNames.size();
	ehdr.e_shentsize = sizeof(Elf32_Shdr);
	ehdr.e_shnum = AllSegNames.size();
	ehdr.e_shstrndx = shIndex[".shstrtab"];

	int curOff = sizeof(ehdr);

	ehdr.e_phoff = curOff;

	//generate program header table(already align)
	unordered_map<string, SegList*, string_hash> segLists = linker->getSegLists();
	for(int i = 0; i < segNames.size(); i++){
		string name = segNames[i];
		Elf32_Word flags = (name == ".text") ? PF_X | PF_R : PF_W | PF_R;
		addPhdr(PT_LOAD, segLists[name]->offset, segLists[name]->baseAddr,
				segLists[name]->size, segLists[name]->size, flags, MEM_ALIGN);
	}
	curOff += ehdr.e_phentsize * ehdr.e_phnum;

	//generate exsiting section table
	addShdr("", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	for(int i = 0; i < segNames.size(); i++){
		string name = segNames[i];
		Elf32_Word sh_flags, sh_align;
		if(name == ".text"){
			sh_flags = SHF_ALLOC | SHF_EXECINSTR;
			sh_align = TEXT_ALIGN;
		}
		else{
			sh_flags = SHF_ALLOC | SHF_WRITE;
			sh_align = DISC_ALIGN;
		}
		addShdr(name, SHT_PROGBITS, sh_flags, segLists[name]->baseAddr,
				segLists[name]->offset, segLists[name]->size, 0, 0, sh_align, 0);
		curOff = segLists[name]->offset + segLists[name]->size;
	}
	curOff += (4 - curOff % 4) % 4;

	//add new section entry
	addShdr(".shstrtab", SHT_STRTAB, 0, 0, curOff, shstrtab.size(), SHN_UNDEF, 0, 1, 0);
	curOff += shstrtab.size();
	curOff += (4 - curOff % 4) % 4;

	ehdr.e_shoff = curOff;//section table offset
	curOff += ehdr.e_shnum * ehdr.e_shentsize;

	addShdr(".symtab", SHT_SYMTAB, 0, 0, curOff, symNames.size() * sizeof(Elf32_Sym),
			shIndex[".strtab"], 0, 1, sizeof(Elf32_Sym));
	curOff += symNames.size() * sizeof(Elf32_Sym);

	addShdr(".strtab", SHT_STRTAB, 0, 0, curOff, strtab.size(), SHN_UNDEF, 0, 1, 0);
	curOff += strtab.size();
	curOff += (4 - curOff % 4) % 4;

	//update section table section name index
	for(int i = 0; i < AllSegNames.size(); i++){
		string name = AllSegNames[i];
		shdrTab[name]->sh_name = shstrIndex[name];
	}

}

void Elf_file::addSym(string st_name, Elf32_Sym* s){
	Elf32_Sym* sym = new Elf32_Sym();
	symTab[st_name] = sym;
	if(st_name == ""){
		sym->st_name = 0;
		sym->st_value = 0;
		sym->st_size = 0;
		sym->st_info = 0;
		sym->st_other = 0;
		sym->st_shndx = 0;
	}
	else{
		sym->st_name = 0;
		sym->st_value = s->st_value;
		sym->st_size = s->st_size;
		sym->st_info = s->st_info;
		sym->st_other = s->st_other;
		sym->st_shndx = s->st_shndx;
	}
	symNames.push_back(st_name);
}

void Elf_file::addPhdr(Elf32_Word type, Elf32_Off off, Elf32_Addr vaddr, Elf32_Word filesz,
		Elf32_Word memsz, Elf32_Word flags, Elf32_Word align){
	Elf32_Phdr* ph = new Elf32_Phdr();
	ph->p_type = type;
	ph->p_offset = off;
	ph->p_vaddr = vaddr;
	ph->p_paddr = vaddr;
	ph->p_filesz = filesz;
	ph->p_memsz = memsz;
	ph->p_flags = flags;
	ph->p_align = align;
	phdrTab.push_back(ph);
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

void Elf_file::writeElf(Linker* linker, const char* dir){
	FILE* fout = fopen(dir, "w");
	int padNum = 0;
	char pad[1] = {0};

	if(fout != 0){
		//elf header
		fwrite(&ehdr, ehdr.e_ehsize, 1, fout);

		//program header table
		for(int i = 0; i < phdrTab.size(); i++){
			fwrite(phdrTab[i], ehdr.e_phentsize, 1, fout);
		}

		//.text .data
		vector<string> segNames = linker->getSegNames();
		for(int i = 0; i < segNames.size(); i++){
			unordered_map<string, SegList*, string_hash> segLists = linker->getSegLists();
			SegList* segs = segLists[segNames[i]];
			padNum = segs->offset - segs->begin;
			fwrite(pad, sizeof(pad), padNum, fout);

			Block* last = NULL;
			int lastEnd = 0;
			for(int j = 0; j < segs->blocks.size(); j++){
				Block* block = segs->blocks[j];
				if(last != NULL){
					lastEnd = last->offset + last->size;
					padNum = block->offset - lastEnd;
					fwrite(pad, sizeof(pad), padNum, fout);
				}
				else{}
				fwrite(block->data, block->size, 1, fout);
				last = block;
			}
		}

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
		fclose(fout);
	}
	else{
		printf("%s open fail\n", dir);
	}
}
