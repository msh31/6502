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
                                   word addr = get_indirect_x_address(cycles, mem);
                                   load_register(addr, A);
                               } break;
            case INS_LDA_INDY: {
                                   word addr = get_indirect_y_address(cycles, mem);
                                   load_register(addr, A);
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
//STA
            case INS_STA_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 write_byte(A, cycles, addr, mem);
                             } break;
            case INS_STA_ZPX: {
                                 word addr = get_zero_page_x_address(cycles, mem);
                                 write_byte(A, cycles, addr, mem);
                             } break;
            case INS_STA_ABS: {
                                 word addr = get_absolute_address(cycles, mem);
                                 write_byte(A, cycles, addr, mem);
                             } break;
            case INS_STA_ABSX: {
                                 word addr = get_absolute_x_address(cycles, mem, true);
                                 write_byte(A, cycles, addr, mem);
                             } break;
            case INS_STA_ABSY: {
                                 word addr = get_absolute_y_address(cycles, mem, true);
                                 write_byte(A, cycles, addr, mem);
                             } break;
            case INS_STA_INDX: {
                                   word addr = get_indirect_x_address(cycles, mem);
                                   write_byte(A, cycles, addr, mem);
                               } break;
            case INS_STA_INDY: {
                                   word addr = get_indirect_y_address(cycles, mem, true);
                                   write_byte(A, cycles, addr, mem);
                               } break;
//STX
            case INS_STX_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 write_byte(X, cycles, addr, mem);
                             } break;
            case INS_STX_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  write_byte(X, cycles, addr, mem);
                              } break;
//STY
            case INS_STY_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 write_byte(Y, cycles, addr, mem);
                             } break;
            case INS_STY_ZPX: {
                                  word addr = get_zero_page_x_address(cycles, mem);
                                  write_byte(Y, cycles, addr, mem);
                              } break;
            case INS_STY_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  write_byte(Y, cycles, addr, mem);
                              } break;
//JSR
            case INS_JSR: {
                              word sub_addr = fetch_word(cycles, mem);
                              write_word(program_counter -1, 0x0100 | stack_pointer, cycles, mem);

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
