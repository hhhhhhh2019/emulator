#pragma once

//extern unsigned char ram;
extern Core *cores;
extern unsigned char cores_count;

struct ProcCtrl {
	char enable = 0;
	uint16 tickOnProccess = 10;

	uint16 *procID;

	ProcCtrl() {
		procID = new uint16[cores_count];
	}

	void step() {
		if (ram[3] == 1) {
			uint8 cmd = ram[0];
			uint16 param =
				((uint16)ram[1] << 0) |
				((uint16)ram[2] << 8);

			if (cmd == 0) { // включить
				enable = 1;
			}

			if (cmd == 1) { // тактов на 1 процесс
				tickOnProccess = param;
			}

			if (cmd == 2) { // загрузить прошивку из ОЗУ (в будующем)

			}

			ram[0] = 0;
			ram[1] = 0;
			ram[2] = 0;
			ram[3] = 0;
		}

		if (enable == 0) return;

		char enable_cores = 0;

		for (int i = 0; i < cores_count; i++)
			cores_count += cores[i].get_flag_state(FLAG::running);

		uint16 proc_count = ram[0];

		for (int i = 0; i < cores_count; i++) {
			if (cores[i].get_flag_state(FLAG::running) == 0)
				continue;

			if (cores[i].counter % tickOnProccess != 0) continue;


		}
	}
};
