#pragma once
#include "common.h"

class Generator{
public:
	bool processRel(int type);
	int getOpCode(Tag tag, op_type des_t, op_type src_t, int len);

	void writeBytes(int value, int len);
	void writeModRM();
	void writeSIB();

	void gen2op(Tag tag, op_type des_t, op_type src_t, int len);
	void gen1op(Tag tag, int opr_t, int len);
	void gen0op(Tag tag);
};
