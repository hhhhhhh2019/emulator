#include <utils.h>


void log_register(uint8 id) {
	if (id % 5 == 0) LOG("r");
	if (id % 5 == 1) LOG("l");
	if (id % 5 == 2) LOG("i");
	if (id % 5 == 3) LOG("s");
	if (id % 5 == 4) LOG("b");
	LOG("%02i", id / 5);
}

void info_register(uint8 id) {
	if (id % 5 == 0) INFO("r");
	if (id % 5 == 1) INFO("l");
	if (id % 5 == 2) INFO("i");
	if (id % 5 == 3) INFO("s");
	if (id % 5 == 4) INFO("b");
	INFO("%02i", id / 5);
}
