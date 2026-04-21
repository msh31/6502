#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

class jump_call_tests : public testing::Test {
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

TEST_F(jump_call_tests, CanJumpToSubroutineAndBack) {
    cpu.reset(0xFF00, mem);

    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = CPU::INS_RTS;
    mem[0xFF03] = CPU::INS_LDA_IM;
    mem[0xFF04] = 0x42;

    constexpr s32 expected_cycles = 6 + 6 + 2;
    CPU cpu_copy = cpu;

    const s32 real_cycles = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycles, expected_cycles);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.stack_pointer, cpu_copy.stack_pointer);
}

TEST_F(jump_call_tests, JSRDoesNotAffectProcessorStatus) {
    cpu.reset(0xFF00, mem);

    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;

    constexpr s32 expected_cycles = 6;
    CPU cpu_copy = cpu;

    const s32 real_cycles = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycles, expected_cycles);
    EXPECT_EQ(cpu.ps, cpu_copy.ps); //processor status
    EXPECT_NE(cpu.stack_pointer, cpu_copy.stack_pointer);
}

TEST_F(jump_call_tests, RTSDoesNotAffectProcessorStatus) {
    cpu.reset(0xFF00, mem);

    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = CPU::INS_RTS;

    constexpr s32 expected_cycles = 6 + 6;
    CPU cpu_copy = cpu;

    const s32 real_cycles = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycles, expected_cycles);
    EXPECT_EQ(cpu.ps, cpu_copy.ps); //processor status
    EXPECT_EQ(cpu.program_counter, 0xFF03);
}

TEST_F(jump_call_tests, JMPAbsoluteCanJumpToNewLocation) {
    cpu.reset(0xFF00, mem);

    mem[0xFF00] = CPU::INS_JMP_ABS;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;

    constexpr s32 expected_cycles = 3;
    CPU cpu_copy = cpu;

    const s32 real_cycles = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycles, expected_cycles);
    EXPECT_EQ(cpu.ps, cpu_copy.ps); //processor status
    // EXPECT_NE(cpu.stack_pointer, cpu_copy.stack_pointer); 
    EXPECT_EQ(cpu.stack_pointer, cpu_copy.stack_pointer); 
    EXPECT_EQ(cpu.program_counter, 0x8000);
}

TEST_F(jump_call_tests, JMPIndirectCanJumpToNewLocation) {
    cpu.reset(0xFF00, mem);

    mem[0xFF00] = CPU::INS_JMP_IND;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = 0x00;
    mem[0x8001] = 0x90;

    constexpr s32 expected_cycles = 5;
    CPU cpu_copy = cpu;

    const s32 real_cycles = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycles, expected_cycles);
    EXPECT_EQ(cpu.ps, cpu_copy.ps); //processor status
    EXPECT_EQ(cpu.program_counter, 0x9000);
}
