#include <core.h>
#include <utils.h>
#include <register.h>

#include <fstream>


uint8 *ram;
Core *cores;

uint8 *bios;

char *bios_name = (char*)"std_bios.bin";
uint64 ram_size = 1024 * 1024 * 1; // 1 M
uint8 cores_count = 1;



int main() {
	ram = new uint8[ram_size];
	cores = new Core[cores_count];

	for (int i = 0; i < cores_count; i++) {
		cores[i].init(i);
	}


	INFO("reading BIOS\n");

	FILE *f = fopen(bios_name, "rb");

	if (f == NULL) {
		printf("File %s not found!\n", bios_name);
		return 2;
	}

	fseek(f, 0L, SEEK_END);
	uint32 bios_size = ftell(f);
	rewind(f);

	bios = new uint8[bios_size];

	fread(bios, 1, bios_size, f);

	fclose(f);


	INFO("writing BIOS\n");

	for (int i = 0; i < bios_size; i++) {
		ram[i + BIOS_OFFSET] = bios[i];
	}


	INFO("init core 0\n");

	cores[0].set_flag(FLAG_RUNNING, 1);
	cores[0].regs[REG_PC].ul = BIOS_OFFSET;
	cores[0].regs[REG_SP].ul = BIOS_OFFSET;

	while (cores[0].get_flag(FLAG_RUNNING)) {
		cores[0].step();

		getc(stdin);
	}

	INFO("all cores stoped. exit\n");

	return 0;
}
