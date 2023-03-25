#pragma once

#include <utils.h>

#define REGISTERS_COUNT 16
#define REG_PC REGISTERS_COUNT - 1
#define REG_SP REGISTERS_COUNT - 2
#define REG_LO REGISTERS_COUNT - 3

#define FLAG_RUNNING 1
#define FLAG_EQUALS 2
#define FLAG_LESS 3
#define FLAG_MORE 4


union Register {
	uint128 ur;
	int128 r;
	uint64 ul;
	int64 l;
	uint32 ui;
	int32 i;
	uint16 us;
	int16 s;
	uint8 ub;
	int8 b;
};
