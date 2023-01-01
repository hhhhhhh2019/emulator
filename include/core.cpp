#pragma once

#include <registers.cpp>


#ifndef DEBUG
	#define LOG
#else
	#define LOG(...) printf(__VA_ARGS__)
#endif


extern unsigned char *ram;


enum ALU {
	SUM,
	SUB,
	MUL,
	DIV,
	OR,
	AND,
	XOR,
	LSHFT,
	RSHFT
};


const char* registers[80] = {
	"r0","l0","i0","s0","b0",
	"r1","l1","i1","s1","b1",
	"r2","l2","i2","s2","b2",
	"r3","l3","i3","s3","b3",
	"r4","l4","i4","s4","b4",
	"r5","l5","i5","s5","b5",
	"r6","l6","i6","s6","b6",
	"r7","l7","i7","s7","b7",
	"r8","l8","i8","s8","b8",
	"r9","l9","i9","s9","b9",
	"r10","l10","i10","s10","b10",
	"r11","l11","i11","s11","b11",
	"r12","l12","i12","s12","b12",
	"r13","l13","i13","s13","b13",
	"r14","l14","i14","s14","b14",
	"r15","l15","i15","s15","b15",
};


class Core {
public:
	Register regs[REG::COUNT];
	uint64 flag;
	uint128 counter;

	Core() {}

	void clear_registers() {
		for (int i = 0; i < REG::COUNT; i++) {
			regs[i].ull = 0;
		}
	}

	void print_registers() {
		for (int i = 0; i < REG::COUNT; i++) {
			LOG("\t r%02i: %016lx%016lx %016lx %08x %04x %02x\n",
				i, (uint64)(regs[i].ull >> 64), regs[i].ul, regs[i].ul, regs[i].ui, regs[i].us, regs[i].ub);
		}
	}

	void set_flag_state(unsigned char id, char v) {
		if (v == 0) {
			flag &= ~(1 << id);
		} else {
			flag |= 1 << id;
		}
	}

	char get_flag_state(unsigned char id) {
		return (flag >> id) & 1;
	}

	void illegal() {
		LOG("\033[0;31m");
		LOG("+--------------------+\n");
		LOG("|Illegal instruction!|\n");
		LOG("+--------------------+\n");
		LOG("\033[0;37m");
	}

	template <class T>
	T alu(T a, T b, char op) {
		set_flag_state(FLAG::equals, 0);
		set_flag_state(FLAG::more, 0);
		set_flag_state(FLAG::overflow, 0);

		T sum, sub, mul, div1, div2;

		sum = a + b;
		sub = a - b;
		mul = a * b;
		div1 = a / b;
		div2 = a % b;

		if (a < b)
			set_flag_state(FLAG::overflow, 1);

		if (a > b)
			set_flag_state(FLAG::more, 1);

		if (a == b)
			set_flag_state(FLAG::equals, 1);

		if (op == ALU::SUM)
			return sum;
		if (op == ALU::SUB)
			return sub;
		if (op == ALU::MUL)
			return mul;
		if (op == ALU::DIV)
			return (div1 << sizeof(T) / 2) | div2;

		return 0;
	}

	void push(uint128 v) {
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   0) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   8) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  16) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  24) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  32) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  40) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  48) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  56) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  64) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  72) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  80) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  88) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  96) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >> 104) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >> 112) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >> 120) & 0xff;
	}

	void push(uint64 v) {
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   0) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   8) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  16) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  24) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  32) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  40) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  48) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  56) & 0xff;
	}

	void push(uint32 v) {
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   0) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   8) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  16) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>  24) & 0xff;
	}

	void push(uint16 v) {
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   0) & 0xff;
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   8) & 0xff;
	}

	void push(uint8 v) {
		ram[regs[sp].ul-- + regs[lo].ul] = (v >>   0) & 0xff;
	}


	uint128 pop128() {
		regs[sp].ul += 16;
		return
			((uint128)ram[regs[sp].ul - 15 + regs[lo].ul] << 120) |
			((uint128)ram[regs[sp].ul - 14 + regs[lo].ul] << 112) |
			((uint128)ram[regs[sp].ul - 13 + regs[lo].ul] << 104) |
			((uint128)ram[regs[sp].ul - 12 + regs[lo].ul] <<  96) |
			((uint128)ram[regs[sp].ul - 11 + regs[lo].ul] <<  88) |
			((uint128)ram[regs[sp].ul - 10 + regs[lo].ul] <<  80) |
			((uint128)ram[regs[sp].ul -  9 + regs[lo].ul] <<  72) |
			((uint128)ram[regs[sp].ul -  8 + regs[lo].ul] <<  64) |
			((uint128)ram[regs[sp].ul -  7 + regs[lo].ul] <<  56) |
			((uint128)ram[regs[sp].ul -  6 + regs[lo].ul] <<  48) |
			((uint128)ram[regs[sp].ul -  5 + regs[lo].ul] <<  40) |
			((uint128)ram[regs[sp].ul -  4 + regs[lo].ul] <<  32) |
			((uint128)ram[regs[sp].ul -  3 + regs[lo].ul] <<  24) |
			((uint128)ram[regs[sp].ul -  2 + regs[lo].ul] <<  16) |
			((uint128)ram[regs[sp].ul -  1 + regs[lo].ul] <<   8) |
			((uint128)ram[regs[sp].ul -  0 + regs[lo].ul] <<   0);
	}

	uint64 pop64() {
		regs[sp].ul += 8;
		return
			((uint64)ram[regs[sp].ul -  7 + regs[lo].ul] <<  56) |
			((uint64)ram[regs[sp].ul -  6 + regs[lo].ul] <<  48) |
			((uint64)ram[regs[sp].ul -  5 + regs[lo].ul] <<  40) |
			((uint64)ram[regs[sp].ul -  4 + regs[lo].ul] <<  32) |
			((uint64)ram[regs[sp].ul -  3 + regs[lo].ul] <<  24) |
			((uint64)ram[regs[sp].ul -  2 + regs[lo].ul] <<  16) |
			((uint64)ram[regs[sp].ul -  1 + regs[lo].ul] <<   8) |
			((uint64)ram[regs[sp].ul -  0 + regs[lo].ul] <<   0);
	}

	uint32 pop32() {
		regs[sp].ul += 4;
		return
			((uint32)ram[regs[sp].ul -  3 + regs[lo].ul] <<  24) |
			((uint32)ram[regs[sp].ul -  2 + regs[lo].ul] <<  16) |
			((uint32)ram[regs[sp].ul -  1 + regs[lo].ul] <<   8) |
			((uint32)ram[regs[sp].ul -  0 + regs[lo].ul] <<   0);
	}

	uint16 pop16() {
		regs[sp].ul += 2;
		return
			((uint16)ram[regs[sp].ul -  1 + regs[lo].ul] <<   8) |
			((uint16)ram[regs[sp].ul -  0 + regs[lo].ul] <<   0);
	}

	uint8 pop8() {
		regs[sp].ul += 1;
		return
			((uint8)ram[regs[sp].ul -  0 + regs[lo].ul] <<   0);
	}


	uint128 popRAM(uint64 addr) {
		addr += 15;
		return
			((uint128)ram[addr -  0 + regs[lo].ul] << 120) |
			((uint128)ram[addr -  1 + regs[lo].ul] << 112) |
			((uint128)ram[addr -  2 + regs[lo].ul] << 104) |
			((uint128)ram[addr -  3 + regs[lo].ul] <<  96) |
			((uint128)ram[addr -  4 + regs[lo].ul] <<  88) |
			((uint128)ram[addr -  5 + regs[lo].ul] <<  80) |
			((uint128)ram[addr -  6 + regs[lo].ul] <<  72) |
			((uint128)ram[addr -  7 + regs[lo].ul] <<  64) |
			((uint128)ram[addr -  8 + regs[lo].ul] <<  56) |
			((uint128)ram[addr -  9 + regs[lo].ul] <<  48) |
			((uint128)ram[addr - 10 + regs[lo].ul] <<  40) |
			((uint128)ram[addr - 11 + regs[lo].ul] <<  32) |
			((uint128)ram[addr - 12 + regs[lo].ul] <<  24) |
			((uint128)ram[addr - 13 + regs[lo].ul] <<  16) |
			((uint128)ram[addr - 14 + regs[lo].ul] <<   8) |
			((uint128)ram[addr - 15 + regs[lo].ul] <<   0);
	}


	void step() {
		if (get_flag_state(FLAG::running) == 0)
			return;

		counter += 1;

		uint8 i1 = ram[regs[pc].ul+0 + regs[lo].ul];
		uint8 i2 = ram[regs[pc].ul+1 + regs[lo].ul];

		uint8 reg =
			ram[regs[pc].ul+2 + regs[lo].ul];

		Register param;

		param.ull = popRAM(regs[pc].ul+3);


		LOG("%02x %02x %02x %016lx %016lx %016lx\n",
				i1,i2, reg, param.ul, (uint64)(param.ull >> 64), flag);

		print_registers();


		regs[pc].ul += 2; // instr size


		if (i1 == 0x00) { // mov
			LOG("mov ");
			print_register_by_id(reg);
			LOG(" ");

			regs[pc].ul += 1; // reg size

			if (i2 == 0x00) { // reg to reg
				print_register_by_id(param.ub);
				LOG("\n");

				regs[pc].ul += 1; // reg size

				if (reg % 5 != param.ub % 5) {
					illegal();
					return;
				}

				if (reg % 5 == 0) {
					regs[reg/5].ull = regs[param.ub/5].ull;
				}

				if (reg % 5 == 1) {
					regs[reg/5].ul = regs[param.ub/5].ul;
				}

				if (reg % 5 == 2) {
					regs[reg/5].ui = regs[param.ub/5].ui;
				}

				if (reg % 5 == 3) {
					regs[reg/5].us = regs[param.ub/5].us;
				}

				if (reg % 5 == 4) {
					regs[reg/5].ub = regs[param.ub/5].ub;
				}
			}

			if (i2 == 0x01) { // num to reg
				if (reg % 5 == 0) {
					LOG("%016lx%016lx\n", (uint64)(param.ull >> 64), param.ul);
					regs[pc].ul += 16; // long long int size
					regs[reg/5].ull = param.ull;
				}

				if (reg % 5 == 1) {
					LOG("%016lx\n", param.ul);
					regs[pc].ul += 8; // long int size
					regs[reg/5].ul = param.ul;
				}

				if (reg % 5 == 2) {
					LOG("%08x\n", param.ui);
					regs[pc].ul += 4; // int size
					regs[reg/5].ui = param.ui;
				}

				if (reg % 5 == 3) {
					LOG("%04x\n", param.us);
					regs[pc].ul += 2; // short size
					regs[reg/5].us = param.us;
				}

				if (reg % 5 == 4) {
					LOG("%02x\n", param.ub);
					regs[pc].ul += 1; // byte size
					regs[reg/5].ub = param.ub;
				}
			}

			if (i2 == 0x02) { // from mem
				LOG("[%08lx]\n", param.ul);

				regs[pc].ul += 8; // addr size

				if (reg % 5 == 0) {
					regs[reg/5].ull = popRAM(param.ul);
				}

				if (reg % 5 == 1) {
					regs[reg/5].ul = popRAM(param.ul);
				}

				if (reg % 5 == 2) {
					regs[reg/5].ui = popRAM(param.ul);
				}

				if (reg % 5 == 3) {
					regs[reg/5].us = popRAM(param.ul);
				}

				if (reg % 5 == 4) {
					regs[reg/5].ub = popRAM(param.ul);
				}
			}
		}


		if (i1 == 0x01) { // alu
			regs[pc].ul += 1; // reg size
			regs[pc].ul += 2; // reg size * 2

			uint8 p2 = param.us >> 8;

			if (i2 == 0x00) { // sum
				LOG("sum ");
				print_register_by_id(reg);
				LOG(" ");
				print_register_by_id(param.ub);
				LOG(" ");
				print_register_by_id(p2);
				LOG("\n");

				if (reg % 5 != param.ub % 5 || reg % 5 != p2 % 5 || param.ub % 5 != p2 % 5) {
					illegal();
					return;
				}

				if (reg % 5 == 0)
					regs[reg/5].ull = alu(regs[param.ub/5].ull,regs[p2/5].ull, ALU::SUM);

				if (reg % 5 == 1)
					regs[reg/5].ul = alu(regs[param.ub/5].ul,regs[p2/5].ul, ALU::SUM);

				if (reg % 5 == 2)
					regs[reg/5].ui = alu(regs[param.ub/5].ui,regs[p2/5].ui, ALU::SUM);

				if (reg % 5 == 3)
					regs[reg/5].us = alu(regs[param.ub/5].us,regs[p2/5].us, ALU::SUM);

				if (reg % 5 == 4)
					regs[reg/5].ub = alu(regs[param.ub/5].ub,regs[p2/5].ub, ALU::SUM);
			}

			if (i2 == 0x01) { // sub
				LOG("sub ");
				print_register_by_id(reg);
				LOG(" ");
				print_register_by_id(param.ub);
				LOG(" ");
				print_register_by_id(p2);
				LOG("\n");

				if (reg % 5 != param.ub % 5 || reg % 5 != p2 % 5 || param.ub % 5 != p2 % 5) {
					illegal();
					return;
				}

				if (reg % 5 == 0)
					regs[reg/5].ull = alu(regs[param.ub/5].ull,regs[p2/5].ull, ALU::SUB);

				if (reg % 5 == 1)
					regs[reg/5].ul = alu(regs[param.ub/5].ul,regs[p2/5].ul, ALU::SUB);

				if (reg % 5 == 2)
					regs[reg/5].ui = alu(regs[param.ub/5].ui,regs[p2/5].ui, ALU::SUB);

				if (reg % 5 == 3)
					regs[reg/5].us = alu(regs[param.ub/5].us,regs[p2/5].us, ALU::SUB);

				if (reg % 5 == 4)
					regs[reg/5].ub = alu(regs[param.ub/5].ub,regs[p2/5].ub, ALU::SUB);
			}

			if (i2 == 0x02) { // cmp
				LOG("cmp ");
				print_register_by_id(reg);
				LOG(" ");
				print_register_by_id(param.ub);
				LOG(" ");
				print_register_by_id(p2);
				LOG("\n");

				if (param.ub % 5 != p2 % 5) {
					illegal();
					return;
				}

				if (reg % 5 == 0)
					alu(regs[param.ub/5].ull,regs[p2/5].ull, ALU::SUB);

				if (reg % 5 == 1)
					alu(regs[param.ub/5].ul,regs[p2/5].ul, ALU::SUB);

				if (reg % 5 == 2)
					alu(regs[param.ub/5].ui,regs[p2/5].ui, ALU::SUB);

				if (reg % 5 == 3)
					alu(regs[param.ub/5].us,regs[p2/5].us, ALU::SUB);

				if (reg % 5 == 4)
					alu(regs[param.ub/5].ub,regs[p2/5].ub, ALU::SUB);
			}

			if (i2 == 0x03) { // mul
				LOG("mul ");
				print_register_by_id(reg);
				LOG(" ");
				print_register_by_id(param.ub);
				LOG(" ");
				print_register_by_id(p2);
				LOG("\n");

				if (reg % 5 != param.ub % 5 || reg % 5 != p2 % 5 || param.ub % 5 != p2 % 5) {
					illegal();
					return;
				}

				if (reg % 5 == 0)
					regs[reg/5].ull = alu(regs[param.ub/5].ull,regs[p2/5].ull, ALU::MUL);

				if (reg % 5 == 1)
					regs[reg/5].ul = alu(regs[param.ub/5].ul,regs[p2/5].ul, ALU::MUL);

				if (reg % 5 == 2)
					regs[reg/5].ui = alu(regs[param.ub/5].ui,regs[p2/5].ui, ALU::MUL);

				if (reg % 5 == 3)
					regs[reg/5].us = alu(regs[param.ub/5].us,regs[p2/5].us, ALU::MUL);

				if (reg % 5 == 4)
					regs[reg/5].ub = alu(regs[param.ub/5].ub,regs[p2/5].ub, ALU::MUL);
			}

			if (i2 == 0x04) { // div
				LOG("div ");
				print_register_by_id(reg);
				LOG(" ");
				print_register_by_id(param.ub);
				LOG(" ");
				print_register_by_id(p2);
				LOG("\n");

				if (reg % 5 != param.ub % 5 || reg % 5 != p2 % 5 || param.ub % 5 != p2 % 5) {
					illegal();
					return;
				}

				if (reg % 5 == 0)
					regs[reg/5].ull = alu(regs[param.ub/5].ull,regs[p2/5].ull, ALU::SUB);

				if (reg % 5 == 1)
					regs[reg/5].ul = alu(regs[param.ub/5].ul,regs[p2/5].ul, ALU::SUB);

				if (reg % 5 == 2)
					regs[reg/5].ui = alu(regs[param.ub/5].ui,regs[p2/5].ui, ALU::SUB);

				if (reg % 5 == 3)
					regs[reg/5].us = alu(regs[param.ub/5].us,regs[p2/5].us, ALU::SUB);

				if (reg % 5 == 4)
					regs[reg/5].ub = alu(regs[param.ub/5].ub,regs[p2/5].ub, ALU::SUB);
			}
		}


		if (i1 == 0x02) { // stack
			regs[pc].ul += 1; // reg size

			if (i2 == 0x00) { // push reg
				LOG("push ");
				print_register_by_id(reg);
				LOG("\n");

				if (reg % 5 == 0)
					push(regs[reg/5].ull);

				if (reg % 5 == 1)
					push(regs[reg/5].ul);

				if (reg % 5 == 2)
					push(regs[reg/5].ui);

				if (reg % 5 == 3)
					push(regs[reg/5].us);

				if (reg % 5 == 4)
					push(regs[reg/5].ub);
			}

			if (i2 == 0x01) { // pop reg
				LOG("pop ");
				print_register_by_id(reg);
				LOG("\n");

				if (reg % 5 == 0)
					regs[reg/5].ull = pop128();

				if (reg % 5 == 1)
					regs[reg/5].ul = pop64();

				if (reg % 5 == 2)
					regs[reg/5].ui = pop32();

				if (reg % 5 == 3)
					regs[reg/5].us = pop16();

				if (reg % 5 == 4)
					regs[reg/5].ub = pop8();
			}

			if (i2 == 0x02) { // push ll num
				LOG("push %016lx%016lx\n", (uint64)(param.ull >> 64), param.ul);

				regs[pc].ul += 16; // ll num size

				push(param.ull);
			}

			if (i2 == 0x03) { // push l num
				LOG("push %016lx\n", param.ul);

				regs[pc].ul += 8; // l num size

				push(param.ul);
			}

			if (i2 == 0x04) { // push i num
				LOG("push %08x\n", param.ui);

				regs[pc].ul += 4; // i num size

				push(param.ui);
			}

			if (i2 == 0x05) { // push s num
				LOG("push %04x\n", param.us);

				regs[pc].ul += 2; // s num size

				push(param.us);
			}

			if (i2 == 0x06) { // push b num
				LOG("push %02x\n", param.ub);

				regs[pc].ul += 1; // b num size

				push(param.ub);
			}
		}


		if (i1 == 0x03) { // jmp
			regs[pc].ul += 1; // reg size
			regs[pc].ul += 8; // addr size

			if (i2 == 0x00) {
				LOG("je %016lx\n", param.ul);

				if (get_flag_state(FLAG::equals))
					regs[pc].ul = param.ul;
			}

			if (i2 == 0x01) {
				LOG("jne %016lx\n", param.ul);

				if (~get_flag_state(FLAG::equals))
					regs[pc].ul = param.ul;
			}

			if (i2 == 0x02) {
				LOG("jl %016lx\n", param.ul);

				if (get_flag_state(FLAG::overflow))
					regs[pc].ul = param.ul;
			}

			if (i2 == 0x03) {
				LOG("jb %016lx\n", param.ul);

				if (get_flag_state(FLAG::more))
					regs[pc].ul = param.ul;
			}

			if (i2 == 0x04) {
				LOG("jle %016lx\n", param.ul);

				if (get_flag_state(FLAG::overflow) || get_flag_state(FLAG::equals))
					regs[pc].ul = param.ul;
			}

			if (i2 == 0x05) {
				LOG("jbe %016lx\n", param.ul);

				if (get_flag_state(FLAG::more) || get_flag_state(FLAG::equals))
					regs[pc].ul = param.ul;
			}

			if (i2 == 0x06) { // call
				LOG("call %016lx\n", param.ul);

				push(regs[pc].ul);
				regs[pc].ul = param.ul;
			}

			if (i2 == 0x08) { // ret
				LOG("ret\n");
				regs[pc].ul = pop64();
			}
		}


		if (i1 == 0xff) { // hlt
			LOG("hlt");
			set_flag_state(FLAG::running, 0);
		}
	}
};
