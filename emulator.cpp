#include <stdio.h>
#include <string>
#include <vector>
#include <getopt.h>
#include <core.cpp>
#include <registers.cpp>


#ifndef DEBUG
	#define LOG
#else
	#define LOG(...) printf(__VA_ARGS__)
#endif


#define BIOS_OFFSET 17044482
#define PROC_TABLE_OFFSET 1024


void print_help();


unsigned char *ram;
unsigned long ram_size = 128 * 1024 * 1024;
unsigned char cores_count = 4;
std::string bios_filename("std_bios.bin");

Core *cores;


static struct option long_opt[] = {
	{"help", 0,0, 'h'},
	{"bios", 1,0, 'b'},
	{"ram" , 1,0, 'm'},
	{"hd0" , 1,0, 1}
};



int main(int argc, char **argv) {
	if (argc == 1) {
		print_help();
		return 0;
	}

	int opt;
	int opt_id;

	while ((opt = getopt_long(argc, argv, "hb:m:", long_opt, &opt_id)) != -1) {
		switch(opt) {
			case 'h':
				print_help();
				return 0;

			case 'm':
				ram_size = atoi(optarg) * 1024 * 1024;
				break;

			case 'c':
				cores_count = atoi(optarg);
				break;

			case 'b':
				bios_filename = optarg;
				break;

			case 1:
				printf("disk\n");
				break;

			default:
				print_help();
				return 0;
		}
	}


	LOG("Инициализация ОЗУ: %lu Кол-во ядер: %i\n", ram_size, cores_count);
	ram = new unsigned char[ram_size];
	cores = new Core[cores_count];


	FILE* bios_file = fopen(bios_filename.c_str(), "rb");

	if (bios_file == NULL) {
		printf("Файла %s не существует!\n", bios_filename.c_str());
		return 2;
	}

	fseek(bios_file, 0, SEEK_END);
	long bios_size = ftell(bios_file);
	rewind(bios_file);

	unsigned char *bios = new unsigned char[bios_size];

	fread(bios, 1, bios_size, bios_file);

	fclose(bios_file);


	LOG("Загрузка BIOS в ОЗУ\n");

	for (int i = 0; i < bios_size; i++) {
		ram[i + BIOS_OFFSET] = bios[i];
	}

	for (int i = 0; i < cores_count; i++)
		cores[i].clear_registers();

	cores[0].regs[pc].ul = BIOS_OFFSET;
	cores[0].regs[sp].ul = BIOS_OFFSET - 1;
	cores[0].set_flag_state(FLAG::running, 1);

	while (1) {
		for (int i = 0; i < cores_count; i++)
			cores[i].step();

#ifdef DEBUG
		fgetc(stdin);
#endif

		char enable_count = 0;

		for (int i = 0; i < cores_count; i++) {
			enable_count += cores[i].get_flag_state(FLAG::running);
		}

		if (enable_count == 0) break;
	}

	printf("Все ядра остановлены. Выход.\n");

	return 0;
}


void print_help() {
	printf("Эмулятор процессора с архитектурой ???.\n\t-m <int>\n\t\tКол-во опертивной памяти в мегабайтах. Если не указанно = 1M.\n\n\t-c <int>\n\t\tКол-во ядер. Если не указанно = 1. Пока не используется.\n\n\t-b <file path>\n\t\tФайл BIOS. Первое, что запускает процессор.\n\n\t-hd0 <file path>\n\t\tФайл - виртуальный диск. Только img формат!\n");
}
