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
