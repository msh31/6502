#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

class transfer_tests : public testing::Test {
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

TEST_F(transfer_tests, CanTAXTransferARegisterToXRegister) {
    cpu.A = 0x69;
    mem[0xFFFC] = CPU::INS_TAX;

    cpu.execute(2, mem);

    EXPECT_EQ(cpu.X, 0x69);
    EXPECT_FALSE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
}
TEST_F(transfer_tests, CanTAXTransferARegisterToXRegisterNegative) {
    cpu.A = 0x80;
    mem[0xFFFC] = CPU::INS_TAX;

    cpu.execute(2, mem);

    EXPECT_EQ(cpu.X, 0x80);
    EXPECT_FALSE(cpu.flag.Z);
    EXPECT_TRUE(cpu.flag.N);
}
TEST_F(transfer_tests, CanTAXTransferARegisterToXRegisterZero) {
    cpu.A = 0x00;
    mem[0xFFFC] = CPU::INS_TAX;

    cpu.execute(2, mem);

    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
}
