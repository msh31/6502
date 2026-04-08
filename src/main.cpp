#include <stdio.h>

using byte = unsigned char;     //8-bit
using word = unsigned short;    //16-bit
using u32 = unsigned int;

struct Memory {
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

struct CPU {
    word program_counter;
    word stack_pointer;

    byte A, X, Y;                   //registers

    byte C : 1;                     //bit fields (used for the processor flags)
    byte Z : 1;
    byte I : 1;
    byte D : 1;
    byte B : 1;
    byte V : 1;
    byte N : 1;

    static constexpr byte
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5;

    void lda_set_status() {
        Z = (A==0);
        N = (A & 0b10000000) > 0;
    }

    void reset(Memory& mem) {
        program_counter = 0xFFFC;
        stack_pointer = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        mem.init();
    }

    byte fetch_byte(u32& cycles, Memory& mem) {
        byte data = mem[program_counter];
        program_counter++;
        cycles--;

        return data;
    }

    byte read_byte(byte address, u32& cycles, Memory& mem) {
        byte data = mem[program_counter];
        cycles--;
        return data;
    }

    void execute(u32 cycles, Memory& mem) { 
        while(cycles > 0) {
            byte instruction = fetch_byte(cycles, mem);
            // ()
            switch(instruction) {
                case INS_LDA_IM: {
                    byte value = fetch_byte(cycles, mem);
                    A = value;
                    lda_set_status();
                } break;
                case INS_LDA_ZP: {
                    byte zero_page_addr = fetch_byte(cycles, mem);
                    A = read_byte(zero_page_addr, cycles, mem);
                    lda_set_status();
                } break;
                case INS_LDA_ZPX: {
                    byte zero_page_addr = fetch_byte(cycles, mem);
                    zero_page_addr += X;
                    cycles--;
                    A = read_byte(zero_page_addr, cycles, mem);
                    lda_set_status();
                } break;
                default:
                    printf("Instruction not handled: %d!", instruction);
                    break;
            }
        }
    }
};

int main() {
    Memory mem;
    CPU cpu;
    cpu.reset(mem);
//prog start
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x84;
//prog end

    cpu.execute(3, mem);

    return 0;
}
