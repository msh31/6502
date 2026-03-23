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

    void reset(Memory& mem) {
        program_counter = 0xFFFC;
        stack_pointer = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        mem.init();
    }

    byte fetch_byte(u32 cycles, Memory& mem) {
        byte data = mem[program_counter];
        program_counter++;
        cycles--;

        return data;
    }

    void execute(u32 cycles, Memory& mem) { 
        while(cycles > 0) {
            byte instruction = fetch_byte(cycles, mem);
            // ()
        }
    }
};

int main() {
    Memory mem;
    CPU cpu;
    cpu.reset(mem);
    cpu.execute(2, mem);

    return 0;
}
