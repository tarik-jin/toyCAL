#include "elf_file.h"
#include "elf.h"

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
