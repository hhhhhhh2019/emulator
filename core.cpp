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
		regs[i].ur = 0;
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
		LOG("\t%-3d %016lx%016lx", i,
				(uint64)(regs[i].ur >> 64), regs[i].ul);//, regs[i].ul, regs[i].ui, regs[i].us, regs[i].ub);

		if (i % 2 == 1)
			putc('\n', stdout);
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
		ram[addr + i] = (val >> (i << 3)) & 0xff;
	}
};


template<class T> T Core::ALU(T a, T b, uint8 op) {
	set_flag(FLAG_EQUALS, 0);
	set_flag(FLAG_LESS, 0);
	set_flag(FLAG_MORE, 0);

	T sign_bit = (T)1 << (sizeof(T) * 8 - 1);
	T ia = a;
	T ib = b;

	if ((a & sign_bit) == sign_bit)
		ia = ~a + 1;

	if ((b & sign_bit) == sign_bit)
		ib = ~b + 1;

	T sum = a + b;
	T sub = a - b;
	T mul = a * b;
	T div = a / b;
	T idiv = ia / ib;

	if (((a & sign_bit) == sign_bit) ^ ((b & sign_bit) == sign_bit)) {
		idiv = ~(idiv - 1);
	}

	//T imul = a * b;//(a * b) & ((1 << (sizeof(T) >> 1)) - 1);

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

	if (op == ALU_MUL)
		return mul;

	/*if (op == ALU_IMUL)
		return imul;*/

	if (op == ALU_DIV)
		return div;

	if (op == ALU_IDIV)
		return idiv;

	return 0;
}


void Core::step() {
	print_info();

	uint8 i1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

	if (i1 == 0x00) { // nop
		LOG("nop\n");
	}

	if (i1 == 0x01) { // hlt
		LOG("hlt\n");
		set_flag(FLAG_RUNNING, 0);
	}

	if (i1 == 0x02) { // movb n
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movb %%%d %02x\n", p1, p2);

		regs[p1].ub = p2;
	}

	if (i1 == 0x03) { // movs n
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint16 p2 = pop2(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 2;

		LOG("movs %%%d %04x\n", p1, p2);

		regs[p1].us = p2;
	}

	if (i1 == 0x04) { // movi n
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint32 p2 = pop4(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 4;

		LOG("movi %%%d %08x\n", p1, p2);

		regs[p1].ui = p2;
	}

	if (i1 == 0x05) { // movl n
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movl %%%d %016lx\n", p1, p2);

		regs[p1].ul = p2;
	}

	if (i1 == 0x06) { // movr n
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint128 p2 = pop16(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 16;

		LOG("movr %%%d %016lx%016lx\n", p1, (uint64)(p2 >> 64),(uint64)p2);

		regs[p1].ur = p2;
	}

	if (i1 == 0x07) { // movb r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movb %%%d %%%d\n", p1,p2);

		regs[p1].ub = regs[p1].ub;
	}

	if (i1 == 0x08) { // movs r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movs %%%d %%%d\n", p1,p2);

		regs[p1].us = regs[p1].us;
	}

	if (i1 == 0x09) { // movi r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movi %%%d %%%d\n", p1,p2);

		regs[p1].ui = regs[p1].ui;
	}

	if (i1 == 0x0a) { // movl r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movl %%%d %%%d\n", p1,p2);

		regs[p1].ul = regs[p1].ul;
	}

	if (i1 == 0x0b) { // movr r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movr %%%d %%%d\n", p1,p2);

		regs[p1].ur = regs[p1].ur;
	}

	if (i1 == 0x0c) { // movb from RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movb %%%d [%016lx]\n", p1,p2);

		regs[p1].ub = pop1(p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x0d) { // movs from RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movs %%%d [%016lx]\n", p1,p2);

		regs[p1].us = pop2(p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x0e) { // movi from RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movi %%%d [%016lx]\n", p1,p2);

		regs[p1].ui = pop4(p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x0f) { // movl from RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movl %%%d [%016lx]\n", p1,p2);

		regs[p1].ul = pop8(p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x10) { // movr from RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movr %%%d [%016lx]\n", p1,p2);

		regs[p1].ur = pop16(p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x11) { // movb to RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movb [%016lx] %%%d\n", p2,p1);

		push(regs[p1].ub, p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x12) { // movs to RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movs [%016lx] %%%d\n", p2,p1);

		push(regs[p1].us,p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x13) { // movi to RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movi [%016lx] %%%d\n", p2,p1);

		push(regs[p1].ui,p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x14) { // movl to RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movl [%016lx] %%%d\n", p2,p1);

		push(regs[p1].ul,p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x15) { // movr to RAM
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movr [%016lx] %%%d\n", p2,p1);

		push(regs[p1].ur,p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x16) { // movb from RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movb %%%d {%016lx}\n", p1,p2);

		regs[p1].ub = pop1(p2);
	}

	if (i1 == 0x17) { // movs from RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movs %%%d {%016lx}\n", p1,p2);

		regs[p1].us = pop2(p2 + regs[REG_LO].ul);
	}

	if (i1 == 0x18) { // movi from RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movi %%%d {%016lx}\n", p1,p2);

		regs[p1].ui = pop4(p2);
	}

	if (i1 == 0x19) { // movl from RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movl %%%d {%016lx}\n", p1,p2);

		regs[p1].ul = pop8(p2);
	}

	if (i1 == 0x1a) { // movr from RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movr %%%d {%016lx}\n", p1,p2);

		regs[p1].ur = pop16(p2);
	}

	if (i1 == 0x1b) { // movb to RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movb {%016lx} %%%d\n", p2,p1);

		push(regs[p1].ub, p2);
	}

	if (i1 == 0x1c) { // movs to RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movs {%016lx} %%%d\n", p2,p1);

		push(regs[p1].us,p2);
	}

	if (i1 == 0x1d) { // movi to RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movi {%016lx} %%%d\n", p2,p1);

		push(regs[p1].ui,p2);
	}

	if (i1 == 0x1e) { // movl to RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movl {%016lx} %%%d\n", p2,p1);

		push(regs[p1].ul,p2);
	}

	if (i1 == 0x1f) { // movr to RAMg
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint64 p2 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("movr {%016lx} %%%d\n", p2,p1);

		push(regs[p1].ur,p2);
	}

	if (i1 == 0x20) { // movb from RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movb %%%d [%%%d]\n", p1,p2);

		regs[p1].ub = pop1(regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x21) { // movs from RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movs %%%d [%%%d]\n", p1,p2);

		regs[p1].us = pop2(regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x22) { // movi from RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movi %%%d [%%%d]\n", p1,p2);

		regs[p1].ui = pop4(regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x23) { // movl from RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movl %%%d [%%%d]\n", p1,p2);

		regs[p1].ul = pop8(regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x24) { // movr from RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movr %%%d [%%%d]\n", p1,p2);

		regs[p1].ur = pop16(regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x25) { // movb to RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movb [%%%d] %%%d\n", p2,p1);

		push(regs[p1].ub,regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x26) { // movs to RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movs [%%%d] %%%d\n", p2,p1);

		push(regs[p1].us,regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x27) { // movi to RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movi [%%%d] %%%d\n", p2,p1);

		push(regs[p1].ui,regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x28) { // movl to RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movl [%%%d] %%%d\n", p2,p1);

		push(regs[p1].ul,regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x29) { // movr to RAMr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("movr [%%%d] %%%d\n", p2,p1);

		push(regs[p1].ur,regs[p2].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x2a) { // pushb n
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pushb %02x\n", p1);

		regs[REG_SP].ul -= 1;
		push(p1,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x2b) { // pushs n
		uint16 p1 = pop2(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 2;

		LOG("pushs %04x\n", p1);

		regs[REG_SP].ul -= 2;
		push(p1,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x2c) { // pushi n
		uint32 p1 = pop4(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 4;

		LOG("pushi %08x\n", p1);

		regs[REG_SP].ul -= 4;
		push(p1,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x2d) { // pushl n
		uint64 p1 = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("pushl %016lx\n", p1);

		regs[REG_SP].ul -= 8;
		push(p1,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x2e) { // pushr n
		uint128 p1 = pop16(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 16;

		LOG("pushi %016lx%016lx\n", p1>>64,p1);

		regs[REG_SP].ul -= 16;
		push(p1,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x2f) { // pushb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pushb %%%d\n", p1);

		regs[REG_SP].ul -= 1;
		push(regs[p1].ub,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x30) { // pushs
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pushs %%%d\n", p1);

		regs[REG_SP].ul -= 2;
		push(regs[p1].us,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x31) { // pushi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pushi %%%d\n", p1);

		regs[REG_SP].ul -= 4;
		push(regs[p1].ui,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x32) { // pushl
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pushl %%%d\n", p1);

		regs[REG_SP].ul -= 8;
		push(regs[p1].ul,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x33) { // pushr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pushr %%%d\n", p1);

		regs[REG_SP].ul -= 16;
		push(regs[p1].ur,regs[REG_SP].ul + regs[REG_LO].ul);
	}

	if (i1 == 0x34) { // popb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("popb %%%d\n", p1);

		regs[p1].ub = pop1(regs[REG_SP].ul + regs[REG_LO].ul);
		regs[REG_SP].ul += 1;
	}

	if (i1 == 0x35) { // pops
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("pops %%%d\n", p1);

		regs[p1].us = pop2(regs[REG_SP].ul + regs[REG_LO].ul);
		regs[REG_SP].ul += 2;
	}

	if (i1 == 0x36) { // popi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("popi %%%d\n", p1);

		regs[p1].ui = pop4(regs[REG_SP].ul + regs[REG_LO].ul);
		regs[REG_SP].ul += 4;
	}

	if (i1 == 0x37) { // popl
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("popl %%%d\n", p1);

		regs[p1].ul = pop8(regs[REG_SP].ul + regs[REG_LO].ul);
		regs[REG_SP].ul += 8;
	}

	if (i1 == 0x38) { // popr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("popr %%%d\n", p1);

		regs[p1].ur = pop16(regs[REG_SP].ul + regs[REG_LO].ul);
		regs[REG_SP].ul += 16;
	}

	if (i1 == 0x39) { // sumb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("sumb %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ub = ALU(regs[p2].ub, regs[p3].ub, ALU_SUM);
	}

	if (i1 == 0x3a) { // sums
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("sums %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].us = ALU(regs[p2].us, regs[p3].us, ALU_SUM);
	}

	if (i1 == 0x3b) { // sumi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("sumi %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ui = ALU(regs[p2].ui, regs[p3].ui, ALU_SUM);
	}

	if (i1 == 0x3c) { // suml
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("suml %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ul = ALU(regs[p2].ul, regs[p3].ul, ALU_SUM);
	}

	if (i1 == 0x3d) { // sumr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("sumr %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_SUM);
	}

	if (i1 == 0x3e) { // subb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("subb %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ub = ALU(regs[p2].ub, regs[p3].ub, ALU_SUB);
	}

	if (i1 == 0x3f) { // subs
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("subs %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].us = ALU(regs[p2].us, regs[p3].us, ALU_SUB);
	}

	if (i1 == 0x40) { // subi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("subi %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ui = ALU(regs[p2].ui, regs[p3].ui, ALU_SUB);
	}

	if (i1 == 0x41) { // subl
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("subl %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ul = ALU(regs[p2].ul, regs[p3].ul, ALU_SUB);
	}

	if (i1 == 0x42) { // subr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("subr %%%d %%%d %%%d\n", p1, p2, p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_SUB);
	}

	if (i1 == 0x43) { // mulb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("mulb %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].us = ALU(regs[p2].us, regs[p3].us, ALU_MUL);
	}

	if (i1 == 0x44) { // muls
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("muls %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ui = ALU(regs[p2].ui, regs[p3].ui, ALU_MUL);
	}

	if (i1 == 0x45) { // muli
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("muli %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ul = ALU(regs[p2].ul, regs[p3].ul, ALU_MUL);
	}

	if (i1 == 0x46) { // mull
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("mull %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_MUL);
	}

	if (i1 == 0x47) { // mulr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("mulr %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_MUL);
	}

	if (i1 == 0x48) { // imulb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("imulb %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ub = ALU(regs[p2].ub, regs[p3].ub, ALU_MUL);
	}

	if (i1 == 0x49) { // imuls
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("imuls %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].us = ALU(regs[p2].us, regs[p3].us, ALU_MUL);
	}

	if (i1 == 0x4a) { // imuli
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("imuli %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ui = ALU(regs[p2].ui, regs[p3].ui, ALU_MUL);
	}

	if (i1 == 0x4b) { // imull
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("imull %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ul = ALU(regs[p2].ul, regs[p3].ul, ALU_MUL);
	}

	if (i1 == 0x4c) { // imulr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("imulr %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_MUL);
	}

	if (i1 == 0x4d) { // divb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("divb %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ub = ALU(regs[p2].ub, regs[p3].ub, ALU_DIV);
	}

	if (i1 == 0x4e) { // divs
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("divs %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].us = ALU(regs[p2].us, regs[p3].us, ALU_DIV);
	}

	if (i1 == 0x4f) { // divi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("divi %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ui = ALU(regs[p2].ui, regs[p3].ui, ALU_DIV);
	}

	if (i1 == 0x50) { // divl
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("divl %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ul = ALU(regs[p2].ul, regs[p3].ul, ALU_DIV);
	}

	if (i1 == 0x51) { // divr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("divr %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_DIV);
	}

	if (i1 == 0x52) { // idivb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("idivb %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ub = ALU(regs[p2].ub, regs[p3].ub, ALU_IDIV);
	}

	if (i1 == 0x53) { // idivs
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("idivs %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].us = ALU(regs[p2].us, regs[p3].us, ALU_IDIV);
	}

	if (i1 == 0x54) { // idivi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("idivi %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ui = ALU(regs[p2].ui, regs[p3].ui, ALU_IDIV);
	}

	if (i1 == 0x55) { // idivl
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("idivl %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ul = ALU(regs[p2].ul, regs[p3].ul, ALU_IDIV);
	}

	if (i1 == 0x56) { // idivr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p3 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("idivr %%%d %%%d %%%d\n", p1,p2,p3);

		regs[p1].ur = ALU(regs[p2].ur, regs[p3].ur, ALU_IDIV);
	}

	if (i1 == 0x57) { // cmpb
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("cmpb %%%d %%%d\n", p1,p2);

		ALU(regs[p1].ub, regs[p2].ub, ALU_SUB);
	}

	if (i1 == 0x58) { // cmps
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("cmps %%%d %%%d\n", p1,p2);

		ALU(regs[p1].us, regs[p2].us, ALU_SUB);
	}

	if (i1 == 0x59) { // cmpi
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("cmpi %%%d %%%d\n", p1,p2);

		ALU(regs[p1].ui, regs[p2].ui, ALU_SUB);
	}

	if (i1 == 0x5a) { // cmpl
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("cmpl %%%d %%%d\n", p1,p2);

		ALU(regs[p1].ul, regs[p2].ul, ALU_SUB);
	}

	if (i1 == 0x5b) { // cmpr
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];
		uint8 p2 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("cmpr %%%d %%%d\n", p1,p2);

		ALU(regs[p1].ur, regs[p2].ur, ALU_SUB);
	}

	if (i1 == 0x5c) { // je
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("je %16lx\n", addr);

		if ((flag & FLAG_EQUALS) == FLAG_EQUALS)
			regs[REG_PC].ul = addr;
	}

	if (i1 == 0x5d) { // jne
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("jne %16lx\n", addr);

		if ((flag & FLAG_EQUALS) != FLAG_EQUALS)
			regs[REG_PC].ul = addr;
	}

	if (i1 == 0x5e) { // jb
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("jb %16lx\n", addr);

		if ((flag & FLAG_MORE) == FLAG_MORE)
			regs[REG_PC].ul = addr;
	}

	if (i1 == 0x5f) { // jbe
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("jbe %16lx\n", addr);

		if ((flag & FLAG_MORE) != FLAG_MORE || (flag & FLAG_EQUALS) == FLAG_EQUALS)
			regs[REG_PC].ul = addr;
	}

	if (i1 == 0x60) { // jl
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("jl %16lx\n", addr);

		if ((flag & FLAG_LESS) == FLAG_LESS)
			regs[REG_PC].ul = addr;
	}

	if (i1 == 0x61) { // jle
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("jle %16lx\n", addr);

		if ((flag & FLAG_LESS) == FLAG_LESS || (flag & FLAG_EQUALS) == FLAG_EQUALS)
			regs[REG_PC].ul = addr;
	}

	if (i1 == 0x62) { // je r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("je %%%d\n", p1);

		if ((flag & FLAG_EQUALS) == FLAG_EQUALS)
			regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x63) { // jne r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("jne %%%d\n", p1);

		if ((flag & FLAG_EQUALS) != FLAG_EQUALS)
			regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x64) { // jb r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("jb %%%d\n", p1);

		if ((flag & FLAG_MORE) == FLAG_MORE)
			regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x65) { // jbe r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("jbe %%%d\n", p1);

		if ((flag & FLAG_MORE) == FLAG_MORE || (flag & FLAG_EQUALS) == FLAG_EQUALS)
			regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x66) { // jl r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("jl %%%d\n", p1);

		if ((flag & FLAG_LESS) == FLAG_LESS)
			regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x67) { // jle r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("jle %%%d\n", p1);

		if ((flag & FLAG_LESS) == FLAG_LESS || (flag & FLAG_EQUALS) == FLAG_EQUALS)
			regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x68) { // call
		uint64 addr = pop8(regs[REG_PC].ul + regs[REG_LO].ul);
		regs[REG_PC].ul += 8;

		LOG("call %16lx\n", addr);

		regs[REG_SP].ul -= 8;
		push(regs[REG_PC].ul, regs[REG_SP].ul + regs[REG_LO].ul);

		regs[REG_PC].ul = addr;
	}

	if (i1 == 0x69) { // call r
		uint8 p1 = ram[regs[REG_PC].ul++ + regs[REG_LO].ul];

		LOG("call %%%d\n", p1);

		regs[REG_SP].ul -= 8;
		push(regs[REG_PC].ul, regs[REG_SP].ul + regs[REG_LO].ul);

		regs[REG_PC].ul = regs[p1].ul;
	}

	if (i1 == 0x6a) { // ret
		LOG("ret\n");

		regs[REG_PC].ul = pop8(regs[REG_SP].ul + regs[REG_LO].ul);
		regs[REG_SP].ul += 8;
	}
}
