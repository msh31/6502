#include "emulator.hpp"
#include <gtest/gtest.h>

class general_test : public testing::Test {
public:
    Memory mem;
    CPU cpu;

    //tip: do not use these in all lowercase.
    virtual void SetUp() override {
        cpu.reset(mem);
    }

    virtual void TearDown() override {

    }
};

static void verify_unmodified_flags_lda(const CPU& cpu, const CPU& cpu_copy) {
    EXPECT_EQ(cpu.C, cpu_copy.C);
    EXPECT_EQ(cpu.I, cpu_copy.I);
    EXPECT_EQ(cpu.D, cpu_copy.D);
    EXPECT_EQ(cpu.B, cpu_copy.B);
    EXPECT_EQ(cpu.V, cpu_copy.V);
}

TEST_F(general_test, LDAImmediateCanLoadValueIntoRegister) {
    //given
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(2, mem);

    //then
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycles_used, 2);

    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);

    verify_unmodified_flags_lda(cpu, cpu_copy);
}

TEST_F(general_test, LDAZeroPageCanLoadValueIntoRegisterA) {
    //given
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(3, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycles_used, 3);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_lda(cpu, cpu_copy);
}

TEST_F(general_test, LDAZeroPageXCanLoadValueIntoRegisterA) {
    //given
    cpu.X = 5;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    //when
    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(4, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycles_used, 4);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);

    verify_unmodified_flags_lda(cpu, cpu_copy);
}

TEST_F(general_test, LDAZeroPageXCanLoadValueIntoRegisterAWhenItWraps) {
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

    verify_unmodified_flags_lda(cpu, cpu_copy);
}
