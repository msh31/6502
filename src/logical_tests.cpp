#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

static void verify_unmodified_flags_from_load_register(const CPU& cpu, const CPU& cpu_copy) {
    EXPECT_EQ(cpu.flag.C, cpu_copy.flag.C);
    EXPECT_EQ(cpu.flag.I, cpu_copy.flag.I);
    EXPECT_EQ(cpu.flag.D, cpu_copy.flag.D);
    EXPECT_EQ(cpu.flag.B, cpu_copy.flag.B);
    EXPECT_EQ(cpu.flag.V, cpu_copy.flag.V);
}

class logical_tests : public testing::Test {
public:
    Memory mem;
    CPU cpu;

    //tip: do not use these in all lowercase.
    virtual void SetUp() override {
        cpu.reset(0xFFFC, mem);
    }

    virtual void TearDown() override {

    }

    enum e_logical_operation {
        AND, 
        EOR,
        OR,
    };

    byte do_logical_op(byte a, byte b, e_logical_operation lop) {
        switch( lop ) {
            case AND:
                return a & b;
                break;
            case EOR:
                return a | b;
                break;
            case OR:
                return a ^ b;
                break;
        }

        throw -1; //shouldnt be reachable
    }

    void test_logical_on_register_a_immediate(e_logical_operation lop) {
        cpu.A = 0xCC;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_IM;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_IM;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_IM;
                break;
        }

        // mem[0xFFFC] = lop;
        mem[0xFFFD] = 0x84;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(2, mem);

        EXPECT_EQ(cpu.A, do_logical_op(0xCC, 0x84, lop));
        EXPECT_EQ(cycles_used, 2);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_TRUE(cpu.flag.N);
        verify_unmodified_flags_from_load_register(cpu, cpu_copy);
    }
    void test_load_register_zero_page(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_zero_page_x(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_zero_page_y(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_x(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_y(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_x_crossing(byte opcode, byte emulator::CPU::*reg);
    void test_load_register_absolute_y_crossing(byte opcode, byte emulator::CPU::*reg);
};

TEST_F(logical_tests, TestLogicalOpANDonRegisterAImmediate) {
    test_logical_on_register_a_immediate(AND);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAImmediate) {
    test_logical_on_register_a_immediate(OR);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAImmediate) {
    test_logical_on_register_a_immediate(EOR);
}

// void logical_tests::test_load_register_zero_page(byte opcode, byte emulator::CPU::*reg) {
//     //given
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x42;
//     mem[0x0042] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     s32 cycles_used = cpu.execute(3, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, 3);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_zero_page_x(byte opcode, byte emulator::CPU::*reg) {
//     //given
//     cpu.X = 5;
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x42;
//     mem[0x0047] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     s32 cycles_used = cpu.execute(4, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, 4);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_zero_page_y(byte opcode, byte emulator::CPU::*reg) {
//     //given
//     cpu.Y = 5;
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x42;
//     mem[0x0047] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     s32 cycles_used = cpu.execute(4, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, 4);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_absolute(byte opcode, byte emulator::CPU::*reg) {
//     cpu.flag.Z = cpu.flag.N = true;
//
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x80;
//     mem[0xFFFE] = 0x44; //0x4480
//     mem[0x4480] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 4;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_absolute_x(byte opcode, byte emulator::CPU::*reg) {
//     cpu.flag.Z = cpu.flag.N = true;
//
//     cpu.X = 1;
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x80;
//     mem[0xFFFE] = 0x44; //0x4480
//     mem[0x4481] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 4;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_absolute_y(byte opcode, byte emulator::CPU::*reg) {
//     cpu.flag.Z = cpu.flag.N = true;
//
//     cpu.Y = 1;
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x80;
//     mem[0xFFFE] = 0x44; //0x4480
//     mem[0x4481] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 4;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_absolute_x_crossing(byte opcode, byte emulator::CPU::*reg) {
//     cpu.X = 0xFF;
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x02;
//     mem[0xFFFE] = 0x44; //0x4402
//     mem[0x4501] = 0x37; //0x4402+0xFF crosses page boundary
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 5;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// void logical_tests::test_load_register_absolute_y_crossing(byte opcode, byte emulator::CPU::*reg) {
//     cpu.Y = 0xFF;
//     mem[0xFFFC] = opcode;
//     mem[0xFFFD] = 0x02;
//     mem[0xFFFE] = 0x44; //0x4402
//     mem[0x4501] = 0x37; //0x4402+0xFF crosses page boundary
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 5;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.*reg, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// TEST_F(logical_tests, LDAImmediateCanAffectZeroFlag) {
//     //given
//     cpu.A = 0x44;
//     mem[0xFFFC] = CPU::INS_LDA_IM;
//     mem[0xFFFD] = 0x0;
//
//     //when
//     CPU cpu_copy = cpu;
//     s32 cycles_used = cpu.execute(2, mem);
//
//     //then
//     EXPECT_TRUE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// TEST_F(logical_tests, LDAZeroPageCanLoadValueIntoRegisterA) {
//     test_load_register_zero_page(CPU::INS_LDA_ZP, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAZeroPageXCanLoadValueIntoRegisterA) {
//     test_load_register_zero_page_x(CPU::INS_LDA_ZPX, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAZeroPageXCanLoadValueIntoRegisterAWhenItWraps) {
//     //given
//     cpu.X = 0xFF;
//     mem[0xFFFC] = CPU::INS_LDA_ZPX;
//     mem[0xFFFD] = 0x80;
//     mem[0x007F] = 0x37;
//
//     //when
//     CPU cpu_copy = cpu;
//     s32 cycles_used = cpu.execute(4, mem);
//
//     //then
//     EXPECT_EQ(cpu.A, 0x37);
//     EXPECT_EQ(cycles_used, 4);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// TEST_F(logical_tests, LDAAbsoluteCanLoadValueIntoRegisterA) {
//     test_load_register_absolute(CPU::INS_LDA_ABS, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAAbsoluteXCanLoadValueIntoRegisterA) {
//     test_load_register_absolute_x(CPU::INS_LDA_ABSX, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAAbsoluteXCanLoadValueIntoRegisterAWhenBoundsCrossing) {
//     test_load_register_absolute_x_crossing(CPU::INS_LDA_ABSX, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAAbsoluteYCanLoadValueIntoRegisterA) {
//     test_load_register_absolute_y(CPU::INS_LDA_ABSY, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAAbsoluteYCanLoadValueIntoRegisterAWhenBoundsCrossing) {
//     test_load_register_absolute_y_crossing(CPU::INS_LDA_ABSY, &emulator::CPU::A);
// }
//
// TEST_F(logical_tests, LDAIndirectXCanLoadValueIntoRegisterA) {
//     cpu.flag.Z = cpu.flag.N = true;
//
//     cpu.X = 0x04;
//     mem[0xFFFC] = CPU::INS_LDA_INDX;
//     mem[0xFFFD] = 0x02;
//     mem[0x0006] = 0x00; //0x02 + 0x4
//     mem[0x0007] = 0x80; 
//     mem[0x8000] = 0x37; 
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 6;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.A, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
//
// TEST_F(logical_tests, LDAIndirectYCanLoadValueIntoRegisterA) {
//     cpu.flag.Z = cpu.flag.N = true;
//
//     cpu.Y = 0x04;
//     mem[0xFFFC] = CPU::INS_LDA_INDY;
//     mem[0xFFFD] = 0x02;
//     mem[0x0002] = 0x00;
//     mem[0x0003] = 0x80; 
//     mem[0x8004] = 0x37;  //0x8000+ 0x4
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 5;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.A, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
// TEST_F(logical_tests, LDAIndirectYCanLoadValueIntoRegisterAWhenBoundsCrossing) {
//     cpu.Y = 0xFF;
//     mem[0xFFFC] = CPU::INS_LDA_INDY;
//     mem[0xFFFD] = 0x02;
//     mem[0x0002] = 0x02;
//     mem[0x0003] = 0x80; 
//     mem[0x8101] = 0x37;  //0x8002 + 0xFF
//
//     //when
//     CPU cpu_copy = cpu;
//     constexpr s32 EXPECTED_CYCLES = 6;
//     s32 cycles_used = cpu.execute(EXPECTED_CYCLES, mem);
//
//     //then
//     EXPECT_EQ(cpu.A, 0x37);
//     EXPECT_EQ(cycles_used, EXPECTED_CYCLES);
//
//     EXPECT_FALSE(cpu.flag.Z);
//     EXPECT_FALSE(cpu.flag.N);
//
//     verify_unmodified_flags_from_load_register(cpu, cpu_copy);
// }
//
