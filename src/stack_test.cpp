#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

class stack_tests : public testing::Test {
public:
    Memory mem;
    CPU cpu;

    //tip: do not use these in all lowercase.
    virtual void SetUp() override {
        cpu.reset(0xFFFC, mem);
    }

    virtual void TearDown() override {

    }
};

TEST_F(stack_tests, TSXCanTransferStackPointerToRegisterX) {
    cpu.reset(0xFF00, mem);
    cpu.flag.Z = cpu.flag.N = true;
    cpu.X = 0x00;
    cpu.stack_pointer = 0x01;
    mem[0xFF00] = CPU::INS_TSX;

    constexpr s32 expected_cycles = 2;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.X, 0x01);
    EXPECT_FALSE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
}

TEST_F(stack_tests, TSXCanTransferAZeroStackPointerToRegisterXSettingFlags) {
    cpu.reset(0xFF00, mem);
    cpu.flag.Z = cpu.flag.N = true;
    cpu.X = 0x00;
    cpu.stack_pointer = 0x00;
    mem[0xFF00] = CPU::INS_TSX;

    constexpr s32 expected_cycles = 2;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
}

TEST_F(stack_tests, TSXCanTransferANegativeStackPointerToRegisterXSettingFlags) {
    cpu.reset(0xFF00, mem);
    cpu.flag.Z = cpu.flag.N = false;
    cpu.X = 0x00;
    cpu.stack_pointer = 0b10000000;
    mem[0xFF00] = CPU::INS_TSX;

    constexpr s32 expected_cycles = 2;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.X, 0b10000000);
    EXPECT_FALSE(cpu.flag.Z);
    EXPECT_TRUE(cpu.flag.N);
}


TEST_F(stack_tests, TXSCanTransferRegisterXToStackPointer) {
    cpu.reset(0xFF00, mem);
    cpu.X = 0xFF;
    cpu.stack_pointer = 0;
    mem[0xFF00] = CPU::INS_TXS;

    constexpr s32 expected_cycles = 2;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.stack_pointer, 0xFF);
    EXPECT_EQ(cpu.ps, cpu_copy.ps);
}



TEST_F(stack_tests, PHACanPushRegisterOnTheStack) {
    cpu.reset(0xFF00, mem);
    cpu.X = 0x69;
    mem[0xFF00] = CPU::INS_PHA;

    constexpr s32 expected_cycles = 3;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[cpu.stack_pointer_to_address() + 1], cpu.A);
    EXPECT_EQ(cpu.ps, cpu_copy.ps);
    EXPECT_EQ(cpu.stack_pointer, 0xFE);
}


TEST_F(stack_tests, PHPCanPushProcessorStatusOnTheStack) {
    cpu.reset(0xFF00, mem);
    cpu.ps = 0xCC;
    mem[0xFF00] = CPU::INS_PHP;

    constexpr s32 expected_cycles = 3;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[cpu.stack_pointer_to_address() + 1], 0xCC);
    EXPECT_EQ(cpu.ps, cpu_copy.ps);
    EXPECT_EQ(cpu.stack_pointer, 0xFE);
}


TEST_F(stack_tests, PLACanPullAValueFromStackIntoRegisterA) {
    cpu.reset(0xFF00, mem);
    cpu.A = 0x00;
    cpu.stack_pointer = 0xFE;
    mem[0x01FF] = 0x69;
    mem[0xFF00] = CPU::INS_PLA;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.A, 0x69);
    EXPECT_EQ(cpu.stack_pointer, 0xFF);
}


TEST_F(stack_tests, PLACanPullAZeroValueFromStackIntoRegisterA) {
    cpu.reset(0xFF00, mem);
    cpu.flag.Z = false;
    cpu.flag.N = true;
    cpu.A = 0x69;
    cpu.stack_pointer = 0xFE;
    mem[0x01FF] = 0x00;
    mem[0xFF00] = CPU::INS_PLA;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
    EXPECT_EQ(cpu.stack_pointer, 0xFF);
}


TEST_F(stack_tests, PLACanPullANegativeValueFromStackIntoRegisterA) {
    cpu.reset(0xFF00, mem);
    cpu.flag.N = false;
    cpu.flag.Z = true;
    cpu.A = 0x69;
    cpu.stack_pointer = 0xFE;
    mem[0x01FF] = 0b10000001;
    mem[0xFF00] = CPU::INS_PLA;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.A, 0b10000001);
    EXPECT_TRUE(cpu.flag.N);
    EXPECT_FALSE(cpu.flag.Z);
    EXPECT_EQ(cpu.stack_pointer, 0xFF);
}


TEST_F(stack_tests, PLPCanPullAValueFromStackIntoProcessorStatus) {
    cpu.reset(0xFF00, mem);
    cpu.stack_pointer = 0xFE;
    cpu.ps = 0;
    mem[0x01FF] = 0x69;
    mem[0xFF00] = CPU::INS_PLP;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;
    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(cpu.ps, 0x69);
}
