#pragma once


#include <register.h>
#include <utils.h>


#define ALU_SUM 0
#define ALU_SUB 1
#define ALU_MUL 2
#define ALU_DIV 3
#define ALU_IMUL 4
#define ALU_IDIV 5


struct Core {
	Register regs[REGISTERS_COUNT];
	uint64 flag;
	uint8 id;

	Core();
	void init(uint8);

	void clear();
	void step();

	void set_flag(uint8, uint8);
	uint8 get_flag(uint8);

	template<class T> T ALU(T,T,uint8);

	uint8   pop1 (uint64);
	uint16  pop2 (uint64);
	uint32  pop4 (uint64);
	uint64  pop8 (uint64);
	uint128 pop16(uint64);

	template<class T> void push(T, uint64);

	void illegal();

	void print_info();
};
