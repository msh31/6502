#pragma once

using byte = unsigned char;     //8-bit
using word = unsigned short;    //16-bit
using u32 = unsigned int;
using s32 = signed int;

namespace emulator {
    struct Memory;
    struct CPU;
}

struct emulator::Memory {
    static constexpr u32 MAX_MEMORY = 1024 * 64;
    byte data[MAX_MEMORY];

    void init() {
        for(u32 i = 0; i < MAX_MEMORY; i++) {
            data[i] = 0;
        }
    }

    // read 1 byte 
    byte operator[](u32 address) const {
        //assert here address is < max_memory
        return data[address];
    }


    // write 1 byte 
    byte& operator[](u32 address) {
        return data[address];
    }

    void write_word(word value, u32 address, s32& cycles) {
        data[address]       = value & 0xFF;
        data[address + 1]   = (value >> 8);
        cycles -= 2;
    }
};

struct emulator::CPU {
    word program_counter;
    byte stack_pointer;

    byte A, X, Y;                   //registers

    byte C : 1;                     //bit fields (used for the processor flags)
    byte Z : 1;
    byte I : 1;
    byte D : 1;
    byte B : 1;
    byte V : 1;
    byte N : 1;

    static constexpr byte
        //LDA
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1,

        //LDX
        INS_LDX_IM = 0xA2,
        INS_LDX_ZP = 0xA6,
        INS_LDX_ZPY = 0xB6,
        INS_LDX_ABS = 0xAE,
        INS_LDX_ABSY = 0xBE,

        //LDY
        INS_LDY_IM = 0xA0,
        INS_LDY_ZP = 0xA4,
        INS_LDY_ZPX = 0xB4,
        INS_LDY_ABS = 0xAC,
        INS_LDY_ABSX = 0xBC,

        INS_JSR = 0x20;

    //sets the right status after load register instr | LDA, LDX, LDY
    void load_register_set_status(byte reg) {
        Z = (reg==0);
        N = (reg & 0b10000000) > 0;
    }

    void reset(Memory& mem) {
        program_counter = 0xFFFC;
        stack_pointer = 0xFF;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        mem.init();
    }

    byte read_byte(word address, s32& cycles, Memory& mem) {
        byte data = mem[address];
        cycles--;
        return data;
    }
    word read_word(word address, s32& cycles, Memory& mem) {
        byte low = read_byte(address, cycles, mem);
        byte high = read_byte(address + 1, cycles, mem);
        return low | (high << 8);
    }

    byte fetch_byte(s32& cycles, const Memory& mem) {
        byte data = mem[program_counter];
        program_counter++;
        cycles--;

        return data;
    }
    //6502 is little endian
    word fetch_word(s32& cycles, const Memory& mem) {
        word data = mem[program_counter];
        program_counter++;

        data |= (mem[program_counter] << 8);
        program_counter++;
        cycles -= 2;
        return data;
    }
   
    //returns the number of cycles used
    u32 execute(s32 cycles, Memory& mem);

    //addressing mode zero page
    word get_zero_page_address(s32& cycles, const Memory& mem) {
       return fetch_byte(cycles, mem);
    } 

    //addressing mode with zero page X offset
    word get_zero_page_x_address(s32& cycles, const Memory& mem) {
        byte zero_page_addr = fetch_byte(cycles, mem);
        zero_page_addr += X;
        cycles--;
        return zero_page_addr;
    }

    //addressing mode with zero page Y offset
    word get_zero_page_y_address(s32& cycles, const Memory& mem) {
        byte zero_page_addr = fetch_byte(cycles, mem);
        zero_page_addr += Y;
        cycles--;
        return zero_page_addr;
    }

    //addressing mode absolute
    word get_absolute_address(s32& cycles, const Memory& mem) {
        return fetch_word(cycles, mem);
    }

    //addressing mode with absolute X offset
    word get_absolute_x_address(s32& cycles, const Memory& mem) {
        word abs_addr = fetch_word(cycles, mem);
        word abs_addr_x = abs_addr + X;
        // cycles--;
        // A = read_byte(abs_addr_x, cycles, mem);

        //crosses page boundary
        if(abs_addr_x - abs_addr >= 0XFF) cycles--;
        // load_register_set_status(A);
        return abs_addr_x;
    }

    //addressing mode with absolute X offset
    word get_absolute_y_address(s32& cycles, const Memory& mem) {
        word abs_addr = fetch_word(cycles, mem);
        word abs_addr_y = abs_addr + Y;
        // cycles--;
        // A = read_byte(abs_addr_x, cycles, mem);

        //crosses page boundary
        if(abs_addr_y - abs_addr >= 0XFF) cycles--;
        // load_register_set_status(A);
        return abs_addr_y;
    }
};
