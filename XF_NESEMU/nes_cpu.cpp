#include "nes_cpu.h"

uint8_t NesCPU::read(uint16_t addr)
{
	if (addr % 2) {
		return 0x20;
	}
	else {
		return 0xfe;
	}
}

void NesCPU::write(uint16_t addr, uint8_t val)
{

}

void NesCPU::print_status()
{
	std::cout << std::hex;
	std::cout << "A: 0x" << (int)reg_a << ", ";
	std::cout << "X: 0x" << (int)reg_x << ", ";
	std::cout << "Y: 0x" << (int)reg_y << std::endl;
	std::cout << "Program counter: 0x" << (int)reg_pc << ", ";
	std::cout << "Stack pointer: 0x" << (int)reg_sp << std::endl;
	for (int i = 0; i < 8; i++) {
		std::cout << check_sr_flag(8 - i - 1);
	}
	std::cout << std::endl << "NV-BDIZC" << std::endl;
	std::cout << "Latest registered value: 0x" << (int)working_val << ", ";
	std::cout << "Latest registered address: 0x" << (int)working_addr << std::endl;
}

void NesCPU::push_stack(uint8_t val)
{
	write(0x100 + reg_sp, val);
	reg_sp--;
}

uint8_t NesCPU::pop_stack()
{
	uint8_t val = read(0x100 + reg_sp);
	reg_sp++;
	return val;
}

uint16_t NesCPU::generate_addr(uint8_t low_byte, uint8_t high_byte)
{
	return (high_byte * 0x100) + low_byte;
}

bool NesCPU::page_crossed(uint16_t cmp_addr)
{
	return !((working_addr & 0xff00) == (cmp_addr & 0xff00));
}

uint8_t NesCPU::rotate_left(uint8_t val, int shifts)
{
	uint8_t result = val;

	for (int i = 0; i < shifts; i++) {
		result = (result << 1) | (result >> 7);
	}

	return result;
}

void NesCPU::set_sr_flag(int index, bool val)
{
	reg_sr &= rotate_left(0b11111110, index);
	reg_sr |= (val) << index;
}

bool NesCPU::check_sr_flag(int index)
{
	return reg_sr & (1 << index);
}

void NesCPU::check_negative(uint8_t val)
{
	set_sr_flag(FLG_NEGATIVE, val & 0b10000000);
}

void NesCPU::check_zero(uint8_t val)
{
	set_sr_flag(FLG_ZERO, val == 0);
}

void NesCPU::check_carry(int val)
{
	set_sr_flag(FLG_CARRY, val > 0xFF);
}

void NesCPU::addr_imm()
{
	working_val = read(reg_pc);
	reg_pc += 1;
}

void NesCPU::addr_zp(uint8_t offset = 0x00)
{
	working_addr = (uint8_t)(read(reg_pc) + offset);
	working_val = read(working_addr);
	reg_pc += 1;
}

bool NesCPU::addr_abs(uint8_t offset = 0x00)
{
	auto base_addr = generate_addr(read(reg_pc), read(reg_pc + 1));
	working_addr = base_addr + offset;
	working_val = read(working_addr);
	reg_pc += 2;
	return page_crossed(base_addr);
}

void NesCPU::addr_ind()
{
	uint8_t ptr_addr = read(reg_pc);
	working_addr = generate_addr(read(ptr_addr), read(ptr_addr + 1));
	working_val = read(working_addr);
	reg_pc += 2;
}

void NesCPU::addr_ind_x()
{
	uint8_t ptr_addr = (uint8_t)(read(reg_pc) + reg_x);
	working_addr = generate_addr(read(ptr_addr), read(ptr_addr + 1));
	working_val = read(working_addr);
	reg_pc += 1;
}

bool NesCPU::addr_ind_y()
{
	uint8_t ptr_addr = read(reg_pc);
	uint16_t base_addr = generate_addr(read(ptr_addr), read(ptr_addr + 1));
	working_addr = base_addr + reg_y;
	working_val = read(working_addr);
	reg_pc += 1;
	return page_crossed(base_addr);
}

void NesCPU::bitwise_AND()
{
	reg_a &= working_val;
	check_negative(reg_a);
	check_zero(reg_a);
}

int NesCPU::opcode_AND_imm()
{
	addr_imm();
	bitwise_AND();
	return 2;
}

int NesCPU::opcode_AND_zp()
{
	addr_zp();
	bitwise_AND();
	return 3;
}

int NesCPU::opcode_AND_zp_x()
{
	addr_zp(reg_x);
	bitwise_AND();
	return 4;
}

int NesCPU::opcode_AND_abs()
{
	addr_abs();
	bitwise_AND();
	return 4;
}

int NesCPU::opcode_AND_abs_x()
{
	auto page_crossed = addr_abs(reg_x);
	bitwise_AND();
	return 4 + page_crossed;
}

int NesCPU::opcode_AND_abs_y()
{
	auto page_crossed = addr_abs(reg_y);
	bitwise_AND();
	return 4 + page_crossed;
}

int NesCPU::opcode_AND_ind_x()
{
	addr_ind_x();
	bitwise_AND();
	return 6;
}

int NesCPU::opcode_AND_ind_y()
{
	auto page_crossed = addr_ind_y();
	bitwise_AND();
	return 5 + page_crossed;
}

void NesCPU::bitwise_shift_left()
{
	set_sr_flag(FLG_CARRY, working_val & 0b10000000);
	working_val <<= 1;
	check_negative(working_val);
	check_zero(working_val);
}

int NesCPU::opcode_ASL_a()
{
	reg_a = working_val;
	bitwise_shift_left();
	return 2;
}

int NesCPU::opcode_ASL_zp()
{
	addr_zp();
	bitwise_shift_left();
	write(working_addr, working_val);
	return 5;
}

int NesCPU::opcode_ASL_zp_x()
{
	addr_zp(reg_x);
	bitwise_shift_left();
	write(working_addr, working_val);
	return 6;
}

int NesCPU::opcode_ASL_abs()
{
	addr_abs(reg_x);
	bitwise_shift_left();
	write(working_addr, working_val);
	return 6;
}

int NesCPU::opcode_ASL_abs_x()
{
	addr_abs(reg_x);
	bitwise_shift_left();
	write(working_addr, working_val);
	return 7;
}

void NesCPU::test_bits()
{
	check_zero(working_val);
	set_sr_flag(FLG_NEGATIVE, working_val & 0b10000000);
	set_sr_flag(FLG_OVERFLOW, working_val & 0b01000000);
}

int NesCPU::opcode_BIT_zp()
{
	addr_zp();
	test_bits();
	return 3;
}

int NesCPU::opcode_BIT_abs()
{
	addr_abs();
	test_bits();
	return 4;
}

int NesCPU::signed_byte(uint8_t val)
{
	if (val & 0b10000000) {
		return -1 * (val ^ 0xFF);
	}
	else {
		return val;
	}
}

int NesCPU::branch_if(bool condition)
{
	auto base_addr = reg_pc;
	addr_imm();
	if (condition) {
		reg_pc = (int)base_addr + signed_byte(working_val);
		return 3 + page_crossed(base_addr);
	}
	else {
		return 2;
	}
}

int NesCPU::opcode_BPL()
{
	return branch_if(check_sr_flag(!FLG_NEGATIVE));
}

int NesCPU::opcode_BMI()
{
	return branch_if(check_sr_flag(FLG_NEGATIVE));
}

int NesCPU::opcode_BVC()
{
	return branch_if(check_sr_flag(!FLG_OVERFLOW));
}

int NesCPU::opcode_BVS()
{
	return branch_if(check_sr_flag(FLG_OVERFLOW));
}

int NesCPU::opcode_BCC()
{
	return branch_if(check_sr_flag(!FLG_CARRY));
}

int NesCPU::opcode_BCS()
{
	return branch_if(check_sr_flag(FLG_CARRY));
}

int NesCPU::opcode_BNE()
{
	return branch_if(check_sr_flag(!FLG_ZERO));
}

int NesCPU::opcode_BEQ()
{
	return branch_if(check_sr_flag(FLG_ZERO));
}

void NesCPU::compare_value(uint8_t val1, uint8_t val2)
{
	check_negative(val1);
	if (val1 == val2) {
		set_sr_flag(FLG_ZERO, true);
	}
	if (val1 >= val2) {
		set_sr_flag(FLG_CARRY, true);
	}
}

int NesCPU::opcode_CMP_imm()
{
	addr_imm();
	compare_value(reg_a, working_val);
	return 2;
}

int NesCPU::opcode_CMP_zp()
{
	addr_zp();
	compare_value(reg_a, working_val);
	return 3;
}

int NesCPU::opcode_CMP_zp_x()
{
	addr_zp(reg_x);
	compare_value(reg_a, working_val);
	return 4;
}

int NesCPU::opcode_CMP_abs()
{
	addr_abs();
	compare_value(reg_a, working_val);
	return 4;
}

int NesCPU::opcode_CMP_abs_x()
{
	auto page_crossed = addr_abs(reg_x);
	compare_value(reg_a, working_val);
	return 4 + page_crossed;
}

int NesCPU::opcode_CMP_abs_y()
{
	auto page_crossed = addr_abs(reg_y);
	compare_value(reg_a, working_val);
	return 4 + page_crossed;
}

int NesCPU::opcode_CMP_ind_x()
{
	addr_ind_x();
	compare_value(reg_a, working_val);
	return 6;
}

int NesCPU::opcode_CMP_ind_y()
{
	auto page_crossed = addr_ind_y();
	compare_value(reg_a, working_val);
	return 5 + page_crossed;
}

int NesCPU::opcode_CPX_imm()
{
	addr_imm();
	compare_value(reg_x, working_val);
	return 2;
}

int NesCPU::opcode_CPX_zp()
{
	addr_zp();
	compare_value(reg_x, working_val);
	return 3;
}

int NesCPU::opcode_CPX_abs()
{
	addr_abs();
	compare_value(reg_x, working_val);
	return 4;
}

int NesCPU::opcode_CPY_imm()
{
	addr_imm();
	compare_value(reg_y, working_val);
	return 2;
}

int NesCPU::opcode_CPY_zp()
{
	addr_zp();
	compare_value(reg_y, working_val);
	return 3;
}

int NesCPU::opcode_CPY_abs()
{
	addr_abs();
	compare_value(reg_y, working_val);
	return 4;
}

void NesCPU::decrement_addr()
{
	write(working_addr, read(working_addr) - 1);
}

int NesCPU::opcode_DEC_zp()
{
	addr_zp();
	decrement_addr();
	return 5;
}

int NesCPU::opcode_DEC_zp_x()
{
	addr_zp(reg_x);
	decrement_addr();
	return 6;
}

int NesCPU::opcode_DEC_abs()
{
	addr_abs();
	decrement_addr();
	return 6;
}

int NesCPU::opcode_DEC_abs_x()
{
	addr_abs(reg_x);
	decrement_addr();
	return 7;
}

void NesCPU::exclusive_or()
{
	reg_a ^= working_val;
	check_negative(reg_a);
	check_zero(reg_a);
}

int NesCPU::opcode_EOR_imm()
{
	addr_imm();
	exclusive_or();
	return 2;
}

int NesCPU::opcode_EOR_zp()
{
	addr_zp();
	exclusive_or();
	return 3;
}

int NesCPU::opcode_EOR_zp_x()
{
	addr_zp(reg_x);
	exclusive_or();
	return 4;
}

int NesCPU::opcode_EOR_abs()
{
	addr_abs();
	exclusive_or();
	return 4;
}

int NesCPU::opcode_EOR_abs_x()
{
	auto page_crossed = addr_abs(reg_x);
	exclusive_or();
	return 4 + page_crossed;
}

int NesCPU::opcode_EOR_abs_y()
{
	auto page_crossed = addr_abs(reg_y);
	exclusive_or();
	return 4 + page_crossed;
}

int NesCPU::opcode_EOR_ind_x()
{
	addr_ind_x();
	exclusive_or();
	return 6;
}

int NesCPU::opcode_EOR_ind_y()
{
	auto page_crossed = addr_ind_y();
	exclusive_or();
	return 5 + page_crossed;
}

int NesCPU::opcode_CLC()
{
	set_sr_flag(FLG_CARRY, false);
	return 2;
}

int NesCPU::opcode_SEC()
{
	set_sr_flag(FLG_CARRY, true);
	return 2;
}

int NesCPU::opcode_CLI()
{
	set_sr_flag(FLG_INTERRUPTS, false);
	return 2;
}

int NesCPU::opcode_SEI()
{
	set_sr_flag(FLG_INTERRUPTS, true);
	return 2;
}

int NesCPU::opcode_CLV()
{
	set_sr_flag(FLG_OVERFLOW, false);
	return 2;
}

int NesCPU::opcode_CLD()
{
	set_sr_flag(FLG_DECIMAL, false);
	return 2;
}

int NesCPU::opcode_SED()
{
	set_sr_flag(FLG_DECIMAL, true);
	return 2;
}

void NesCPU::increment_addr()
{
	write(working_addr, read(working_addr) + 1);
}

int NesCPU::opcode_INC_zp()
{
	addr_zp();
	increment_addr();
	return 5;
}

int NesCPU::opcode_INC_zp_x()
{
	addr_zp(reg_x);
	increment_addr();
	return 6;
}

int NesCPU::opcode_INC_abs()
{
	addr_abs();
	increment_addr();
	return 6;
}

int NesCPU::opcode_INC_abs_x()
{
	addr_abs(reg_x);
	increment_addr();
	return 7;
}

int NesCPU::opcode_JMP_abs()
{
	addr_abs();
	reg_pc = working_addr;
	return 3;
}

int NesCPU::opcode_JMP_ind()
{
	addr_ind();
	reg_pc = working_addr;
	return 5;
}

int NesCPU::opcode_JSR()
{
	addr_abs();
	reg_pc = working_addr;
	uint8_t low_byte = (uint8_t)reg_pc;
	uint8_t high_byte = (uint8_t)reg_pc << 8;
	push_stack(low_byte);
	push_stack(high_byte);
	return 6;
}

int NesCPU::opcode_LDA_imm()
{
	addr_imm();
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 2;
}

int NesCPU::opcode_LDA_zp()
{
	addr_zp();
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 3;
}

int NesCPU::opcode_LDA_zp_x()
{
	addr_zp(reg_x);
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 4;
}

int NesCPU::opcode_LDA_abs()
{
	addr_abs();
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 4;
}

int NesCPU::opcode_LDA_abs_x()
{
	auto page_crossed = addr_abs(reg_x);
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 4 + page_crossed;
}

int NesCPU::opcode_LDA_abs_y()
{
	auto page_crossed = addr_abs(reg_y);
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 5 + page_crossed;
}

int NesCPU::opcode_LDA_ind_x()
{
	addr_ind_x();
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 6;
}

int NesCPU::opcode_LDA_ind_y()
{
	auto page_crossed = addr_ind_y();
	reg_a = working_val;
	check_negative(reg_a);
	check_zero(reg_a);
	return 5 + page_crossed;
}

int NesCPU::opcode_LDX_imm()
{
	addr_imm();
	reg_x = working_val;
	check_negative(reg_x);
	check_zero(reg_x);
	return 2;
}

int NesCPU::opcode_LDX_zp()
{
	addr_zp();
	reg_x = working_val;
	check_negative(reg_x);
	check_zero(reg_x);
	return 3;
}

int NesCPU::opcode_LDX_zp_y()
{
	addr_zp(reg_y);
	reg_x = working_val;
	check_negative(reg_x);
	check_zero(reg_x);
	return 4;
}

int NesCPU::opcode_LDX_abs()
{
	addr_abs();
	reg_x = working_val;
	check_negative(reg_x);
	check_zero(reg_x);
	return 4;
}
int NesCPU::opcode_LDX_abs_y()
{
	auto page_crossed = addr_abs(reg_y);
	reg_x = working_val;
	check_negative(reg_x);
	check_zero(reg_x);
	return 4 + page_crossed;
}

int NesCPU::opcode_LDY_imm()
{
	addr_imm();
	reg_y = working_val;
	check_negative(reg_y);
	check_zero(reg_y);
	return 2;
}

int NesCPU::opcode_LDY_zp()
{
	addr_zp();
	reg_y = working_val;
	check_negative(reg_y);
	check_zero(reg_y);
	return 3;
}

int NesCPU::opcode_LDY_zp_x()
{
	addr_zp(reg_x);
	reg_y = working_val;
	check_negative(reg_y);
	check_zero(reg_y);
	return 4;
}

int NesCPU::opcode_LDY_abs()
{
	addr_abs();
	reg_y = working_val;
	check_negative(reg_y);
	check_zero(reg_y);
	return 4;
}
int NesCPU::opcode_LDY_abs_x()
{
	auto page_crossed = addr_abs(reg_x);
	reg_y = working_val;
	check_negative(reg_y);
	check_zero(reg_y);
	return 4 + page_crossed;
}

void NesCPU::bitwise_shift_right()
{
	set_sr_flag(FLG_CARRY, working_val & 0b00000001);
	working_val >>= 1;
	check_negative(working_val);
	check_zero(working_val);
}

int NesCPU::opcode_LSR_a()
{
	reg_a = working_val;
	bitwise_shift_right();
	return 2;
}

int NesCPU::opcode_LSR_zp()
{
	addr_zp();
	bitwise_shift_right();
	write(working_addr, working_val);
	return 5;
}

int NesCPU::opcode_LSR_zp_x()
{
	addr_zp(reg_x);
	bitwise_shift_right();
	write(working_addr, working_val);
	return 6;
}

int NesCPU::opcode_LSR_abs()
{
	addr_abs(reg_x);
	bitwise_shift_right();
	write(working_addr, working_val);
	return 6;
}

int NesCPU::opcode_LSR_abs_x()
{
	addr_abs(reg_x);
	bitwise_shift_right();
	write(working_addr, working_val);
	return 7;
}

int NesCPU::opcode_NOP()
{
	return 2;
}

void NesCPU::bitwise_or()
{
	reg_a |= working_val;
	check_negative(reg_a);
	check_zero(reg_a);
}

int NesCPU::opcode_ORA_imm()
{
	addr_imm();
	bitwise_or();
	return 2;
}

int NesCPU::opcode_ORA_zp()
{
	addr_zp();
	bitwise_or();
	return 3;
}

int NesCPU::opcode_ORA_zp_x()
{
	addr_zp(reg_x);
	bitwise_or();
	return 4;
}

int NesCPU::opcode_ORA_abs()
{
	addr_abs();
	bitwise_or();
	return 4;
}

int NesCPU::opcode_ORA_abs_x()
{
	auto page_crossed = addr_abs(reg_x);
	bitwise_or();
	return 4 + page_crossed;
}

int NesCPU::opcode_ORA_abs_y()
{
	auto page_crossed = addr_abs(reg_y);
	bitwise_or();
	return 4 + page_crossed;
}

int NesCPU::opcode_ORA_ind_x()
{
	addr_ind_x();
	bitwise_or();
	return 6;
}

int NesCPU::opcode_ORA_ind_y()
{
	auto page_crossed = addr_ind_y();
	bitwise_or();
	return 5 + page_crossed;
}

void NesCPU::reg_transfer(uint8_t reg)
{
	check_negative(reg);
	check_zero(reg);
}

int NesCPU::opcode_TAX()
{
	reg_x = reg_a;
	reg_transfer(reg_x);
	return 2;
}

int NesCPU::opcode_TXA()
{
	reg_a = reg_x;
	reg_transfer(reg_a);
	return 2;
}

int NesCPU::opcode_DEX()
{
	reg_x--;
	reg_transfer(reg_x);
	return 2;
}

int NesCPU::opcode_INX()
{
	reg_x++;
	reg_transfer(reg_x);
	return 2;
}

int NesCPU::opcode_TAY()
{
	reg_y = reg_a;
	reg_transfer(reg_y);
	return 2;
}

int NesCPU::opcode_TYA()
{
	reg_a = reg_y;
	reg_transfer(reg_a);
	return 2;
}

int NesCPU::opcode_DEY()
{
	reg_y--;
	reg_transfer(reg_y);
	return 2;
}

int NesCPU::opcode_INY()
{
	reg_y++;
	reg_transfer(reg_y);
	return 2;
}
