#pragma once
#include <print>

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

    void write_word(word value, u32 address, u32& cycles) {
        data[address]       = value & 0xFF;
        data[address + 1]   = (value >> 8);
        cycles -= 2;
    }
};

struct CPU {
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
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_JSR = 0x20;

    void lda_set_status() {
        Z = (A==0);
        N = (A & 0b10000000) > 0;
    }

    void reset(Memory& mem) {
        program_counter = 0xFFFC;
        stack_pointer = 0xFF;
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
        byte data = mem[address];
        cycles--;
        return data;
    }

    //6502 is little endian
    word fetch_word(u32& cycles, Memory& mem) {
        word data = mem[program_counter];
        program_counter++;

        data |= (mem[program_counter] << 8);
        // program_counter++;
        cycles -= 2;
        return data;
    }
    
    void execute(u32 cycles, Memory& mem) { 
        while(cycles > 0) {
            // std::println("Fetching opcode at PC = 0x{:04X}, value = 0x{:02X}", program_counter, mem[program_counter]);
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
                 //        std::println("LDA_ZPX: zp_addr after offset = 0x{:02X}, X = 0x{:02X}, value at that addr = 0x{:02X}", 
                 // zero_page_addr, X, mem[zero_page_addr]);
                    cycles--;
                    A = read_byte(zero_page_addr, cycles, mem);
                    lda_set_status();
                } break;
                case INS_JSR: {
                    // std::println("before JSR: PC = 0x{:04X}, SP = 0x{:02X}", program_counter, (u32)stack_pointer);
                    word sub_addr = fetch_word(cycles, mem);
                    // std::println("After fetch_word: PC = 0x{:04X}, sub_addr = 0x{:04X}", program_counter, sub_addr);
                    // std::println("Pushing to address: 0x{:04X}, value: 0x{:04X}", 0x0100 | stack_pointer, program_counter);

                    mem.write_word(program_counter -1, 0x0100 | stack_pointer, cycles);

                    // std::println("after push: SP = 0x{:02X}", (u32)stack_pointer);
                    program_counter = sub_addr;
                    stack_pointer--;
                    // std::println("after JSR complete: PC = 0x{:04X}, SP = 0x{:02X}", program_counter -1, (u32)stack_pointer);
              } break;
                default:
                  std::println("Instruction not handled: {}!", instruction);
                break;
            }
        }
    }
};
