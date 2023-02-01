#include <core.h>


extern uint8 *ram;


Core::Core() {

}


void Core::init(uint8 _id) {
	id = _id;

	clear();
}


void Core::clear() {
	for (int i = 0; i < REGISTERS_COUNT; i++)
		regs[i].ull = 0;
}


void Core::set_flag(uint8 id, uint8 val) {
	if (val == 0) {
		flag &= ~(1 << id);
	} else {
		flag |= 1 << id;
	}
}

uint8 Core::get_flag(uint8 id) {
	return (flag >> id) & 1;
}


void Core::print_info() {
	LOG("Core: %0x\n", id);

	for (int i = 0; i < REGISTERS_COUNT; i++) {
		LOG("\t");
		log_register(i * 5);
		LOG(" %016lx%016lx %016lx %08x %04x %02x\n",
				(uint64)(regs[i].ull >> 64), regs[i].ul, regs[i].ul, regs[i].ui, regs[i].us, regs[i].ub);
	}
}

void Core::illegal() {
	LOG("\e[1;31m%-6s\e[m", "Illegal instruction!\n");
}


uint8 Core::pop1(uint64 addr) {
	return ram[addr];
}

uint16 Core::pop2(uint64 addr) {
	return
		((uint16)(ram[addr + 0]) << 0) |
		((uint16)(ram[addr + 1]) << 8);
}

uint32 Core::pop4(uint64 addr) {
	return
		((uint32)(ram[addr + 0]) <<  0) |
		((uint32)(ram[addr + 1]) <<  8) |
		((uint32)(ram[addr + 2]) << 16) |
		((uint32)(ram[addr + 3]) << 24);
}

uint64 Core::pop8(uint64 addr) {
	return
		((uint64)(ram[addr + 0]) <<  0) |
		((uint64)(ram[addr + 1]) <<  8) |
		((uint64)(ram[addr + 2]) << 16) |
		((uint64)(ram[addr + 3]) << 24) |
		((uint64)(ram[addr + 4]) << 32) |
		((uint64)(ram[addr + 5]) << 40) |
		((uint64)(ram[addr + 6]) << 48) |
		((uint64)(ram[addr + 7]) << 56);
}

uint128 Core::pop16(uint64 addr) {
	return
		((uint128)(ram[addr + 0]) << 0) |
		((uint128)(ram[addr + 1]) << 8) |
		((uint128)(ram[addr + 2]) << 16) |
		((uint128)(ram[addr + 3]) << 24) |
		((uint128)(ram[addr + 4]) << 32) |
		((uint128)(ram[addr + 5]) << 40) |
		((uint128)(ram[addr + 6]) << 48) |
		((uint128)(ram[addr + 7]) << 56) |
		((uint128)(ram[addr + 8]) << 64) |
		((uint128)(ram[addr + 9]) << 72) |
		((uint128)(ram[addr +10]) << 80) |
		((uint128)(ram[addr +11]) << 88) |
		((uint128)(ram[addr +12]) << 96) |
		((uint128)(ram[addr +13]) << 104) |
		((uint128)(ram[addr +14]) << 112) |
		((uint128)(ram[addr +15]) << 120);
}

template<class T> void Core::push(T val, uint64 addr) {
	for (int i = 0; i < sizeof(val); i++) {
		ram[addr + i] = (val << (i << 3)) & 0xff;
	}
};


template<class T> T Core::ALU(T a, T b, uint8 op) {
	set_flag(FLAG_EQUALS, 0);
	set_flag(FLAG_LESS, 0);
	set_flag(FLAG_MORE, 0);

	T sum = a + b;
	T sub = a - b;

	if (a == b)
		set_flag(FLAG_EQUALS, 1);

	if (a > b)
		set_flag(FLAG_MORE, 1);

	if (a < b)
		set_flag(FLAG_LESS, 1);

	if (op == ALU_SUM)
		return sum;

	if (op == ALU_SUB)
		return sub;

	return 0;
}


void Core::step() {
	print_info();

	uint8 i1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
	uint8 i2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

	uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

	LOG("%02x %02x %02x\n", i1,i2,p1);

	if (i1 == 0x00) { // special
		if (i2 == 0x00) { // halt
			LOG("hlt\n");

			set_flag(FLAG_RUNNING, 0);
		}
	}

	if (i1 == 0x01) { // registers
		if (i2 == 0x00) { // num -> reg
			LOG("mov ");
			log_register(p1);

			if (p1 % 5 == 0) {
				uint128 val = pop16(regs[REG_PC].ul + regs[REG_LO].ul);
				regs[REG_PC].ul += 16;

				regs[p1 / 5].ull = val;

				LOG(" %016lx%016lx", (uint64)(val >> 64), val);
			}

			if (p1 % 5 == 1) {
				uint64 val = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
				regs[REG_PC].ul += 8;

				regs[p1 / 5].ul = val;

				LOG(" %016lx", val);
			}

			if (p1 % 5 == 2) {
				uint32 val = pop4(regs[REG_PC].ul + regs[REG_LO].ul);
				regs[REG_PC].ul += 4;

				regs[p1 / 5].ui = val;

				LOG(" %08x", regs[p1 / 5].ui);
			}

			if (p1 % 5 == 3) {
				uint16 val = pop2(regs[REG_PC].ul + regs[REG_LO].ul);
				regs[REG_PC].ul += 2;

				regs[p1 / 5].us = val;

				LOG(" %04x", val);
			}

			if (p1 % 5 == 4) {
				uint8 val = pop1(regs[REG_PC].ul + regs[REG_LO].ul);
				regs[REG_PC].ul += 1;

				regs[p1 / 5].ub = val;

				LOG(" %02x", val);
			}

			LOG("\n");
		}

		if (i2 == 0x01) { // reg -> reg
			uint8 p2 = ram[regs[REG_PC].ul++];

			LOG("mov ");
			log_register(p1);
			LOG(" ");
			log_register(p2);
			LOG("\n");

			if (p1 % 5 != p2 % 5) {
				illegal();
			} else {
				if (p1 % 5 == 0)
					regs[p1 / 5].ull = regs[p2 / 5].ull;

				if (p1 % 5 == 1)
					regs[p1 / 5].ul = regs[p2 / 5].ul;

				if (p1 % 5 == 2)
					regs[p1 / 5].ui = regs[p2 / 5].ui;

				if (p1 % 5 == 3)
					regs[p1 / 5].us = regs[p2 / 5].us;

				if (p1 % 5 == 4)
					regs[p1 / 5].ub = regs[p2 / 5].ub;
			}
		}

		if (i2 == 0x02) { // ram -> reg
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 8;

			LOG("mov ");
			log_register(p1);
			LOG(" [%016lx]\n", addr);

			addr += regs[REG_LO].ul;

			if (p1 % 5 == 0)
				regs[p1 / 5].ull = pop16(addr);

			if (p1 % 5 == 1)
				regs[p1 / 5].ul = pop8(addr);

			if (p1 % 5 == 2)
				regs[p1 / 5].ui = pop4(addr);

			if (p1 % 5 == 3)
				regs[p1 / 5].us = pop2(addr);

			if (p1 % 5 == 4)
				regs[p1 / 5].ub = pop1(addr);
		}

		if (i2 == 0x03) { // reg -> ram
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 8;

			LOG("mov ");
			log_register(p1);
			LOG(" {%016lx}\n", addr);

			addr += regs[REG_LO].ul;

			if (p1 % 5 == 0)
				push(regs[p1 / 5].ull, addr);

			if (p1 % 5 == 1)
				push(regs[p1 / 5].ul, addr);

			if (p1 % 5 == 2)
				push(regs[p1 / 5].ui, addr);

			if (p1 % 5 == 3)
				push(regs[p1 / 5].us, addr);

			if (p1 % 5 == 4)
				push(regs[p1 / 5].ub, addr);
		}

		if (i2 == 0x04) { // ram -> reg abs.
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 8;

			LOG("mov ");
			log_register(p1);
			LOG(" [%016lx]a\n", addr);

			if (p1 % 5 == 0)
				push(regs[p1 / 5].ull, addr);

			if (p1 % 5 == 1)
				push(regs[p1 / 5].ul, addr);

			if (p1 % 5 == 2)
				push(regs[p1 / 5].ui, addr);

			if (p1 % 5 == 3)
				push(regs[p1 / 5].us, addr);

			if (p1 % 5 == 4)
				push(regs[p1 / 5].ub, addr);
		}

		if (i2 == 0x05) { // reg -> ram abs.
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 8;

			LOG("mov ");
			log_register(p1);
			LOG(" {%016lx}a\n", addr);

			if (p1 % 5 == 0)
				push(regs[p1 / 5].ull, addr);

			if (p1 % 5 == 1)
				push(regs[p1 / 5].ul, addr);

			if (p1 % 5 == 2)
				push(regs[p1 / 5].ui, addr);

			if (p1 % 5 == 3)
				push(regs[p1 / 5].us, addr);

			if (p1 % 5 == 4)
				push(regs[p1 / 5].ub, addr);
		}
	}

	if (i1 == 0x02) { // ALU
		if (i2 = 0x00) { // sum
			uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
			uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

			LOG("sum ");
			log_register(p1);
			log_register(p2);
			log_register(p3);
			LOG("\n");

			if (p1 % 5 != p2 % 5 || p1 % 5 != p3 % 5 || p2 % 5 != p3 % 5) {
				illegal();
			} else {
				if (p1 % 5 == 0)
					regs[p1 / 5].ull = ALU(regs[p2 / 5].ull, regs[p3 / 5].ull, ALU_SUM);

				if (p1 % 5 == 1)
					regs[p1 / 5].ul = ALU(regs[p2 / 5].ul, regs[p3 / 5].ul, ALU_SUM);

				if (p1 % 5 == 2)
					regs[p1 / 5].ui = ALU(regs[p2 / 5].ui, regs[p3 / 5].ui, ALU_SUM);

				if (p1 % 5 == 3)
					regs[p1 / 5].us = ALU(regs[p2 / 5].us, regs[p3 / 5].us, ALU_SUM);

				if (p1 % 5 == 4)
					regs[p1 / 5].ub = ALU(regs[p2 / 5].ub, regs[p3 / 5].ub, ALU_SUM);
			}
		}

		if (i2 = 0x01) { // sub
			uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
			uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

			LOG("sum ");
			log_register(p1);
			log_register(p2);
			log_register(p3);
			LOG("\n");

			if (p1 % 5 != p2 % 5 || p1 % 5 != p3 % 5 || p2 % 5 != p3 % 5) {
				illegal();
			} else {
				if (p1 % 5 == 0)
					regs[p1 / 5].ull = ALU(regs[p2 / 5].ull, regs[p3 / 5].ull, ALU_SUB);

				if (p1 % 5 == 1)
					regs[p1 / 5].ul = ALU(regs[p2 / 5].ul, regs[p3 / 5].ul, ALU_SUB);

				if (p1 % 5 == 2)
					regs[p1 / 5].ui = ALU(regs[p2 / 5].ui, regs[p3 / 5].ui, ALU_SUB);

				if (p1 % 5 == 3)
					regs[p1 / 5].us = ALU(regs[p2 / 5].us, regs[p3 / 5].us, ALU_SUB);

				if (p1 % 5 == 4)
					regs[p1 / 5].ub = ALU(regs[p2 / 5].ub, regs[p3 / 5].ub, ALU_SUB);
			}
		}

		if (i2 = 0x02) { // cmp
			uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
			uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

			LOG("cmp ");
			log_register(p2);
			log_register(p3);
			LOG("\n");

			if (p2 % 5 != p3 % 5) {
				illegal();
			} else {
				if (p1 % 5 == 0)
					ALU(regs[p2 / 5].ull, regs[p3 / 5].ull, ALU_SUB);

				if (p1 % 5 == 1)
					ALU(regs[p2 / 5].ul, regs[p3 / 5].ul, ALU_SUB);

				if (p1 % 5 == 2)
					ALU(regs[p2 / 5].ui, regs[p3 / 5].ui, ALU_SUB);

				if (p1 % 5 == 3)
					ALU(regs[p2 / 5].us, regs[p3 / 5].us, ALU_SUB);

				if (p1 % 5 == 4)
					ALU(regs[p2 / 5].ub, regs[p3 / 5].ub, ALU_SUB);
			}
		}
	}

	if (i1 == 0x03) { // stack
		if (i2 == 0x00) { // push 8b num
			uint8 p2 = pop1(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 1;

			LOG("push %02x\n", p2);

			regs[REG_SP].ul -= 1;

			push(p2, regs[REG_SP].ul + regs[REG_LO].ul);
		}

		if (i2 == 0x01) { // push 16b num
			uint16 p2 = pop2(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 2;

			LOG("push %04x\n", p2);

			regs[REG_SP].ul -= 2;

			push(p2, regs[REG_SP].ul + regs[REG_LO].ul);
		}

		if (i2 == 0x02) { // push 32b num
			uint32 p2 = pop4(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 4;

			LOG("push %08x\n", p2);

			regs[REG_SP].ul -= 4;

			push(p2, regs[REG_SP].ul + regs[REG_LO].ul);
		}

		if (i2 == 0x03) { // push 64b num
			uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 8;

			LOG("push %016lx\n", p2);

			regs[REG_SP].ul -= 8;

			push(p2, regs[REG_SP].ul + regs[REG_LO].ul);
		}

		if (i2 == 0x04) { // push 128b num
			uint128 p2 = pop16(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 16;

			LOG("push %016lx%016lx\n", (uint64)(p2 >> 64), (uint64)p2);

			regs[REG_SP].ul -= 16;

			push(p2, regs[REG_SP].ul + regs[REG_LO].ul);
		}

		if (i2 == 0x05) { // push reg
			LOG("push ");
			log_register(p1);
			LOG("\n");

			if (p1 % 5 == 0) {
				regs[REG_SP].ul -= 16;

				push(regs[p1 / 5].ull, regs[REG_SP].ul + regs[REG_LO].ul);
			}
			if (p1 % 5 == 1) {
				regs[REG_SP].ul -= 8;

				push(regs[p1 / 5].ul, regs[REG_SP].ul + regs[REG_LO].ul);
			}

			if (p1 % 5 == 2) {
				regs[REG_SP].ul -= 4;

				push(regs[p1 / 5].ui, regs[REG_SP].ul + regs[REG_LO].ul);
			}

			if (p1 % 5 == 3) {
				regs[REG_SP].ul -= 2;

				push(regs[p1 / 5].us, regs[REG_SP].ul + regs[REG_LO].ul);
			}

			if (p1 % 5 == 4) {
				regs[REG_SP].ul -= 1;

				push(regs[p1 / 5].ub, regs[REG_SP].ul + regs[REG_LO].ul);
			}
		}

		if (i2 == 0x06) { // pop reg
			LOG("pop ");
			log_register(p1);
			LOG("\n");

			if (p1 % 5 == 0) {
				regs[p1 / 5].ull = pop16(regs[REG_SP].ul + regs[REG_LO].ul);

				regs[REG_SP].ul += 16;
			}

			if (p1 % 5 == 1) {
				regs[p1 / 5].ul = pop8(regs[REG_SP].ul + regs[REG_LO].ul);

				regs[REG_SP].ul += 8;
			}

			if (p1 % 5 == 2) {
				regs[p1 / 5].ui = pop4(regs[REG_SP].ul + regs[REG_LO].ul);

				regs[REG_SP].ul += 4;
			}

			if (p1 % 5 == 3) {
				regs[p1 / 5].us = pop2(regs[REG_SP].ul + regs[REG_LO].ul);

				regs[REG_SP].ul += 2;
			}

			if (p1 % 5 == 4) {
				regs[p1 / 5].ub = pop1(regs[REG_SP].ul + regs[REG_LO].ul);

				regs[REG_SP].ul += 1;
			}
		}
	}

	if (i1 == 0x04) { // jumps
		if (i2 == 0x00) { // jl
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);

			LOG("jl %016lx\n", addr);

			if (get_flag(FLAG_LESS))
				regs[REG_PC].ul = addr;
		}

		if (i2 == 0x01) { // jb
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);

			LOG("jb %016lx\n", addr);

			if (get_flag(FLAG_MORE))
				regs[REG_PC].ul = addr;
		}

		if (i2 == 0x02) { // je
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);

			LOG("je %016lx\n", addr);

			if (get_flag(FLAG_EQUALS))
				regs[REG_PC].ul = addr;
		}

		if (i2 == 0x03) { // jbe
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);

			LOG("jbe %016lx\n", addr);

			if (get_flag(FLAG_MORE) && get_flag(FLAG_EQUALS))
				regs[REG_PC].ul = addr;
		}

		if (i2 == 0x04) { // jle
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);

			LOG("jle %016lx\n", addr);

			if (get_flag(FLAG_LESS) && get_flag(FLAG_EQUALS))
				regs[REG_PC].ul = addr;
		}

		if (i2 == 0x05) { // jne
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);

			LOG("jne %016lx\n", addr);

			if (!get_flag(FLAG_EQUALS))
				regs[REG_PC].ul = addr;
		}

		if (i2 == 0x06) { // call
			uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
			regs[REG_PC].ul += 8;

			LOG("call %016lx\n", addr);

			regs[REG_SP].ul -= 8;

			push(regs[REG_PC].ul, regs[REG_SP].ul + regs[REG_LO].ul);

			regs[REG_PC].ul = addr + regs[REG_LO].ul;
		}

		if (i2 == 0x07) { // ret
			LOG("ret\n");

			regs[REG_PC].ul = pop8(regs[REG_SP].ul + regs[REG_LO].ul);

			regs[REG_SP].ul += 8;
		}
	}
}

