#include <iostream>
#include "nes_cpu.h"
#include <iomanip>

int main() {
	NesCPU cpu = NesCPU();
	cpu.opcode_LDA_ind_y();
	cpu.print_status();
	return 0;
}