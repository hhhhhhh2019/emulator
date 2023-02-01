#pragma once

#include <utils.h>


// 256 / 5
#define REGISTERS_COUNT 51
#define REG_PC 50
#define REG_SP 49
#define REG_LO 48

#define FLAG_RUNNING 1
#define FLAG_EQUALS 2
#define FLAG_LESS 3
#define FLAG_MORE 4


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
