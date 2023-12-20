// /*
// MIT License

// Copyright (c) 2023 MegrajChauhan

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// */

#include "../Components/CPU/aspire_cpu.h"

// /* xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx */
// aStr_t _asp_get_cpu_error(_Asp_CPU_Errors err)
// {
//     switch(err)
//     {
//         case _ASP_CIREAD_ERR: // a fatal error, reading from instruction memory caused some error
//            return _asp_concat(_ASP_ERR_FATAL("Internal Error"), ": There has been some problem while reading instructions.\n");
//         case _ASP_CDREAD_ERR: // a fatal error, reading from data memory caused some error
//            return _asp_concat(_ASP_ERR_FATAL("Internal Error"), ": There has been some problem reading the data.\n");
//         case _ASP_CDWRITE_ERR:
//            return _asp_concat(_ASP_ERR_FATAL("Internal Error"), ": There has been some problem writing the data.\n");
//         case _ASP_CDIV_BY_ZERO_ERR:
//            return _asp_concat(_ASP_ERR_FATAL("Error"), ": Dividing by zero.\n");
//         case _ASP_CSTACK_OVERFLOW:
//            return _asp_concat(_ASP_ERR_FATAL("Error"), ": Stack Overflow\n");
//         case _ASP_CSTACK_UNDERFLOW:
//            return _asp_concat(_ASP_ERR_FATAL("Error"), ": Stack Underflow\n");
//     }
// }

// As there is no clear meaning as to wheather inc and dec are signed or unsigned, they will assume that it is signed and unsigned
// Whatever the user or the program may assume the numbers to be, the inc and dec instructions will set all flags for signed and unsigned assuming that it is a
// signed as well as unsigned
// if the program wants to act as if the operation was signed, then it can check the overflow and negative flag for jumps or move
// if the program wants to act as if the opeartion was unsigned, then it can check the carry flag
void _asp_inst_inc(_Asp_CPU *cpu, aQword inst)
{
    register aQword regr = inst & 15;
    register aQword temp = cpu->_asp_registers[regr];
    register aQword res = (cpu->_asp_registers[regr]++);
    cpu->flags.carry = res < temp ? 1 : 0; //[Assuming unsigned]if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63) == 1 ? 1 : 0;
    cpu->flags.overflow = (temp >> 63 == 0 && res >> 63 == 1) ? 1 : 0; //[Assuming signed] since we are adding +1, if the result is negative then, there is an overflow
}

void _asp_inst_dec(_Asp_CPU *cpu, aQword inst)
{
    register aQword regr = inst & 15;
    register aQword temp = cpu->_asp_registers[regr];
    register aQword res = (cpu->_asp_registers[regr]--); // subtract the number
    // decrement should never produce carry[if i am correct]
    cpu->flags.carry = 0;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63) == 1 ? 1 : 0;
    cpu->flags.overflow = (temp >> 63 == 1 && res >> 63 == 0) ? 1 : 0; // since we are subtracting +1, if the result is negative then, there is an overflow
}

int _asp_inst_pusha(_Asp_CPU *cpu)
{
    if (cpu->sp >= (_ASP_STACK_SIZE - 16))
    {
        // this applies that the stack is not able to hold these values
        return 1;
    }
    else
    {
        if (cpu->sp == 0)
            cpu->sp = -1;
        for (aSize_t i = 0; i < REGR_COUNT; i++)
        {
            cpu->sp++;
            cpu->stack[cpu->sp] = cpu->_asp_registers[i];
        }
    }
    return 0;
}

int _asp_inst_popa(_Asp_CPU *cpu)
{
    if (cpu->sp < 15) // we have to take the base pointer into consideration as well
    {
        // this applies that the stack is not able to hold these values
        return _INTR_STACK_UNDERFLOW;
    }
    else if (cpu->bp > cpu->sp || (cpu->sp - cpu->bp) < 15)
    {
        // this implies that we are trying to pop off the values pushed to stack for a different function
        return _INTR_STACK_INVALID_ACCESS;
    }
    else
    {
        for (aSize_t i = 0; i < REGR_COUNT; i++)
        {
            cpu->_asp_registers[i] = cpu->stack[cpu->sp];
            cpu->sp--;
        }
    }
    return 0;
}

int _asp_inst_call(_Asp_CPU *cpu)
{
    if (cpu->sp >= (_ASP_STACK_SIZE - 18))
    {
        // this applies that the stack is not able to hold these values
        return 1;
    }
    else
    {
        cpu->sp += (cpu->sp == 0) ? 0 : 1;
        cpu->stack[cpu->sp] = cpu->pc; // we save the PC and set the BP
        cpu->sp++;
        cpu->stack[cpu->sp] = cpu->bp; // save the current position of the base pointer
        cpu->bp = cpu->sp;             // create a new frame
    }
    return 0;
}

int _asp_inst_ret(_Asp_CPU *cpu)
{
    // do the same thing as the popa instruction
    if (cpu->sp < 2) // for ret instruction to work, there must be at least 2 values(one for the pc and one for the bc's old position on the stack)
    {
        return 1;
    }
    else
    {
        // stack pointer points two address back the bp's current position
        cpu->sp = cpu->bp - 2;
        // restore the pc
        cpu->pc = cpu->stack[cpu->bp - 1];
        // restore the bp
        cpu->bp = cpu->stack[cpu->bp];
    }
    return 0;
}

int _asp_inst_sba(_Asp_CPU *cpu)
{
    register aQword offset = (cpu->ir >> 8) & 0x7FFF; // get the offset[since there are only 32768 addresses]
    register aQword pos = cpu->bp + offset;
    if (pos > cpu->sp)
    {
        // this means error
        return 1;
    }
    cpu->_asp_registers[cpu->ir & 15] = cpu->stack[pos];
    return 0;
}

// Just the same function copied with little changes

void _asp_inst_addc_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val + imm + ((cpu->flags.carry == 1) ? 1 : 0);
    cpu->flags.carry = res < dest_regr_val ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_addc_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val + src_regr_val + ((cpu->flags.carry == 1) ? 1 : 0);
    cpu->flags.carry = res < dest_regr_val ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_add_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val + imm;
    cpu->flags.carry = res < dest_regr_val ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_add_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val + src_regr_val;
    cpu->flags.carry = res < dest_regr_val ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_sub_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val - imm;
    cpu->flags.carry = dest_regr_val < imm ? 0 : 1; // here zero would mean there was a carry
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_sub_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val - src_regr_val;
    cpu->flags.carry = dest_regr_val < src_regr_val ? 0 : 1;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_mul_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val * imm;
    cpu->flags.carry = res < imm || res < dest_regr_val ? 1 : 0; // here zero would mean there was a carry
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_mul_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val * src_regr_val;
    cpu->flags.carry = res < dest_regr_val || res < src_regr_val ? 1 : 0;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
}

int _asp_inst_div_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    if (imm == 0)
        return 1;                              // divide by zero
    register aQword res = dest_regr_val / imm; // discard the remainder
    // cpu->flags.carry = res < imm || res < dest_regr_val ? 1 : 0; // I believe that division doesn't cause carry{tell me if i am wrong}
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

int _asp_inst_div_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    if (src_regr_val == 0)
        return 1;
    register aQword res = dest_regr_val / src_regr_val;
    // cpu->flags.carry = res < dest_regr_val || res < src_regr_val ? 1 : 0;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

int _asp_inst_mod_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    if (imm == 0)
        return 1;                              // what?!!
    register aQword res = dest_regr_val % imm; // use the remainder
    // cpu->flags.carry = res < imm || res < dest_regr_val ? 1 : 0; // I believe that modulos also doesn't cause carry{tell me if i am wrong}
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

int _asp_inst_mod_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    if (src_regr_val == 0)
        return 1;
    register aQword res = dest_regr_val % src_regr_val;
    // cpu->flags.carry = res < dest_regr_val || res < src_regr_val ? 1 : 0;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

void _asp_inst_iaddc_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val + imm + ((cpu->flags.carry == 1) ? 1 : 0);
    cpu->flags.overflow = ((imm >> 63) == (dest_regr_val >> 63)) != (res >> 63) ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63);
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_iaddc_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val + src_regr_val + ((cpu->flags.carry == 1) ? 1 : 0);
    cpu->flags.overflow = ((src_regr_val >> 63) == (dest_regr_val >> 63)) != (res >> 63) ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63);
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_iadd_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val + imm;
    cpu->flags.overflow = ((imm >> 63) == (dest_regr_val >> 63)) != (res >> 63) ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63);
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_iadd_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val + src_regr_val;
    cpu->flags.overflow = ((src_regr_val >> 63) == (dest_regr_val >> 63)) != (res >> 63) ? 1 : 0; // if there was an overflow, the result will wrap resulting in a smaller number than the starting value
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63);
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_isub_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val - imm;
    cpu->flags.overflow = (dest_regr_val >> 63) != ((imm >> 63) == (res >> 63)) ? 1 : 0;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63);
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_isub_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val - src_regr_val;
    cpu->flags.overflow = (dest_regr_val >> 63) != ((src_regr_val >> 63) == (res >> 63)) ? 1 : 0;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = (res >> 63);
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_imul_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    register aQword res = dest_regr_val * imm;
    register int imm_sign = imm >> 63;
    register int res_sign = res >> 63;
    register int dest_val_sign = dest_regr_val >> 63;
    cpu->flags.overflow = ((imm_sign == dest_val_sign) && (imm_sign == 0)) && res_sign == 0 ? 0 : ((imm_sign == dest_val_sign) && (imm_sign == 1)) && res_sign == 0 ? 0
                                                                                              : (imm_sign != dest_val_sign) && res_sign == 1                        ? 0
                                                                                                                                                                    : 1;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = res_sign;
    cpu->_asp_registers[dest_regr] = res;
}

void _asp_inst_imul_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    register aQword res = dest_regr_val * src_regr_val;
    register int src_val_sign = src_regr_val >> 63;
    register int res_sign = res >> 63;
    register int dest_val_sign = dest_regr_val >> 63;
    cpu->flags.overflow = ((src_val_sign == dest_val_sign) && (dest_val_sign == 0)) && res_sign == 0 ? 0 : ((src_val_sign == dest_val_sign) && (src_val_sign == 1)) && res_sign == 0 ? 0
                                                                                                       : (src_val_sign != dest_val_sign) && res_sign == 1                            ? 0
                                                                                                                                                                                     : 1;
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->flags.negative = res_sign;
    cpu->_asp_registers[dest_regr] = res;
}

int _asp_inst_idiv_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    /*
      Following steps have been taken:
      Extracted the operands, checked their sign, if negative converted to +ve, performed division, used sign to get the correct result and set the correct flags
    */
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    if (imm == 0)
        return 1; // divide by zero
    register aQword imm_sign = imm >> 63;
    register aQword dest_regr_sign = dest_regr_val >> 63;
    register aQword res = ((dest_regr_sign == 1) ? _ASP_REVERSE_COMPLEMENT(dest_regr_val) : dest_regr_val) / ((imm == 1) ? _ASP_REVERSE_COMPLEMENT(imm) : imm); // discard the remainder
    // // cpu->flags.carry = res < imm || res < dest_regr_val ? 1 : 0; // I believe that division doesn't cause carry{tell me if i am wrong}
    if ((imm_sign == 1 && dest_regr_sign == 0) || (imm_sign == 0 && dest_regr_sign == 1))
    {
        cpu->flags.negative = 1;
        res = _ASP_TWO_COMPLEMENT(res);
    }
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

int _asp_inst_idiv_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    // same logic as above
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    if (src_regr_val == 0)
        return 1;
    register aQword src_regr_sign = src_regr_val >> 63;
    register aQword dest_regr_sign = dest_regr_val >> 63;
    register aQword res = ((dest_regr_sign == 1) ? _ASP_REVERSE_COMPLEMENT(dest_regr_val) : dest_regr_val) / ((src_regr_sign == 1) ? _ASP_REVERSE_COMPLEMENT(src_regr_sign) : src_regr_sign);
    // cpu->flags.carry = res < dest_regr_val || res < src_regr_val ? 1 : 0;
    if ((src_regr_sign == 1 && dest_regr_sign == 0) || (src_regr_sign == 0 && dest_regr_sign == 1))
    {
        cpu->flags.negative = 1;
        res = _ASP_TWO_COMPLEMENT(res);
    }
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

int _asp_inst_imod_imm_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = inst & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword imm = (inst >> 8) & 0xFFFFFFFFFFFF;
    if (imm == 0)
        return 1; // divide by zero
    register aQword imm_sign = imm >> 63;
    register aQword dest_regr_sign = dest_regr_val >> 63;
    register aQword res = ((dest_regr_sign == 1) ? _ASP_REVERSE_COMPLEMENT(dest_regr_val) : dest_regr_val) % ((imm == 1) ? _ASP_REVERSE_COMPLEMENT(imm) : imm); // discard the remainder
    if ((imm_sign == 1 && dest_regr_sign == 0) || (imm_sign == 0 && dest_regr_sign == 1))
    {
        cpu->flags.negative = 1;
        res = _ASP_TWO_COMPLEMENT(res);
    }
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

int _asp_inst_imod_reg_reg(_Asp_CPU *cpu, aQword inst)
{
    register aQword dest_regr = (inst >> 4) & 15;
    register aQword src_regr = (inst) & 15;
    register aQword dest_regr_val = cpu->_asp_registers[dest_regr];
    register aQword src_regr_val = cpu->_asp_registers[src_regr];
    if (src_regr_val == 0)
        return 1;
    register aQword src_regr_sign = src_regr_val >> 63;
    register aQword dest_regr_sign = dest_regr_val >> 63;
    register aQword res = ((dest_regr_sign == 1) ? _ASP_REVERSE_COMPLEMENT(dest_regr_val) : dest_regr_val) % ((src_regr_sign == 1) ? _ASP_REVERSE_COMPLEMENT(src_regr_sign) : src_regr_sign);
    // cpu->flags.carry = res < dest_regr_val || res < src_regr_val ? 1 : 0;
    if ((src_regr_sign == 1 && dest_regr_sign == 0) || (src_regr_sign == 0 && dest_regr_sign == 1))
    {
        cpu->flags.negative = 1;
        res = _ASP_TWO_COMPLEMENT(res);
    }
    cpu->flags.zero = res == 0 ? 1 : 0;
    cpu->_asp_registers[dest_regr] = res;
    return 0;
}

_Asp_CPU *_asp_start_cpu(_Asp_Memory *mem, _Asp_Inst_Mem *imem, _Asp_Intr_Hdlr *hdlr)
{
    _Asp_CPU *cpu = (_Asp_CPU *)malloc(sizeof(_Asp_CPU));
    if (cpu == NULL)
        return NULL;
    cpu->cid = 0; // we do not know yet[The manager will add it during boot up of this core]
    // The condition variable is really only needed once we make it possible for one core to halt or stop another
    cpu->hdlr = hdlr;
    cpu->imem = imem;
    cpu->memory = mem;
    cpu->lock = _asp_mutex_init(); // this is needed by the manager
    if (cpu->lock == NULL)
    {
        free(cpu);
        return NULL;
    }
    cpu->pc = 0; // start from 0
    cpu->stack = (aQptr_t)malloc(sizeof(aQword) * _ASP_STACK_SIZE);
    if (cpu->stack == NULL)
    {
        _asp_mutex_destroy(cpu->lock);
        free(cpu);
        return NULL;
    }
    return cpu;
}

void _asp_stop_cpu(_Asp_CPU *cpu)
{
    if (cpu == NULL)
        return;
    if (cpu->cond != NULL)
        _asp_cond_destroy(cpu->cond);
    if (cpu->lock != NULL)
        _asp_mutex_destroy(cpu->lock);
    if (cpu->stack != NULL)
        free(cpu->stack);
    // other things can clean themselves up
    cpu->hdlr = NULL;
    cpu->imem = NULL;
    cpu->memory = NULL;
    free(cpu);
}

void *_asp_start_execution(void *cpu)
{
    _Asp_CPU *core = (_Asp_CPU *)cpu;
    register aQptr_t pc = &core->pc;
    register aQptr_t sp = &core->sp;
    register aQptr_t ir = &core->ir;
    *ir = 0;
    register int ret = 0;
    while (aTrue)
    {
        _asp_mutex_lock(core->lock);
        if (core->running == aTrue)
        {
            if ((_asp_inst_read(core->imem, *pc, ir)) == aFalse)
            {
                _asp_handle_interrupt(core->hdlr, core->imem->err, core->cid); // we are just telling the manager to stop the VM
            }
            else
            {
                switch (_ASP_EXTRACT_INST(*ir))
                {
                // we are currently assuming that we will only have 255 instructions, we might change that with a lot more number of instructions
                case ASP_NOP:
                    break;
                case ASP_HALT:
                    _asp_handle_interrupt(core->hdlr, _INTR_STOP_CPU, core->cid); // by default the halt instruction generates this interrupt
                    break;
                case ASP_MOVE_IMM:
                    core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVE_REG:
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVE_REG8:
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFF;
                    break;
                case ASP_MOVE_REG16:
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFFFF;
                    break;
                case ASP_MOVE_REG32:
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFFFFFF;
                    break;
                case ASP_MOVEZX_IMM:
                    // this is the same as the simple move but with an extra AND operation added
                    core->_asp_registers[*ir & 15] = ((*ir >> 8) & 0xFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF;
                    break;
                case ASP_MOVEZX_REG:
                    // this is the same as the simple move but with an extra AND operation added
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFFFFFFFFFFFFFFFF;
                    break;
                case ASP_MOVEZX_REG8:
                    // this is the same as the simple move but with an extra AND operation added
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFF;
                    break;
                case ASP_MOVEZX_REG16:
                    // this is the same as the simple move but with an extra AND operation added
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFFFF;
                    break;
                case ASP_MOVEZX_REG32:
                    // this is the same as the simple move but with an extra AND operation added
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15] & 0xFFFFFF;
                    break;
                case ASP_MOVESX_IMM:
                    // the instruction can take 4 bytes long numbers. The 0th(31th from the opposite) is the sign for this number which is extended
                    core->_asp_registers[(*ir) & 15] = _asp_sign_ext((*ir >> 8) & 0xFFFFFFFF, 31);
                    break;
                case ASP_MOVESX_REG:
                    // we don't need to do anything here
                    core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVESX_REG8:
                    // we don't need to do anything here
                    core->_asp_registers[(*ir >> 8) & 15] = _asp_sign_ext(core->_asp_registers[(*ir) & 15] & 0xFF, 7);
                    break;
                case ASP_MOVESX_REG16:
                    // we don't need to do anything here
                    core->_asp_registers[(*ir >> 8) & 15] = _asp_sign_ext(core->_asp_registers[(*ir) & 15] & 0xFFFF, 7);
                    break;
                case ASP_MOVESX_REG32:
                    // we don't need to do anything here
                    core->_asp_registers[(*ir >> 8) & 15] = _asp_sign_ext(core->_asp_registers[(*ir) & 15] & 0xFFFF, 7);
                    break;
                // these are conditional moves. They perform move only when the given flag is set
                case ASP_MOVEZ_IMM:
                    if (core->flags.zero == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEZ_REG:
                    if (core->flags.zero == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVENZ_IMM:
                    if (core->flags.zero == 0)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVENZ_REG:
                    if (core->flags.zero == 0)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVEE_IMM:
                    if (core->flags.equal == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEE_REG:
                    if (core->flags.equal == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVENE_IMM:
                    if (core->flags.equal == 0)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVENE_REG:
                    if (core->flags.equal == 0)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVEG_IMM:
                    if (core->flags.greater == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEG_REG:
                    if (core->flags.greater == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVES_IMM:
                    if (core->flags.greater == 0)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVES_REG:
                    if (core->flags.greater == 0)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVEO_IMM:
                    if (core->flags.overflow == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEO_REG:
                    if (core->flags.overflow == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVENO_IMM:
                    if (core->flags.overflow == 0)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVENO_REG:
                    if (core->flags.overflow == 0)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVEN_IMM:
                    if (core->flags.negative == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEN_REG:
                    if (core->flags.negative == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVENN_IMM:
                    if (core->flags.negative == 0)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVENN_REG:
                    if (core->flags.negative == 0)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVEGE_IMM:
                    if (core->flags.greater == 1 || core->flags.equal == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEGE_REG:
                    if (core->flags.greater == 1 || core->flags.equal == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVESE_IMM:
                    if (core->flags.greater == 0 || core->flags.equal == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVESE_REG:
                    if (core->flags.greater == 0 || core->flags.equal == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVEC_IMM:
                    if (core->flags.carry == 1)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVEC_REG:
                    if (core->flags.carry == 1)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_MOVENC_IMM:
                    if (core->flags.carry == 0)
                        core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    break;
                case ASP_MOVENC_REG:
                    if (core->flags.carry == 0)
                        core->_asp_registers[(*ir >> 8) & 15] = core->_asp_registers[(*ir) & 15];
                    break;
                case ASP_EXCG8:
                { // exchange the 8 bits of of register to the 8 bits of another
                    register aQword byte1 = core->_asp_registers[(*ir >> 8) & 15] & 0XFF;
                    register aQword byte2 = core->_asp_registers[*ir & 15] & 0xFF;
                    core->_asp_registers[(*ir >> 8) & 15] &= (0xFFFFFFFFFFFFFF00 | byte2);
                    core->_asp_registers[(*ir) & 15] &= (0xFFFFFFFFFFFFFF00 | byte1);
                    break;
                }
                case ASP_EXCG16:
                { // exchange the 16 bits of of register to the 8 bits of another
                    register aQword bytes1 = core->_asp_registers[(*ir >> 8) & 15] & 0XFFFF;
                    register aQword bytes2 = core->_asp_registers[*ir & 15] & 0xFFFF;
                    core->_asp_registers[(*ir >> 8) & 15] &= (0xFFFFFFFFFFFF0000 | bytes2);
                    core->_asp_registers[(*ir) & 15] &= (0xFFFFFFFFFFFF0000 | bytes1);
                    break;
                }
                case ASP_EXCG32:
                { // exchange the 32 bits of of register to the 8 bits of another
                    register aQword bytes1 = core->_asp_registers[(*ir >> 8) & 15] & 0XFFFFFFFF;
                    register aQword bytes2 = core->_asp_registers[*ir & 15] & 0xFFFFFFFF;
                    core->_asp_registers[(*ir >> 8) & 15] &= (0xFFFFFFFF00000000 | bytes2);
                    core->_asp_registers[(*ir) & 15] &= (0xFFFFFFFF00000000 | bytes1);
                    break;
                }
                case ASP_EXCG:
                { // exchange the 64 bits of of register to the 8 bits of another
                    register aQword bytes1 = core->_asp_registers[(*ir >> 8) & 15];
                    register aQword bytes2 = core->_asp_registers[*ir & 15];
                    core->_asp_registers[(*ir >> 8) & 15] = bytes2;
                    core->_asp_registers[(*ir) & 15] = bytes1;
                    break;
                }
                case ASP_CMP_IMM_REG:
                {
                    register aQword imm = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    register aQword value = core->_asp_registers[*ir & 15];
                    core->flags.equal = (imm == value) ? 1 : 0;
                    core->flags.greater = (value > imm) ? 1 : 0;
                    break;
                }
                case ASP_CMP_REG_REG:
                {
                    register aQword reg1 = core->_asp_registers[(*ir >> 8)];
                    register aQword reg2 = core->_asp_registers[*ir & 15];
                    core->flags.equal = (reg1 == reg2) ? 1 : 0;
                    core->flags.greater = (reg1 > reg2) ? 1 : 0;
                    break;
                }
                case ASP_JMP:
                    // the core blindly jumps to any address provided, so, be careful
                    *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JZ:
                    if (core->flags.zero == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JNZ:
                    if (core->flags.zero == 0)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JE:
                    if (core->flags.equal == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JNE:
                    if (core->flags.equal == 0)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JG:
                    if (core->flags.greater == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JS:
                    if (core->flags.greater == 0)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JO:
                    if (core->flags.overflow == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JNO:
                    if (core->flags.overflow == 0)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JN:
                    if (core->flags.negative == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JNN:
                    if (core->flags.negative == 0)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JGE:
                    if (core->flags.greater == 1 || core->flags.equal == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_JSE:
                    if (core->flags.greater == 0 || core->flags.equal == 1)
                        *pc = *ir & 0x00FFFFFFFFFFFFFF;
                    break;
                case ASP_AND_IMM_REG:
                {
                    register aQword imm = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    core->_asp_registers[*ir & 15] &= imm;
                    break; // simple
                }
                case ASP_AND_REG_REG:
                    core->_asp_registers[(*ir >> 8) & 15] &= core->_asp_registers[(*ir & 15)];
                    break;
                case ASP_OR_IMM_REG:
                {
                    register aQword imm = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    core->_asp_registers[*ir & 15] |= imm;
                    break;
                }
                case ASP_OR_REG_REG:
                    core->_asp_registers[(*ir >> 8) & 15] |= core->_asp_registers[(*ir & 15)];
                    break;
                case ASP_XOR_IMM_REG:
                {
                    register aQword imm = (*ir >> 8) & 0xFFFFFFFFFFFF;
                    core->_asp_registers[*ir & 15] ^= imm;
                    break;
                }
                case ASP_XOR_REG_REG:
                    core->_asp_registers[(*ir >> 8) & 15] ^= core->_asp_registers[(*ir & 15)];
                    break;
                case ASP_NOT:
                    core->_asp_registers[*ir & 15] = ~(core->_asp_registers[*ir & 15]);
                    break;
                case ASP_LSHIFT:
                    core->_asp_registers[*ir & 15] >>= (((*ir >> 8) & 0x3F) + 1);
                    break;
                case ASP_RSHIFT:
                    core->_asp_registers[*ir & 15] <<= (((*ir >> 8) & 0x3F) + 1);
                    break;
                case ASP_INC:
                    _asp_inst_inc(core, *ir);
                    break;
                case ASP_DEC:
                    _asp_inst_dec(core, *ir);
                    break;
                case ASP_RESET:
                    _asp_reset_all(core);
                    break;
                case ASP_CFLAGS:
                    _asp_reset_flags(core);
                    break;
                case ASP_CLZ:
                    core->flags.zero = 0;
                    break;
                case ASP_CLN:
                    core->flags.negative = 0;
                    break;
                case ASP_CLE:
                    core->flags.equal = 0;
                    break;
                case ASP_CLG:
                    core->flags.greater = 0;
                    break;
                case ASP_CLO:
                    core->flags.overflow = 0;
                    break;
                case ASP_CLC:
                    core->flags.carry = 0;
                    break;
                // when it comes to memory related instructions, the instruction memory is unaccessible to anything but the cores and the Manager
                // No instruction can access it.
                case ASP_LOAD:
                    // load instruction loads a value from memory to a given register
                    // the address can be as long as 5 bytes or 40 bits which should be more than enough
                    if (_asp_memory_read(core->memory, (*ir >> 8) & 0xFFFFFFFFFF, &core->_asp_registers[*ir & 15]) == aFalse)
                    {
                        // if we get an error, we have to report to the interrupt handler
                        // we have yet to make the interrupt handler capable of handling memory related errors and so we will simply send in a divide by zero error
                        _asp_handle_interrupt(core->hdlr, core->memory->err, core->cid);
                    }
                    break; // if we don't fail the result of the read should be in the register
                case ASP_STORE:
                    // store instruction stores a value to memory from a given register
                    // the address can be as long as 5 bytes or 40 bits which should be more than enough
                    if (_asp_memory_write(core->memory, (*ir >> 8) & 0xFFFFFFFFFF, core->_asp_registers[*ir & 15]) == aFalse)
                    {
                        // we don't handle memory interrupts just yet so
                        _asp_handle_interrupt(core->hdlr, core->memory->err, core->cid);
                    }
                    break; // if we don't fail the value should be written
                case ASP_LEA:
                    core->_asp_registers[*ir & 15] = (*ir >> 8) & 0xFFFFFFFFFF;
                    break;
                case ASP_LEA_OFF:
                    core->_asp_registers[*ir & 15] = core->_asp_registers[(*ir >> 4) & 15] + ((*ir >> 8) & 0xFFFFFFFFFF);
                    break;
                case ASP_LEA_IND:
                    core->_asp_registers[*ir & 15] = core->_asp_registers[(*ir >> 4) & 15] + ((*ir >> 8) & 0xFFFFFFFFFFFF) * 8;
                    break;
                case ASP_PUSH_IMM:
                    // push an immediate to the stack
                    if (*sp >= _ASP_STACK_SIZE)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_OVERFLOW, core->cid);
                    else
                    {
                        *sp += (*sp == 0) ? 1 : 0;
                        core->stack[*sp] = *ir & 0xFFFFFFFFFFFFFF; // 7 bytes of immediate!
                    }
                    break;
                case ASP_PUSH_REG:
                    // same as push imm but for a register instead
                    if (*sp >= _ASP_STACK_SIZE)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_OVERFLOW, core->cid);
                    else
                    {
                        (*sp)++;
                        core->stack[*sp] = core->_asp_registers[*ir & 15];
                    }
                    break;
                case ASP_POP:
                    if (*sp == 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_UNDERFLOW, core->cid);
                    else
                    {
                        core->_asp_registers[*ir & 15] = core->stack[*sp];
                        (*sp)--;
                    }
                    break;
                case ASP_PUSHA:
                    // we push Aa register and go through upto Ax5 and popped in opposite order
                    if (_asp_inst_pusha(core) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_OVERFLOW, core->cid);
                    break;
                case ASP_POPA:
                    // we push Aa register and go through upto Ax5 and popped in opposite order
                    if ((ret = _asp_inst_popa(core)) != 0)
                        _asp_handle_interrupt(core->hdlr, ret, core->cid);
                    break;
                case ASP_CALL:
                    if (_asp_inst_call(core) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_OVERFLOW, core->cid);
                    break;
                case ASP_RET:
                    if (_asp_inst_ret(core) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_UNDERFLOW, core->cid);
                    break;
                case ASP_SBA:
                    // this is simple move instruction but it moves a value from stack to the given address
                    if (_asp_inst_sba(core) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_STACK_INVALID_ADDRESSING, core->cid);
                    break;
                case ASP_ADDC_IMM_REG:
                    _asp_inst_addc_imm_reg(core, *ir);
                    break;
                case ASP_ADDC_REG_REG:
                    _asp_inst_addc_reg_reg(core, *ir);
                    break;
                case ASP_ADD_IMM_REG:
                    _asp_inst_add_imm_reg(core, *ir);
                    break;
                case ASP_ADD_REG_REG:
                    _asp_inst_add_reg_reg(core, *ir);
                    break;
                case ASP_SUB_IMM_REG:
                    _asp_inst_sub_imm_reg(core, *ir);
                    break;
                case ASP_SUB_REG_REG:
                    _asp_inst_sub_reg_reg(core, *ir);
                    break;
                case ASP_MUL_IMM_REG:
                    _asp_inst_mul_imm_reg(core, *ir);
                    break;
                case ASP_MUL_REG_REG:
                    _asp_inst_mul_reg_reg(core, *ir);
                    break;
                case ASP_DIV_IMM_REG:
                    if (_asp_inst_div_imm_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid); // now this seriously means divide by 0 error
                    break;
                case ASP_DIV_REG_REG:
                    if (_asp_inst_div_reg_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid);
                    break;
                case ASP_MOD_IMM_REG:
                    if (_asp_inst_mod_imm_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid); // now this also seriously means divide by 0 error
                    break;
                case ASP_MOD_REG_REG:
                    if (_asp_inst_mod_reg_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid);
                    break;
                case ASP_IADDC_IMM_REG:
                    _asp_inst_iaddc_imm_reg(core, *ir);
                    break;
                case ASP_IADDC_REG_REG:
                    _asp_inst_iaddc_reg_reg(core, *ir);
                    break;
                case ASP_IADD_IMM_REG:
                    _asp_inst_iadd_imm_reg(core, *ir);
                    break;
                case ASP_IADD_REG_REG:
                    _asp_inst_iadd_reg_reg(core, *ir);
                    break;
                case ASP_ISUB_IMM_REG:
                    _asp_inst_isub_imm_reg(core, *ir);
                    break;
                case ASP_ISUB_REG_REG:
                    _asp_inst_isub_reg_reg(core, *ir);
                    break;
                case ASP_IMUL_IMM_REG:
                    _asp_inst_imul_imm_reg(core, *ir);
                    break;
                case ASP_IMUL_REG_REG:
                    _asp_inst_imul_reg_reg(core, *ir);
                    break;
                case ASP_IDIV_IMM_REG:
                    if (_asp_inst_idiv_imm_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid); // now this seriously means divide by 0 error
                    break;
                case ASP_IDIV_REG_REG:
                    if (_asp_inst_idiv_reg_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid);
                    break;
                case ASP_IMOD_IMM_REG:
                    if (_asp_inst_imod_imm_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid); // now this also seriously means divide by 0 error
                    break;
                case ASP_IMOD_REG_REG:
                    if (_asp_inst_imod_reg_reg(core, *ir) != 0)
                        _asp_handle_interrupt(core->hdlr, _INTR_DIVIDE_BY_ZERO, core->cid);
                    break;
                case ASP_INTR:
                    // the last byte will hold the interrupt number
                    _asp_handle_interrupt(core->hdlr, *ir & 255, core->cid);
                    break;
                }

                (*pc)++;
            }
        }
        else
        {
            break;
        }
        _asp_mutex_unlock(core->lock);
    }
    return NULL;
}

/*
 0 -> 12
 1 -> 01 -> pc
 2 -> 00 -> bc
 3 -> 45
 4 -> 89
 5 -> 22
 6 -> [pc]
 7 -> 02 -> bc's old position[bc now points here]
 8 ->
 9 ->

 Program running....
 push 12
 call x [push the pc and the current position of bc. bc then points to the it's old position]
 x:
  push 45 [accessing these values requires using the base pointer]
  push 89
  push 22
  call p [push the pc and save the current position of bc again]
p:
 // do something
  ret[pop value upto bc, restore bp and pc]
*/
