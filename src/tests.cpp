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

TEST_F(general_test, LDAImmediateCanLoadValueIntoRegister) {
    //given
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;

    // std::cout << "mem[0x4242] before execute: 0x" << std::hex << (int)mem[0x4242] << std::dec << std::endl;
    // std::cout << "PC before execute: 0x" << std::hex << cpu.program_counter << std::dec << std::endl;
    //when
    cpu.execute(9, mem);

    //then
    EXPECT_EQ(cpu.A, 0x84);
}

TEST_F(general_test, LDAZeroPageCanLoadValueIntoRegisterA) {
    //given
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    //when
    cpu.execute(3, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
}

TEST_F(general_test, LDAZeroPageXCanLoadValueIntoRegisterA) {
    //given
    cpu.X = 5;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    //when
    cpu.execute(4, mem);

    //then
    EXPECT_EQ(cpu.A, 0x37);
}
