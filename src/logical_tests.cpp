#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

static void verify_unmodified_flags_from_logical_op(const CPU& cpu, const CPU& cpu_copy) {
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

    void test_logical_op_immediate(e_logical_operation lop) {
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
        mem[0xFFFD] = 0x84;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(2, mem);

        const byte expected = do_logical_op(0xCC, 0x84, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 2);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_zero_page(e_logical_operation lop) {
        cpu.A = 0xCC;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ZP;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ZP;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ZP;
                break;
        }
        mem[0xFFFD] = 0x42;
        mem[0x0042] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(3, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 3);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_zero_page_x(e_logical_operation lop) {
        cpu.A = 0xCC;
        cpu.X = 5;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ZPX;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ZPX;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ZPX;
                break;
        }
        mem[0xFFFD] = 0x42;
        mem[0x0042] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(4, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 4);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_zero_page_x_wrap(e_logical_operation lop) {
        cpu.A = 0xCC;
        cpu.X = 0xFF;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ZPX;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ZPX;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ZPX;
                break;
        }
        mem[0xFFFD] = 0x80;
        mem[0x007D] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(4, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 4);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_absolute(e_logical_operation lop) {
        cpu.A = 0xCC;
        cpu.flag.Z = cpu.flag.N = true;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ABS;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ABS;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ABS;
                break;
        }
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44;
        mem[0x4480] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(4, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 4);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_absolute_x(e_logical_operation lop) {
        cpu.flag.Z = cpu.flag.N = true;
        cpu.A = 0xCC;
        cpu.X = 1;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ABSX;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ABSX;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ABSX;
                break;
        }
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44;
        mem[0x4480] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(4, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 4);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_absolute_y(e_logical_operation lop) {
        cpu.flag.Z = cpu.flag.N = true;
        cpu.A = 0xCC;
        cpu.Y = 1;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ABSY;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ABSY;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ABSY;
                break;
        }
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44;
        mem[0x4480] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(4, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 4);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_absolute_x_cross(e_logical_operation lop) {
        cpu.A = 0xCC;
        cpu.X = 0xFF;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ABSX;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ABSX;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ABSX;
                break;
        }
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x44; //0x4402
        mem[0x4501] = 0x37; //0x4402 + 0xFF

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(5, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 5);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_absolute_y_cross(e_logical_operation lop) {
        cpu.A = 0xCC;
        cpu.Y = 0xFF;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_ABSY;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_ABSY;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_ABSY;
                break;
        }
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x44; //0x4402
        mem[0x4501] = 0x37; //0x4402 + 0xFF

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(5, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 5);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_indirect_x(e_logical_operation lop) {
        cpu.flag.Z = cpu.flag.N = true;
        cpu.A = 0xCC;
        cpu.X = 0x04;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_INDX;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_INDX;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_INDX;
                break;
        }
        mem[0xFFFD] = 0x02;
        mem[0x0006] = 0x00;
        mem[0x0007] = 0x80;
        mem[0x8000] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(6, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 6);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_indirect_y(e_logical_operation lop) {
        cpu.flag.Z = cpu.flag.N = true;
        cpu.A = 0xCC;
        cpu.Y = 0x04;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_INDY;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_INDY;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_INDY;
                break;
        }
        mem[0xFFFD] = 0x02;
        mem[0x0006] = 0x00;
        mem[0x0007] = 0x80;
        mem[0x8004] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(5, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 5);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
    void test_logical_op_indirect_y_cross(e_logical_operation lop) {
        cpu.A = 0xCC;
        cpu.Y = 0x04;
        switch( lop ) {
            case AND:
                mem[0xFFFC] = CPU::INS_AND_INDY;
                break;
            case EOR:
                mem[0xFFFC] = CPU::INS_EOR_INDY;
                break;
            case OR:
                mem[0xFFFC] = CPU::INS_ORA_INDY;
                break;
        }
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x02;
        mem[0x0003] = 0x80;
        mem[0x8101] = 0x37;

        CPU cpu_copy = cpu;
        s32 cycles_used = cpu.execute(6, mem);

        const byte expected = do_logical_op(0xCC, 0x37, lop);
        const bool expected_n = (expected & 0b10000000) > 0;

        EXPECT_EQ(cpu.A, expected);
        EXPECT_EQ(cycles_used, 6);

        EXPECT_FALSE(cpu.flag.Z);
        EXPECT_EQ(cpu.flag.N, expected_n);
        verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
    }
};

TEST_F(logical_tests, TestLogicalOpANDonRegisterAImmediate) {
    test_logical_op_immediate(AND);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAImmediate) {
    test_logical_op_immediate(OR);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAImmediate) {
    test_logical_op_immediate(EOR);
}
TEST_F(logical_tests, TestLogicialOpEORImmmediateCanAffectZeroflag) {
    cpu.A = 0xCC;
    mem[0xFFFC] = CPU::INS_EOR_IM;
    mem[0xFFFD] = cpu.A;

    CPU cpu_copy = cpu;
    s32 cycles_used = cpu.execute(2, mem);

    EXPECT_TRUE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
    verify_unmodified_flags_from_logical_op(cpu, cpu_copy);
}


TEST_F(logical_tests, TestLogicialOpANDOnRegisterAZeroPage) {
    test_logical_op_zero_page(AND);
}
TEST_F(logical_tests, TestLogicialOpEOROnRegisterAZeroPage) {
    test_logical_op_zero_page(EOR);
}
TEST_F(logical_tests, TestLogicialOpOROnRegisterAZeroPage) {
    test_logical_op_zero_page(OR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAZeroPageX) {
    test_logical_op_zero_page_x(AND);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAZeroPageX) {
    test_logical_op_zero_page_x(OR);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAZeroPageX) {
    test_logical_op_zero_page_x(EOR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAZeroPageXWrap) {
    test_logical_op_zero_page_x_wrap(AND);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAZeroPageXWrap) {
    test_logical_op_zero_page_x_wrap(EOR);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAZeroPageXWrap) {
    test_logical_op_zero_page_x_wrap(OR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAAbsolute) {
    test_logical_op_absolute(AND);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAAbsolute) {
    test_logical_op_absolute(EOR);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAAbsolute) {
    test_logical_op_absolute(OR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAAbsoluteX) {
    test_logical_op_absolute_x(AND);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAAbsoluteX) {
    test_logical_op_absolute_x(EOR);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAAbsoluteX) {
    test_logical_op_absolute_x(OR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAAbsoluteXCross) {
    test_logical_op_absolute_x_cross(AND);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAAbsoluteXCross) {
    test_logical_op_absolute_x_cross(EOR);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAAbsoluteXCross) {
    test_logical_op_absolute_x_cross(OR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAAbsoluteY) {
    test_logical_op_absolute_y(AND);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAAbsoluteY) {
    test_logical_op_absolute_y(EOR);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAAbsoluteY) {
    test_logical_op_absolute_y(OR);
}


TEST_F(logical_tests, TestLogicalOpANDonRegisterAAbsoluteYCross) {
    test_logical_op_absolute_y_cross(AND);
}
TEST_F(logical_tests, TestLogicalOpEORonRegisterAAbsoluteYCross) {
    test_logical_op_absolute_y_cross(EOR);
}
TEST_F(logical_tests, TestLogicalOpORonRegisterAAbsoluteYCross) {
    test_logical_op_absolute_y_cross(OR);
}

TEST_F( logical_tests, TestLogicalOpANDIndirectX ) {
	test_logical_op_indirect_x( AND );
}
TEST_F( logical_tests, TestLogicalOpEORIndirectX ) {
	test_logical_op_indirect_x( EOR );
}
TEST_F( logical_tests, TestLogicalOpORIndirectX ) {
	test_logical_op_indirect_x( OR );
}

TEST_F( logical_tests, TestLogicalOpANDIndirectY ) {
	test_logical_op_indirect_y( AND );
}
TEST_F( logical_tests, TestLogicalOpEORIndirectY ) {
	test_logical_op_indirect_y( EOR );
}
TEST_F( logical_tests, TestLogicalOpORIndirectY ) {
	test_logical_op_indirect_y( OR );
}

TEST_F( logical_tests, TestLogicalOpANDIndirectYCross) {
	test_logical_op_indirect_y_cross( AND );
}
TEST_F( logical_tests, TestLogicalOpEORIndirectYCross) {
	test_logical_op_indirect_y_cross( EOR );
}
TEST_F( logical_tests, TestLogicalOpORIndirectYCross) {
	test_logical_op_indirect_y_cross( OR );
}

