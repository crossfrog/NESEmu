#pragma once
#include <iostream>

class NesCPU {
public:
	const int FLG_CARRY = 0;
	const int FLG_ZERO = 1;
	const int FLG_INTERRUPTS = 2;
	const int FLG_DECIMAL = 3;
	const int FLG_BREAKPOINT = 4;
	const int FLG_UNUSED = 5;
	const int FLG_OVERFLOW = 6;
	const int FLG_NEGATIVE = 7;

	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t val);

	void print_status();

	uint8_t reg_a;
	uint8_t reg_x;
	uint8_t reg_y;

	uint8_t reg_sp = 0xFF;
	uint16_t reg_pc;

	uint8_t reg_sr;

	uint8_t working_val;
	uint16_t working_addr;

	void push_stack(uint8_t val);
	uint8_t pop_stack();

	uint16_t generate_addr(uint8_t low_byte, uint8_t high_byte);
	bool page_crossed(uint16_t cmp_addr);

	uint8_t rotate_left(uint8_t val, int shifts);

	void set_sr_flag(int index, bool val);
	bool check_sr_flag(int index);

	void check_negative(uint8_t val);
	void check_zero(uint8_t val);
	void check_carry(int val);

	void addr_imm();
	void addr_zp(uint8_t offset);
	bool addr_abs(uint8_t offset);
	void addr_ind();
	void addr_ind_x();
	bool addr_ind_y();

	void bitwise_AND();
	int opcode_AND_imm();
	int opcode_AND_zp();
	int opcode_AND_zp_x();
	int opcode_AND_abs();
	int opcode_AND_abs_x();
	int opcode_AND_abs_y();
	int opcode_AND_ind_x();
	int opcode_AND_ind_y();

	void bitwise_shift_left();
	int opcode_ASL_a();
	int opcode_ASL_zp();
	int opcode_ASL_zp_x();
	int opcode_ASL_abs();
	int opcode_ASL_abs_x();

	void test_bits();
	int opcode_BIT_zp();
	int opcode_BIT_abs();

	int signed_byte(uint8_t val);
	int branch_if(bool condition);
	int opcode_BPL();
	int opcode_BMI();
	int opcode_BVC();
	int opcode_BVS();
	int opcode_BCC();
	int opcode_BCS();
	int opcode_BNE();
	int opcode_BEQ();

	void compare_value(uint8_t val1, uint8_t val2);
	int opcode_CMP_imm();
	int opcode_CMP_zp();
	int opcode_CMP_zp_x();
	int opcode_CMP_abs();
	int opcode_CMP_abs_x();
	int opcode_CMP_abs_y();
	int opcode_CMP_ind_x();
	int opcode_CMP_ind_y();

	int opcode_CPX_imm();
	int opcode_CPX_zp();
	int opcode_CPX_abs();

	int opcode_CPY_imm();
	int opcode_CPY_zp();
	int opcode_CPY_abs();

	void decrement_addr();
	int opcode_DEC_zp();
	int opcode_DEC_zp_x();
	int opcode_DEC_abs();
	int opcode_DEC_abs_x();

	void exclusive_or();
	int opcode_EOR_imm();
	int opcode_EOR_zp();
	int opcode_EOR_zp_x();
	int opcode_EOR_abs();
	int opcode_EOR_abs_x();
	int opcode_EOR_abs_y();
	int opcode_EOR_ind_x();
	int opcode_EOR_ind_y();

	int opcode_CLC();
	int opcode_SEC();
	int opcode_CLI();
	int opcode_SEI();
	int opcode_CLV();
	int opcode_CLD();
	int opcode_SED();

	void increment_addr();
	int opcode_INC_zp();
	int opcode_INC_zp_x();
	int opcode_INC_abs();
	int opcode_INC_abs_x();

	int opcode_JMP_abs();
	int opcode_JMP_ind();

	int opcode_JSR();

	int opcode_LDA_imm();
	int opcode_LDA_zp();
	int opcode_LDA_zp_x();
	int opcode_LDA_abs();
	int opcode_LDA_abs_x();
	int opcode_LDA_abs_y();
	int opcode_LDA_ind_x();
	int opcode_LDA_ind_y();

	int opcode_LDX_imm();
	int opcode_LDX_zp();
	int opcode_LDX_zp_y();
	int opcode_LDX_abs();
	int opcode_LDX_abs_y();

	int opcode_LDY_imm();
	int opcode_LDY_zp();
	int opcode_LDY_zp_x();
	int opcode_LDY_abs();
	int opcode_LDY_abs_x();

	void bitwise_shift_right();
	int opcode_LSR_a();
	int opcode_LSR_zp();
	int opcode_LSR_zp_x();
	int opcode_LSR_abs();
	int opcode_LSR_abs_x();

	int opcode_NOP();

	void bitwise_or();
	int opcode_ORA_imm();
	int opcode_ORA_zp();
	int opcode_ORA_zp_x();
	int opcode_ORA_abs();
	int opcode_ORA_abs_x();
	int opcode_ORA_abs_y();
	int opcode_ORA_ind_x();
	int opcode_ORA_ind_y();

	void reg_transfer(uint8_t reg);
	int opcode_TAX();
	int opcode_TXA();
	int opcode_DEX();
	int opcode_INX();
	int opcode_TAY();
	int opcode_TYA();
	int opcode_DEY();
	int opcode_INY();
};