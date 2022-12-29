#pragma once


#ifndef DEBUG
	#define LOG
#else
	#define LOG(...) printf(__VA_ARGS__)
#endif


#define pc r31
#define sp r30
#define lo r29
#define ip r28


// r - 128 bit, l - 64 bit, i - 32 bit, s - 16 bit, b - 8 bit

enum REG {
	r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,
	r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,

	COUNT
};

enum FLAG {
	running,
	equals,
	more,
	overflow,
};


typedef unsigned __int128 uint128;
typedef __int128 int128;
typedef unsigned long uint64;
typedef long int64;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;


union Register {
	uint128 ull;
	int128 ll;
	uint64 ul;
	int64 l;
	uint32 ui;
	int32 i;
	uint16 us;
	int16 s;
	uint8 ub;
	int8 b;
};


void print_register_by_id(uint16 id) {
	if (id % 5 == 0)
		LOG("r%02i", id/5);
	if (id % 5 == 1)
		LOG("l%02i", id/5);
	if (id % 5 == 2)
		LOG("i%02i", id/5);
	if (id % 5 == 3)
		LOG("s%02i", id/5);
	if (id % 5 == 4)
		LOG("b%02i", id/5);
}
