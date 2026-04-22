#include "emulator.hpp"
#include <print>

//by val because the cycles are passed down to all helpers which use references
//its modified internally not by the caller
u32 emulator::CPU::execute(s32 cycles, Memory& mem) { 
    //load register with value from the a mem address
    auto load_register = [&cycles, &mem, this](word addr, byte& reg) {
        reg = read_byte(addr, cycles, mem);
        load_register_set_status(reg);
    };

    //AND/EOR/OR the A register with val from the addr
    auto and_register = [&cycles, &mem, this](word addr) {
        A &= read_byte(addr, cycles, mem);
        load_register_set_status(A);
    };
    auto eor_register = [&cycles, &mem, this](word addr) {
        A ^= read_byte(addr, cycles, mem);
        load_register_set_status(A);
    };
    auto or_register = [&cycles, &mem, this](word addr) {
        A |= read_byte(addr, cycles, mem);
        load_register_set_status(A);
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
                              push_program_counter_to_stack(cycles, mem);
                              program_counter = sub_addr;
                              cycles--;
                          } break;
//RTS
            case INS_RTS: {  
                              word return_addr = pop_word_from_stack(cycles, mem);
                              program_counter = return_addr + 1;
                              cycles -= 2;
                          } break;
//JMP
            case INS_JMP_ABS: {  
                                  word addr = get_absolute_address(cycles, mem);
                                  program_counter = addr;
                              } break;
            case INS_JMP_IND: {  
                                  word addr = get_absolute_address(cycles, mem);
                                  addr = read_word(addr, cycles, mem);
                                  program_counter = addr;
                              } break;
//TSX
            case INS_TSX: {  
                              X = stack_pointer;
                              cycles--;
                              load_register_set_status(X);
                          } break;
            case INS_TXS: {  
                              stack_pointer = X;
                              cycles--;
                          } break;
//PHA
            case INS_PHA: { 
                              push_byte_onto_stack(A, cycles, mem);
                          } break;
//PHP
            case INS_PHP: { 
                              push_byte_onto_stack(ps, cycles, mem);
                          } break;
//PLA
            case INS_PLA: { 
                              A = pop_byte_from_stack(cycles, mem);
                              load_register_set_status(A);
                          } break;
//PLP
            case INS_PLP: { 
                              ps = pop_byte_from_stack(cycles, mem);
                          } break;
//AND
            case INS_AND_IM: {
                                 A &= fetch_byte(cycles, mem);
                                 load_register_set_status(A);
                             } break;
            case INS_AND_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 and_register(addr);
                             } break;
            case INS_AND_ZPX: {
                                  word addr = get_zero_page_x_address(cycles, mem);
                                  and_register(addr);
                              } break;
            case INS_AND_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  and_register(addr);
                              } break;
            case INS_AND_ABSX: {
                                   word addr = get_absolute_x_address(cycles, mem);
                                   and_register(addr);
                               } break;
            case INS_AND_ABSY: {
                                   word addr = get_absolute_y_address(cycles, mem);
                                   and_register(addr);
                               } break;
            case INS_AND_INDX: {
                                   word addr = get_indirect_x_address(cycles, mem);
                                   and_register(addr);
                               } break;
            case INS_AND_INDY: {
                                   word addr = get_indirect_y_address(cycles, mem);
                                   and_register(addr);
                               } break;
//EOR
            case INS_EOR_IM: {
                                 A ^= fetch_byte(cycles, mem);
                                 load_register_set_status(A);
                             } break;
            case INS_EOR_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 eor_register(addr);
                             } break;
            case INS_EOR_ZPX: {
                                  word addr = get_zero_page_x_address(cycles, mem);
                                  eor_register(addr);
                              } break;
            case INS_EOR_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  eor_register(addr);
                              } break;
            case INS_EOR_ABSX: {
                                   word addr = get_absolute_x_address(cycles, mem);
                                  eor_register(addr);
                               } break;
            case INS_EOR_ABSY: {
                                   word addr = get_absolute_y_address(cycles, mem);
                                  eor_register(addr);
                               } break;
            case INS_EOR_INDX: {
                                   word addr = get_indirect_x_address(cycles, mem);
                                  eor_register(addr);
                               } break;
            case INS_EOR_INDY: {
                                   word addr = get_indirect_y_address(cycles, mem);
                                  eor_register(addr);
                               } break;
//OR
            case INS_ORA_IM: {
                                 A |= fetch_byte(cycles, mem);
                                 load_register_set_status(A);
                             } break;
            case INS_ORA_ZP: {
                                 word addr = get_zero_page_address(cycles, mem);
                                 or_register(addr);
                             } break;
            case INS_ORA_ZPX: {
                                  word addr = get_zero_page_x_address(cycles, mem);
                                  or_register(addr);
                              } break;
            case INS_ORA_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  or_register(addr);
                              } break;
            case INS_ORA_ABSX: {
                                   word addr = get_absolute_x_address(cycles, mem);
                                  or_register(addr);
                               } break;
            case INS_ORA_ABSY: {
                                   word addr = get_absolute_y_address(cycles, mem);
                                  or_register(addr);
                               } break;
            case INS_ORA_INDX: {
                                   word addr = get_indirect_x_address(cycles, mem);
                                  or_register(addr);
                               } break;
            case INS_ORA_INDY: {
                                   word addr = get_indirect_y_address(cycles, mem);
                                  or_register(addr);
                               } break;
//BIT
            case INS_BIT_ZP: {
                                  word addr = get_zero_page_address(cycles, mem);
                                  byte value = read_byte(addr, cycles, mem);
                                  flag.Z = !(A & value);
                                  flag.N = (value & neg_flag_bit) != 0;
                                  flag.V = (value & ofl_flag_bit) != 0;
                              } break;
            case INS_BIT_ABS: {
                                  word addr = get_absolute_address(cycles, mem);
                                  byte value = read_byte(addr, cycles, mem);
                                  flag.Z = !(A & value);
                                  flag.N = (value & neg_flag_bit) != 0;
                                  flag.V = (value & ofl_flag_bit) != 0;
                              } break;
            default:
                               std::println("Instruction not handled: {}!", instruction);
                               throw -1;
                               break;
        }
    }
    return req_cycles - cycles;
}
