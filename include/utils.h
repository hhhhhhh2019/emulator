#pragma once

#include <cstdio>


#ifndef DEBUG
	#define LOG
#else
	#define LOG(...) printf(__VA_ARGS__)
#endif

#define INFO(...) LOG("\e[1;34m%-6s\e[m", __VA_ARGS__)


typedef unsigned char     uint8;
typedef char               int8;
typedef unsigned short    uint16;
typedef short              int16;
typedef unsigned int      uint32;
typedef int                int32;
typedef unsigned long     uint64;
typedef long               int64;
typedef unsigned __int128 uint128;
typedef __int128           int128;


#define BIOS_OFFSET 1024


void log_register(uint8);
