#include "emulator.hpp"
#include <print>

u32 emulator::CPU::execute(s32 cycles, Memory& mem) { 
    //load register with value from the a mem address
    auto load_register = [&cycles, &mem, this](word addr, byte& reg) {
        reg = read_byte(addr, cycles, mem);
        load_register_set_status(reg);
    };

    const s32 req_cycles = cycles;

    while(cycles > 0) {
        byte instruction = fetch_byte(cycles, mem);

        switch(instruction) {
//LDA
            case INS_LDA_IM: {
                                 A = fetch_byte(cycles, mem);
                                 load_register_set_status(A);
                             } break;
            case INS_LDA_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 load_register(addr, A);
                             } break;
            case INS_LDA_ZPX: {
                                  word addr = get_zero_page_x_address(cycles, mem);
                                  load_register(addr, A);
                              } break;
            case INS_LDA_ABS: {
                                  word abs_addr = get_absolute_address(cycles, mem);
                                  load_register(abs_addr, A);
                              } break;
            case INS_LDA_ABSX: {
                                   word addr = get_absolute_x_address(cycles, mem);
                                  load_register(addr, A);
                               } break;
            case INS_LDA_ABSY: {
                                   word addr = get_absolute_y_address(cycles, mem);
                                  load_register(addr, A);
                               } break;
            case INS_LDA_INDX: {
                                   byte zp_addr = fetch_byte(cycles, mem);
                                   zp_addr += X;
                                   cycles--;
                                   word effective_addr = read_word(zp_addr, cycles, mem);
                                   load_register(effective_addr, A);
                               } break;
            case INS_LDA_INDY: {
                                   byte zp_addr = fetch_byte(cycles, mem);
                                   word effective_addr = read_word(zp_addr, cycles, mem);
                                   word effective_addr_y = effective_addr + Y;

                                   if(effective_addr_y - effective_addr >= 0XFF) cycles--;
                                   load_register(effective_addr_y, A);
                               } break;
//LDX
            case INS_LDX_IM: {
                                 X = fetch_byte(cycles, mem);
                                 load_register_set_status(X);
                             } break;
            case INS_LDX_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 load_register(addr, X);
                             } break;
            case INS_LDX_ZPY: {
                                  word addr = get_zero_page_y_address(cycles, mem);
                                  load_register(addr, X);
                              } break;
            case INS_LDX_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  load_register(addr, X);
                              } break;
            case INS_LDX_ABSY: {
                                   word addr = get_absolute_y_address(cycles, mem);
                                  load_register(addr, X);
                               } break;
//LDY
            case INS_LDY_IM: {
                                 Y = fetch_byte(cycles, mem);
                                 load_register_set_status(Y);
                             } break;
            case INS_LDY_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 load_register(addr, Y);
                             } break;
            case INS_LDY_ZPX: {
                                  word addr = get_zero_page_x_address(cycles, mem);
                                  load_register(addr, Y);
                              } break;
            case INS_LDY_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  load_register(addr, Y);
                              } break;
            case INS_LDY_ABSX: {
                                   word addr = get_absolute_x_address(cycles, mem);
                                  load_register(addr, Y);
                               } break;
//JSR
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
