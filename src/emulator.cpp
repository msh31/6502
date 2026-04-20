#include "emulator.hpp"
#include <print>

u32 emulator::CPU::execute(s32 cycles, Memory& mem) { 
    const s32 req_cycles = cycles;

    while(cycles > 0) {
        byte instruction = fetch_byte(cycles, mem);

        switch(instruction) {
            case INS_LDA_IM: {
                                 A = fetch_byte(cycles, mem);
                                 load_register_set_status(A);
                             } break;
            case INS_LDA_ZP: {
                                 byte zero_page_addr = fetch_byte(cycles, mem);
                                 A = read_byte(zero_page_addr, cycles, mem);
                                 load_register_set_status(A);
                             } break;
            case INS_LDA_ZPX: {
                                  byte zero_page_addr = fetch_byte(cycles, mem);
                                  zero_page_addr += X;
                                  cycles--;
                                  A = read_byte(zero_page_addr, cycles, mem);
                                  load_register_set_status(A);
                              } break;
            case INS_LDA_ABS: {
                                  //TODO: processor statsu
                                  word abs_addr = fetch_word(cycles, mem);
                                  A = read_byte(abs_addr, cycles, mem);
                              } break;
            case INS_LDA_ABSX: {
                                  //TODO: processor statsu
                                   word abs_addr = fetch_word(cycles, mem);
                                   word abs_addr_x = abs_addr + X;
                                   // cycles--;
                                   A = read_byte(abs_addr_x, cycles, mem);

                                   //crosses page boundary
                                   if(abs_addr_x - abs_addr >= 0XFF) cycles--;
                               } break;
            case INS_LDA_ABSY: {
                                  //TODO: processor statsu
                                   word abs_addr = fetch_word(cycles, mem);
                                   word abs_addr_y = abs_addr + Y;
                                   // cycles--;
                                   A = read_byte(abs_addr_y, cycles, mem);

                                   //crosses page boundary
                                   if(abs_addr_y - abs_addr >= 0XFF) cycles--;
                               } break;
            case INS_LDA_INDX: {
                                  //TODO: processor statsu
                                   byte zp_addr = fetch_byte(cycles, mem);
                                   zp_addr += X;
                                   word effective_addr = read_word(zp_addr, cycles, mem);
                                   A = read_byte(effective_addr, cycles, mem);
                                   cycles--;
                               } break;
            case INS_LDA_INDY: {
                                  //TODO: processor statsu
                                   byte zp_addr = fetch_byte(cycles, mem);
                                   word effective_addr = read_word(zp_addr, cycles, mem);
                                   word effective_addr_y = effective_addr + Y;
                                   A = read_byte(effective_addr_y, cycles, mem);

                                   if(effective_addr_y - effective_addr >= 0XFF) cycles--;
                               } break;
            case INS_LDX_IM: {
                                 X = fetch_byte(cycles, mem);
                                 load_register_set_status(X);
                             } break;
            case INS_LDY_IM: {
                                 Y = fetch_byte(cycles, mem);
                                 load_register_set_status(Y);
                             } break;

            case INS_JSR: {
                              word sub_addr = fetch_word(cycles, mem);
                              mem.write_word(program_counter -1, 0x0100 | stack_pointer, cycles);

                              program_counter = sub_addr;
                              stack_pointer -= 2;
                              cycles -= 1;
                          } break;
            default:
                          std::println("Instruction not handled: {}!", instruction);
                          throw -1;
                          break;
        }
    }
    return req_cycles - cycles;
}
