#include "generator.h"
#include "elf.h"
#include "elf_file.h"
#include "table.h"

bool Generator::precessRel(int type){
	if(scanLop == 1 || relLb == NULL){
		relLb = NULL;
		return false;
	}
	else{
		bool flag = false;
		if(type == R_386_32){
			obj.addRel(curSeg, lb_record::curAddr, relLb->lbName, type);
			flag = true;
		}
		else if(type == R_386_PC32){
			if(relLb->externed){
				obj.addRel(curSeg, lb_record::curAddr, relLb->lbName, type);
				flag = true;
			}
		}
		else{}
		relLb = NULL;
		return flag;
	}
}
