#pragma once
#include <print>

using byte = unsigned char;     //8-bit
using word = unsigned short;    //16-bit
using u32 = unsigned int;
using s32 = signed int;

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

    void write_word(word value, u32 address, s32& cycles) {
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
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1,
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

    byte fetch_byte(s32& cycles, Memory& mem) {
        byte data = mem[program_counter];
        program_counter++;
        cycles--;

        return data;
    }

    byte read_byte(byte address, s32& cycles, Memory& mem) {
        byte data = mem[address];
        cycles--;
        return data;
    }

    //6502 is little endian
    word fetch_word(s32& cycles, Memory& mem) {
        word data = mem[program_counter];
        program_counter++;

        data |= (mem[program_counter] << 8);
        // program_counter++;
        cycles -= 2;
        return data;
    }
   
    //returns the number of cycles used
    u32 execute(s32 cycles, Memory& mem) { 
        const s32 req_cycles = cycles;

        while(cycles > 0) {
            byte instruction = fetch_byte(cycles, mem);

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
                case INS_LDA_ABS: {

                } break;
                // case INS_LDA_ABS: {
                //
                // } break;
                // case INS_LDA_ABS: {
                //
                // } break;
                // case INS_LDA_ABS: {
                //
                // } break;
                case INS_JSR: {
                    word sub_addr = fetch_word(cycles, mem);
                    mem.write_word(program_counter -1, 0x0100 | stack_pointer, cycles);

                    program_counter = sub_addr;
                    stack_pointer -= 2;
                    cycles -= 1;
              } break;
                default:
                  std::println("Instruction not handled: {}!", instruction);
                break;
            }
        }
        return req_cycles - cycles;
    }
};
