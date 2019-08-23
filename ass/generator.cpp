#include "generator.h"
#include "elf.h"
#include "elf_file.h"
#include "table.h"

Generator generator;

static int i_2opcode[][2][4]{
//8-bit op                | 32-bit op
//r,r   r,rm  rm,r  r,imm   r,r   r,rm  rm,r  r,imm
{{0x8a, 0x8a, 0x88, 0xb0}, {0x8b, 0x8b, 0x89, 0xb8}},//mov
{{0x3a, 0x3a, 0x38, 0x80}, {0x3b, 0x3b, 0x39, 0x81}},//cmp
{{0x2a, 0x2a, 0x28, 0x80}, {0x2b, 0x2b, 0x29, 0x81}},//sub
{{0x02, 0x02, 0x00, 0x80}, {0x03, 0x03, 0x01, 0x81}},//add
{{0x22, 0x22, 0x20, 0x80}, {0x23, 0x23, 0x21, 0x81}},//and
{{0x0a, 0x0a, 0x08, 0x80}, {0x0b, 0x0b, 0x09, 0x81}},//or
{{0x00, 0x00, 0x00, 0x00}, {0x8d, 0x8d, 0x00, 0x00}},//lea
};

static int i_1opcode[] = {
	0xe8, 0xcd, 0xf7, 0xf7, 0xf7, 0x40, 0x48, 0xe9, //call, int, imul, idiv, neg, inc, dec, jmp
	0x84, 0x85, //je, jne
	0x94, 0x95, 0x9f, 0x9d, 0x9c, 0x9e, //sete, setne, setg, setge, setl, setle
	0x50, 0x58 //push, pop
};

static int i_0opcode[] = {
	0xc3 //ret
};

bool Generator::processRel(int type){
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

int Generator::getOpCode(Tag tag, op_type des_t, op_type src_t, int len){
	int index = 0;
	switch (src_t){
		case IMMEDIATE: index = 3; break;
		case REGISTER: index = 2 * (des_t != REGISTER); break;
		case MEMORY: index = 1; break;
	}
	return i_2opcode[tag - I_MOV][len != 8][index];
}

void Generator::gen2op(Tag tag, op_type des_t, op_type src_t, int len){
	int opcode = getOpCode(tag, des_t, src_t, len);
	switch(modrm.mod){
		case -1: //reg32 <- imm32
			if(tag == I_MOV){
				opcode += modrm.reg;
			}
			else{
				int reg_codes[] = {7, 5, 0, 4, 1};
				modrm.mod = 3;
				modrm.rm = modrm.reg;
				modrm.reg = reg_codes[tag - I_CMP];
			}
			writeBytes(opcode, 1);
			if(tag != I_MOV){
				writeModRM();
			}
			else{}
			processRel(R_386_32); //may be "mov eax, @buffer", not very sure is for this case;
			writeBytes(instr.imm32, len);
			break;
		case 0: //[reg32] <-> reg
			writeBytes(opcode, 1);
			writeModRM();
			if(modrm.rm == 5){
				processRel(R_386_32);
				instr.writeDisp();
			}
			else if(modrm.rm == 4){
				writeSIB();
			}
			else{}
			break;
		case 1: //[reg32 + disp8] <-> reg
			writeBytes(opcode, 1);
			writeModRM();
			if(modrm.rm == 4){
				writeSIB();
			}
			else{}
			instr.writeDisp();
			break;
		case 2: //[reg32 + disp32] <-> reg
			writeBytes(opcode, 1);
			writeModRM();
			if(modrm.rm == 4){
				writeSIB();
			}
			else{}
			instr.writeDisp();
			break;
		case 3: //reg <-> reg
			writeBytes(opcode, 1);
			writeModRM();
			break;
	}
}

void Generator::writeBytes(int value, int len){
	lb_record::curAddr += len;
	if(scanLop == 2){
		fwrite(&value, len, 1, ftmp);
	}
	else{}
}

void Generator::writeModRM(){
	if(modrm.mod != -1){
		int byte = (modrm.mod << 6) + (modrm.reg << 3) + modrm.rm;
		writeBytes(byte, 1);
	}
	else{}
}

void Generator::writeSIB(){
	if(sib.scale != -1){
		int byte = (sib.scale << 6) + (sib.index << 3) + sib.base;
		writeBytes(byte, 1);
	}
	else{}
}

void Generator::gen1op(Tag tag, int opr_t, int len){
	int opcode = i_1opcode[tag - I_CALL];
	if(tag == I_CALL || tag >= I_JMP && tag <= I_JNE){
		if(tag != I_CALL || tag != I_JMP){
			writeBytes(0x0f, 1);
		}
		else{}
		writeBytes(opcode, 1);
		int addr = processRel(R_386_PC32) ? lb_record::curAddr : instr.imm32;
		int pc = lb_record::curAddr + 4;
		writeBytes(addr - pc, 4);
	}
	else if(tag >= I_SETE && tag <= I_SETLE){
		modrm.mod = 3;
		modrm.rm = modrm.reg;
		modrm.reg = 0;

		writeBytes(0x0f, 1);
		writeBytes(opcode, 1);
		writeModRM();
	}
	else if(tag == I_INT){
		writeBytes(opcode, 1);
		writeBytes(instr.imm32, 1);
	}
	else if(tag == I_PUSH){
		if(opr_t == IMMEDIATE){
			opcode = 0x68;
		}
		else{
			opcode += modrm.reg;
		}

		writeBytes(opcode, 1);
		if(opr_t == IMMEDIATE){
			writeBytes(instr.imm32, 4);
		}
		else{}
	}
	else if(tag == I_INC || tag == I_DEC){
		if(len == 1){
			opcode = 0xfe;
			int reg_codes[] = {0, 1};
			modrm.mod = 3;
			modrm.rm = modrm.reg;
			modrm.reg = reg_codes[tag - I_INC];
		}
		else{
			opcode += modrm.reg;
		}

		writeBytes(opcode, 1);
		if(len == 1){
			writeModRM();
		}
		else{}
	}
	else if(tag == I_NEG){
		if(len == 1){
			opcode = 0xf6;
		}
		else{}
		modrm.mod = 3;
		modrm.rm = modrm.reg;
		modrm.reg = 3;

		writeBytes(opcode, 1);
		writeModRM();
	}
	else if(tag == I_POP){
		opcode += modrm.reg;
		writeBytes(opcode, 1);
	}
	else if(tag == I_IMUL || tag == I_IDIV){
		int reg_codes[] = {5, 7};
		modrm.mod = 3;
		modrm.rm = modrm.reg;
		modrm.reg = reg_codes[tag - I_IMUL];

		writeBytes(opcode, 1);
		writeModRM();
	}
	else{}
}

void Generator::gen0op(Tag tag){
	int opcode = i_0opcode[tag - I_RET];
	writeBytes(opcode, 1);
}

