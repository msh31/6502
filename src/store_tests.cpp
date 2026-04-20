#include "emulator.hpp"
#include <gtest/gtest.h>

using namespace emulator;

class store_tests : public testing::Test {
public:
    Memory mem;
    CPU cpu;

    //tip: do not use these in all lowercase.
    virtual void SetUp() override {
        cpu.reset(mem);
    }

    virtual void TearDown() override {

    }

    void store_register_zero_page(byte opcode, byte CPU::*reg);
    void store_register_absolute(byte opcode, byte CPU::*reg);
    void store_register_zero_page_x(byte opcode, byte CPU::*reg);
};

static void verify_unmodified_flags_from_store_register(const CPU& cpu, const CPU& cpu_copy) {
    EXPECT_EQ(cpu.C, cpu_copy.C);
    EXPECT_EQ(cpu.I, cpu_copy.I);
    EXPECT_EQ(cpu.D, cpu_copy.D);
    EXPECT_EQ(cpu.B, cpu_copy.B);
    EXPECT_EQ(cpu.V, cpu_copy.V);
    EXPECT_EQ(cpu.Z, cpu_copy.Z);
    EXPECT_EQ(cpu.N, cpu_copy.N);
}

void store_tests::store_register_zero_page(byte opcode, byte CPU::*reg) {
    cpu.*reg = 0x23;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0x0080] = 0x00;

    constexpr s32 expected_cycles = 3;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x0080], 0x23);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

void store_tests::store_register_absolute(byte opcode, byte CPU::*reg) {
    cpu.*reg = 0x23;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0x00;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x8000], 0x23);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

void store_tests::store_register_zero_page_x(byte opcode, byte CPU::*reg) {
    cpu.*reg = 0x42;
    cpu.X = 0x0F;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0x008F] = 0x00;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x0080], 0x42);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

//actual tests

TEST_F(store_tests, STAZeroPageCanStoreRegisterAIntoMemory) {
    store_register_zero_page(CPU::INS_STA_ZP, &CPU::A);
}
TEST_F(store_tests, STXZeroPageCanStoreRegisterAIntoMemory) {
    store_register_zero_page(CPU::INS_STX_ZP, &CPU::X);
}
TEST_F(store_tests, STYZeroPageCanStoreRegisterAIntoMemory) {
    store_register_zero_page(CPU::INS_STY_ZP, &CPU::Y);
}

TEST_F(store_tests, STAZeroPageXCanStoreRegisterAIntoMemory) {
    store_register_zero_page_x(CPU::INS_STA_ZPX, &CPU::A);
}
TEST_F(store_tests, STYZeroPageXCanStoreRegisterAIntoMemory) {
    store_register_zero_page_x(CPU::INS_STY_ZPX, &CPU::X);
}

TEST_F(store_tests, STXZeroPageYCanStoreRegisterAIntoMemory) {
    cpu.A = 0x42;
    cpu.X = 0x0F;
    mem[0xFFFC] = CPU::INS_STX_ZPY;
    mem[0xFFFD] = 0x80;
    mem[0x008F] = 0x00;

    constexpr s32 expected_cycles = 4;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x0080], 0x42);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

TEST_F(store_tests, STAAbsoluteCanStoreRegisterAIntoMemory) {
    store_register_absolute(CPU::INS_STA_ABS, &CPU::A);
}
TEST_F(store_tests, STXAbsoluteCanStoreRegisterAIntoMemory) {
    store_register_absolute(CPU::INS_STX_ABS, &CPU::X);
}
TEST_F(store_tests, STYAbsoluteCanStoreRegisterAIntoMemory) {
    store_register_absolute(CPU::INS_STY_ABS, &CPU::Y);
}

TEST_F(store_tests, STAAbsoluteXCanStoreRegisterAIntoMemory) {
    cpu.A = 0x42;
    cpu.X = 0x0F;

    mem[0xFFFC] = CPU::INS_STA_ABSX;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;

    constexpr s32 expected_cycles = 5;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x008F], 0x42);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

TEST_F(store_tests, STAAbsoluteYCanStoreRegisterAIntoMemory) {
    cpu.A = 0x42;
    cpu.Y = 0x0F;

    mem[0xFFFC] = CPU::INS_STA_ABSY;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;

    constexpr s32 expected_cycles = 5;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x008F], 0x42);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

TEST_F(store_tests, STAIndirectXCanStoreRegisterAIntoMemory) {
    cpu.A = 0x42;
    cpu.X = 0x0F;

    mem[0xFFFC] = CPU::INS_STA_INDX;
    mem[0xFFFD] = 0x20;
    mem[0x002F] = 0x00;
    mem[0x0030] = 0x80;
    mem[0x8000] = 0x00;

    constexpr s32 expected_cycles = 6;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x8000], 0x42);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}

TEST_F(store_tests, STAIndirectYCanStoreRegisterAIntoMemory) {
    cpu.A = 0x42;
    cpu.Y = 0x0F;

    mem[0xFFFC] = CPU::INS_STA_INDX;
    mem[0xFFFD] = 0x20;
    mem[0x0020] = 0x00;
    mem[0x0021] = 0x80;
    mem[0x8000 + 0x0F] = 0x00;

    constexpr s32 expected_cycles = 6;
    CPU cpu_copy = cpu;

    const s32 real_cycle_count = cpu.execute(expected_cycles, mem);

    EXPECT_EQ(real_cycle_count, expected_cycles);
    EXPECT_EQ(mem[0x8000 + 0x0F], 0x42);
    verify_unmodified_flags_from_store_register(cpu, cpu_copy);
}
