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
};

struct status_flags {
    byte C : 1;                     //bit fields (used for the processor flags)
    byte Z : 1;
    byte I : 1;
    byte D : 1;
    byte B : 1;
    byte V : 1;
    byte N : 1;
};

struct emulator::CPU {
    word program_counter;
    byte stack_pointer;

    byte A, X, Y;                   //registers
    
    union { //processor status
        byte ps;
        status_flags flag;
    };

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

        //STA
        INS_STA_ZP = 0x85,
        INS_STA_ZPX = 0x95,
        INS_STA_ABS = 0x8D,
        INS_STA_ABSX = 0x9D,
        INS_STA_ABSY = 0x99,
        INS_STA_INDX = 0x81,
        INS_STA_INDY = 0x91,

        //STX
        INS_STX_ZP = 0x86,
        INS_STX_ZPY = 0x96,
        INS_STX_ABS = 0x8E,

        //STY
        INS_STY_ZP = 0x84,
        INS_STY_ZPX = 0x94,
        INS_STY_ABS = 0x8C,

        //JSR
        INS_JSR = 0x20,

        //RTS
        INS_RTS = 0x60;

    //sets the right status after load register instr | LDA, LDX, LDY
    void load_register_set_status(byte reg) {
        flag.Z = (reg==0);
        flag.N = (reg & 0b10000000) > 0;
    }

    void reset(word reset_vector, Memory& mem) {
        program_counter = reset_vector;
        stack_pointer = 0xFF;
        flag.C = flag.Z = flag.I = flag.D = flag.B = flag.V = flag.N = 0;
        A = X = Y = 0;
        mem.init();
    }

    byte read_byte(word address, s32& cycles, const Memory& mem) {
        byte data = mem[address];
        cycles--;
        return data;
    }
    void write_byte(byte value, s32& cycles, word addr, Memory& mem) {
        mem[addr] = value;
        cycles--;

        // if(addr > 0xFF) cycles--;
    }

    word read_word(word address, s32& cycles, const Memory& mem) {
        byte low = read_byte(address, cycles, mem);
        byte high = read_byte(address + 1, cycles, mem);
        return low | (high << 8);
    }
    void write_word(word value, word address, s32& cycles, Memory& mem) {
        mem[address]       = value & 0xFF;
        mem[address + 1]   = (value >> 8);
        cycles -= 2;
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

    //returns the SP as a full 16-bit addr (in 1st page)
    word stack_pointer_to_address() const {
        return 0x100 | stack_pointer;
    }

    //push program counter - 1 onto the stack
    void push_program_counter_to_stack(s32& cycles, Memory& mem) {
        write_word(program_counter -1, stack_pointer_to_address() -1, cycles, mem);
        stack_pointer -= 2;
    }

    //pop from porgram counter
    word pop_word_from_stack(s32& cycles, Memory& mem) {
        word val = read_word(stack_pointer_to_address() + 1, cycles, mem);
        stack_pointer += 2;
        cycles--;
        return val;
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
    word get_absolute_x_address(s32& cycles, const Memory& mem, bool consume_extra = false) {
        word abs_addr = fetch_word(cycles, mem);
        word abs_addr_x = abs_addr + X;
        // cycles--;
        // A = read_byte(abs_addr_x, cycles, mem);

        //crosses page boundary
        if(abs_addr_x - abs_addr >= 0xff || consume_extra) cycles--;
        // load_register_set_status(A);
        return abs_addr_x;
    }

    //addressing mode with absolute X offset
    word get_absolute_y_address(s32& cycles, const Memory& mem, bool consume_extra = false) {
        word abs_addr = fetch_word(cycles, mem);
        word abs_addr_y = abs_addr + Y;
        // cycles--;
        // A = read_byte(abs_addr_x, cycles, mem);

        //crosses page boundary
        if(abs_addr_y - abs_addr >= 0xff || consume_extra) cycles--;
        // load_register_set_status(A);
        return abs_addr_y;
    }

    //addressing mode with indirect X / indeed indirected
    word get_indirect_x_address(s32& cycles, const Memory& mem) {
        byte zp_addr = fetch_byte(cycles, mem);
        zp_addr += X;
        cycles--;
        return read_word(zp_addr, cycles, mem);
    }

    //addressing mode with indirect Y / indirect index
    word get_indirect_y_address(s32& cycles, const Memory& mem, bool consume_extra = false) {
        byte zp_addr = fetch_byte(cycles, mem);
        word effective_addr = read_word(zp_addr, cycles, mem);
        word effective_addr_y = effective_addr + Y;

        if(effective_addr_y  - effective_addr >= 0xff || consume_extra) cycles--;
        return effective_addr_y;
    }
};
