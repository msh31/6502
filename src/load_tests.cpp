#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

class load_tests : public testing::Test {
public:
    Memory mem;
    CPU cpu;

    //tip: do not use these in all lowercase.
    virtual void SetUp() override {
        cpu.reset(mem);
    }

    virtual void TearDown() override {

    }

    void test_load_register_immediate(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_zero_page(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_zero_page_x(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_zero_page_y(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_x(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_y(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_x_crossing(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_y_crossing(byte opcode, byte emulator::CPU::*reg);
};

static void verify_unmodified_flags_from_load_register(const CPU& cpu, const CPU& cpu_copy) {
    EXPECT_EQ(cpu.C, cpu_copy.C);
    EXPECT_EQ(cpu.I, cpu_copy.I);
    EXPECT_EQ(cpu.D, cpu_copy.D);
    EXPECT_EQ(cpu.B, cpu_copy.B);
    EXPECT_EQ(cpu.V, cpu_copy.V);
}

TEST_F(load_tests, DoNothingWhenExecutingZeroCycles) {
    constexpr s32 NUM_CYCLES = 0;

    s32 cycles_used = cpu.execute(NUM_CYCLES, mem);

    EXPECT_EQ(cycles_used, 0);
}

TEST_F(load_tests, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction) {
    //given
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    constexpr s32 NUM_CYCLES = 1;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(NUM_CYCLES, mem); //explicitly set to 1 for testing!

    //then
    EXPECT_EQ(cycles_used, 2);
}

void load_tests::test_load_register_immediate(byte opcode, byte CPU::*reg) {
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x84;

    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(2, mem);

    EXPECT_EQ(cpu.*reg, 0x84);
    EXPECT_EQ(cycles_used, 2);

    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_zero_page(byte opcode, byte emulator::CPU::*reg) {
    //given
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(3, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, 3);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_zero_page_x(byte opcode, byte emulator::CPU::*reg) {
    //given
    cpu.X = 5;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(4, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, 4);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_zero_page_y(byte opcode, byte emulator::CPU::*reg) {
    //given
    cpu.Y = 5;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(4, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, 4);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_absolute(byte opcode, byte emulator::CPU::*reg) {
    cpu.Z = cpu.N = true;

    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; //0x4480
    mem[0x4480] = 0x37;

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_absolute_x(byte opcode, byte emulator::CPU::*reg) {
    cpu.Z = cpu.N = true;

    cpu.X = 1;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; //0x4480
    mem[0x4481] = 0x37;

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_absolute_y(byte opcode, byte emulator::CPU::*reg) {
    cpu.Z = cpu.N = true;

    cpu.Y = 1;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; //0x4480
    mem[0x4481] = 0x37;

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_absolute_x_crossing(byte opcode, byte emulator::CPU::*reg) {
    cpu.X = 0xFF;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; //0x4402
    mem[0x4501] = 0x37; //0x4402+0xFF crosses page boundary

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 5;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

void load_tests::test_load_register_absolute_y_crossing(byte opcode, byte emulator::CPU::*reg) {
    cpu.Y = 0xFF;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; //0x4402
    mem[0x4501] = 0x37; //0x4402+0xFF crosses page boundary

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 5;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.*reg, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

//actual testing

TEST_F(load_tests, LDAImmediateCanLoadValueIntoRegisterA) {
    test_load_register_immediate(CPU::INS_LDA_IM, &CPU::A);
}
TEST_F(load_tests, LDXImmediateCanLoadValueIntoRegisterX) {
    test_load_register_immediate(CPU::INS_LDX_IM, &CPU::X);
}
TEST_F(load_tests, LDYImmediateCanLoadValueIntoRegisterY) {
    test_load_register_immediate(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(load_tests, LDAImmediateCanAffectZeroFlag) {
    //given
    cpu.A = 0x44;
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x0;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(2, mem);

    //then
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

TEST_F(load_tests, LDAZeroPageCanLoadValueIntoRegisterA) {
    test_load_register_zero_page(CPU::INS_LDA_ZP, &emulator::CPU::A);
}
TEST_F(load_tests, LDXZeroPageCanLoadValueIntoRegisterX) {
    test_load_register_zero_page(CPU::INS_LDX_ZP, &emulator::CPU::X);
}
TEST_F(load_tests, LDYZeroPageCanLoadValueIntoRegisterY) {
    test_load_register_zero_page(CPU::INS_LDY_ZP, &emulator::CPU::Y);
}

TEST_F(load_tests, LDAZeroPageXCanLoadValueIntoRegisterA) {
    test_load_register_zero_page_x(CPU::INS_LDA_ZPX, &emulator::CPU::A);
}
TEST_F(load_tests, LDXZeroPageYCanLoadValueIntoRegisterX) {
    test_load_register_zero_page_y(CPU::INS_LDX_ZPY, &emulator::CPU::X);
}
TEST_F(load_tests, LDYZeroPageXCanLoadValueIntoRegisterY) {
    test_load_register_zero_page_x(CPU::INS_LDY_ZPX, &emulator::CPU::Y);
}

TEST_F(load_tests, LDAZeroPageXCanLoadValueIntoRegisterAWhenItWraps) {
    //given
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(4, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycles_used, 4);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

TEST_F(load_tests, LDAAbsoluteCanLoadValueIntoRegisterA) {
    test_load_register_absolute(CPU::INS_LDA_ABS, &emulator::CPU::A);
}
TEST_F(load_tests, LDXAbsoluteCanLoadValueIntoRegisterX) {
    test_load_register_absolute(CPU::INS_LDX_ABS, &emulator::CPU::X);
}
TEST_F(load_tests, LDYAbsoluteCanLoadValueIntoRegisterY) {
    test_load_register_absolute(CPU::INS_LDY_ABS, &emulator::CPU::Y);
}

TEST_F(load_tests, LDAAbsoluteXCanLoadValueIntoRegisterA) {
    test_load_register_absolute_x(CPU::INS_LDA_ABSX, &emulator::CPU::A);
}
TEST_F(load_tests, LDXAbsoluteXCanLoadValueIntoRegisterX) {
    test_load_register_absolute_y(CPU::INS_LDX_ABSY, &emulator::CPU::X);
}
TEST_F(load_tests, LDYAbsoluteXCanLoadValueIntoRegisterY) {
    test_load_register_absolute_x(CPU::INS_LDY_ABSX, &emulator::CPU::Y);
}

TEST_F(load_tests, LDAAbsoluteXCanLoadValueIntoRegisterAWhenBoundsCrossing) {
    test_load_register_absolute_x_crossing(CPU::INS_LDA_ABSX, &emulator::CPU::A);
}
TEST_F(load_tests, LDYAbsoluteXCanLoadValueIntoRegisterYWhenBoundsCrossing) {
    test_load_register_absolute_x_crossing(CPU::INS_LDY_ABSX, &emulator::CPU::Y);
}

TEST_F(load_tests, LDAAbsoluteYCanLoadValueIntoRegisterA) {
    test_load_register_absolute_y(CPU::INS_LDA_ABSY, &emulator::CPU::A);
}

TEST_F(load_tests, LDAAbsoluteYCanLoadValueIntoRegisterAWhenBoundsCrossing) {
    test_load_register_absolute_y_crossing(CPU::INS_LDA_ABSY, &emulator::CPU::A);
}
TEST_F(load_tests, LDXAbsoluteYCanLoadValueIntoRegisterXWhenBoundsCrossing) {
    test_load_register_absolute_y_crossing(CPU::INS_LDX_ABSY, &emulator::CPU::X);
}

TEST_F(load_tests, LDAIndirectXCanLoadValueIntoRegisterA) {
    cpu.Z = cpu.N = true;

    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDX;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; //0x02 + 0x4
    mem[0x0007] = 0x80; 
    mem[0x8000] = 0x37; 

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 6;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}


TEST_F(load_tests, LDAIndirectYCanLoadValueIntoRegisterA) {
    cpu.Z = cpu.N = true;

    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80; 
    mem[0x8004] = 0x37;  //0x8000+ 0x4

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 5;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

TEST_F(load_tests, LDAIndirectYCanLoadValueIntoRegisterAWhenBoundsCrossing) {
    cpu.Y = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02;
    mem[0x0003] = 0x80; 
    mem[0x8101] = 0x37;  //0x8002 + 0xFF

    //when
    CPU cpu_copy = cpu;
    constexpr s32 EXPECTED_CYCLES = 6;
    s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycles_used, EXPECTED_CYCLES);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_from_load_register(cpu, cpu_copy);
}

