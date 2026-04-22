#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

class bit_tests : public testing::Test {
public:
    Memory mem;
    CPU cpu;

    //tip: do not use these in all lowercase.
    virtual void SetUp() override {
        cpu.reset(0xFF00, mem);
    }

    virtual void TearDown() override {

    }
};

TEST_F( bit_tests, TestBitZeroPage ) {
	cpu.flag.V = cpu.flag.N = false;
	cpu.A = 0xCC;
	mem[0xFF00] = CPU::INS_BIT_ZP;	
	mem[0xFF01] = 0x42;
	mem[0x0042] = 0xCC;
	constexpr s32 EXPECTED_CYCLES = 3;

	//when:
	s32 CyclesUsed = cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0xCC );
	EXPECT_FALSE( cpu.flag.Z );
	EXPECT_TRUE( cpu.flag.V );
	EXPECT_TRUE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitZeroPageResultZero ) {
	cpu.flag.V = cpu.flag.N = true;
	cpu.A = 0xCC;
	mem[0xFF00] = CPU::INS_BIT_ZP;
	mem[0xFF01] = 0x42;
	mem[0x0042] = 0x33;
	constexpr s32 EXPECTED_CYCLES = 3;

	//when:
	s32 CyclesUsed = cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0xCC );
	EXPECT_TRUE( cpu.flag.Z );
	EXPECT_FALSE( cpu.flag.V );
	EXPECT_FALSE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitZeroPageResultZeroBits6And7Zero ) {
	cpu.flag.V = cpu.flag.N = false;
	cpu.A = 0x33;
	mem[0xFF00] = CPU::INS_BIT_ZP;
	mem[0xFF01] = 0x42;
	mem[0x0042] = 0xCC;
	constexpr s32 EXPECTED_CYCLES = 3;

	//when:
	s32 CyclesUsed = cpu.execute(EXPECTED_CYCLES, mem);

	//then:
	EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0x33 );
	EXPECT_TRUE( cpu.flag.Z );
	EXPECT_TRUE( cpu.flag.V );
	EXPECT_TRUE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitZeroPageResultZeroBits6And7Mixed) {
	cpu.flag.V = false;
	cpu.flag.N = true;
	mem[0xFF00] = CPU::INS_BIT_ZP;
	mem[0xFF01] = 0x42;
	mem[0x0042] = 0b01000000;
	constexpr s32 EXPECTED_CYCLES = 3;

	//when:
	cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_TRUE( cpu.flag.V );
	EXPECT_FALSE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitAbsolute ) {
	cpu.flag.V = cpu.flag.N = false;
	cpu.A = 0xCC;
	mem[0xFF00] = CPU::INS_BIT_ABS;
	mem[0xFF01] = 0x00;
	mem[0xFF02] = 0x80;
	mem[0x8000] = 0xCC;
	constexpr s32 EXPECTED_CYCLES = 4;

	//when:
	s32 CyclesUsed = cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0xCC );
	EXPECT_FALSE( cpu.flag.Z );
	EXPECT_TRUE( cpu.flag.V );
	EXPECT_TRUE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitAbsoluteResultZero ) {
	cpu.flag.V = cpu.flag.N = true;
	cpu.A = 0xCC;
	mem[0xFF00] = CPU::INS_BIT_ABS;
	mem[0xFF01] = 0x00;
	mem[0xFF02] = 0x80;
	mem[0x8000] = 0x33;
	constexpr s32 EXPECTED_CYCLES = 4;

	//when:
	s32 CyclesUsed = cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0xCC );
	EXPECT_TRUE( cpu.flag.Z );
	EXPECT_FALSE( cpu.flag.V );
	EXPECT_FALSE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitAbsoluteResultZeroBit6And7Zero ) {
	cpu.flag.V = cpu.flag.N = false;
	cpu.A = 0x33;
	mem[0xFF00] = CPU::INS_BIT_ABS;
	mem[0xFF01] = 0x00;
	mem[0xFF02] = 0x80;
	mem[0x8000] = 0xCC;
	constexpr s32 EXPECTED_CYCLES = 4;

	//when:
	s32 CyclesUsed = cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0x33 );
	EXPECT_TRUE( cpu.flag.Z );
	EXPECT_TRUE( cpu.flag.V );
	EXPECT_TRUE( cpu.flag.N );
}

TEST_F( bit_tests, TestBitAbsoluteResultZeroBit6And7Mixed ) {
	cpu.flag.V = true;
	cpu.flag.N = false;
	mem[0xFF00] = CPU::INS_BIT_ABS;
	mem[0xFF01] = 0x00;
	mem[0xFF02] = 0x80;
	mem[0x8000] = 0b10000000;
	constexpr s32 EXPECTED_CYCLES = 4;

	//when:
	cpu.execute( EXPECTED_CYCLES, mem );

	//then:
	EXPECT_FALSE( cpu.flag.V );
	EXPECT_TRUE( cpu.flag.N );
}
