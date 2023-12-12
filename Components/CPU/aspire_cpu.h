/*
MIT License

Copyright (c) 2023 MegrajChauhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ASPIRE_CPU
#define ASPIRE_CPU

#include "../Memory/aspire_memory.h"
#include "../Memory/aspire_instruction_memory.h"
#include "../../Utils/aspire_config.h"
#include "../../Utils/aspire_helpers.h"

/*
The CPU is going to be big endian. If need be, there will or might be a way for changing the endianness
The Memory, the Cache will follow whatever endianness the CPU follows
*/

/*
 The way as to how instructions are laid out in the memory.
 The memory is loaded with instructions to execute.
 Then the cache is loaded with specific length of instructions from certain pages.
*/

#define _ASP_EXTRACT_INST(x) x >> 56
/*#define _ASP_TCOND_CALL(cond, func, arg1, arg2) \
    if (cond == 1)                              \
    func(arg1, arg2)
#define _ASP_FCOND_CALL(cond, func, arg1, arg2) \
    if (cond == 0)                              \
    func(arg1, arg2)

#define _ASP_UNIVERSAL_ZERO                   \
    case 0:                                   \
    {                                         \
        pos = (inst >> 8) & 15;               \
        op1 = cpu->_asp_registers[pos];       \
        op2 = cpu->_asp_registers[inst & 15]; \
        break;                                \
    }

#define _ASP_UNIVERSAL_ONE                \
    case 1:                               \
    {                                     \
        pos = inst & 15;                  \
        op1 = cpu->_asp_registers[pos];   \
        op2 = (inst >> 8) & 0xFFFFFFFFFF; \
        break;                            \
    }

#define _ASP_ERR_WAR(msg) _asp_concat(_asp_concat(_ASP_CCODE_RED,  msg), _ASP_CCODE_RESET)*/

typedef struct _Asp_Flags
{
    unsigned zero : 1;
    unsigned equal : 1;
    unsigned greater : 1;
    unsigned overflow : 1; // CARRY flag for signed arithmetic
    unsigned carry : 1;    // CARRY flag for unsigned arithmetic
    unsigned negative : 1;
    unsigned res1 : 1; // reserved
    unsigned res2 : 1; // reserved
} _Asp_Flags;

enum _Asp_Instrs
{
    ASP_NOP,  // the nop instructon
    ASP_HALT, // halt the CPU from what it's doing

    // The move instructions have four modes for movement between registers but the given size for immediates is fixed.

    // first some basic insructions
    ASP_MOVE_IMM,   // move instruction[an immediate to a register]
    ASP_MOVE_REG,   // move instruction[a register value to another register]
    ASP_MOVE_REG8,  // move instruction[a register value to another register]
    ASP_MOVE_REG16, // move instruction[a register value to another register]
    ASP_MOVE_REG32, // move instruction[a register value to another register]

    ASP_MOVEZX_IMM,   // move with zero extension to the value[Same as ASP_MOVE_IMM]
    ASP_MOVEZX_REG,   // move with zero extension to the value[Same as ASP_MOVE_REG]
    ASP_MOVEZX_REG8,  // move with zero extension to the value[Same as ASP_MOVE_REG]
    ASP_MOVEZX_REG16, // move with zero extension to the value[Same as ASP_MOVE_REG]
    ASP_MOVEZX_REG32, // move with zero extension to the value[Same as ASP_MOVE_REG]

    ASP_MOVESX_IMM,   // move with sign extension
    ASP_MOVESX_REG,   // move with sign extension
    ASP_MOVESX_REG8,  // move with sign extension
    ASP_MOVESX_REG16, // move with sign extension
    ASP_MOVESX_REG32, // move with sign extension

    // conditional moves don't have the same variations like the unconditional moves
    // conditional moves
    ASP_MOVEZ_IMM, // move if zero flag is set
    ASP_MOVEZ_REG, // move if zero flag is set

    ASP_MOVENZ_IMM, // move if zero flag is not set
    ASP_MOVENZ_REG, // move if zero flag is not set

    ASP_MOVEE_IMM, // move if equal flag is set
    ASP_MOVEE_REG, // move if equal flag is set

    ASP_MOVENE_IMM, // move if equal flag is not set
    ASP_MOVENE_REG, // move if equal flag is not set

    ASP_MOVEG_IMM, // move if greater flag is set
    ASP_MOVEG_REG, // move if greater flag is set

    ASP_MOVES_IMM, // move if greater flag is not set
    ASP_MOVES_REG, // move if greater flag is not set

    ASP_MOVEO_IMM, // move if overflow flag is set
    ASP_MOVEO_REG, // move if overflow flag is set

    ASP_MOVENO_IMM, // move if overflow flag is not set
    ASP_MOVENO_REG, // move if overflow flag is not set

    ASP_MOVEN_IMM, // move if negative flag is set
    ASP_MOVEN_REG, // move if negative flag is set

    ASP_MOVENN_IMM, // move if negative flag is not set
    ASP_MOVENN_REG, // move if negative flag is not set

    ASP_MOVEGE_IMM, // move if greater or equal flag is set
    ASP_MOVEGE_REG, // move if greater or equal flag is set

    ASP_MOVESE_IMM, // move if greater is not set or equal flag is set
    ASP_MOVESE_REG, // move if greater is not set or equal flag is set

    ASP_MOVEC_IMM, // move if carry is set
    ASP_MOVEC_REG, // move if carry is set

    ASP_MOVENC_IMM, // move if carry is not set
    ASP_MOVENC_REG, // move if carry is not set

    ASP_EXCG8,  // exchange instruction that exchanges the values between two registers[8 bits]
    ASP_EXCG16, // exchange instruction that exchanges the values between two registers[16 bits]
    ASP_EXCG32, // exchange instruction that exchanges the values between two registers[32 bits]
    ASP_EXCG,   // exchange instruction that exchanges the values between two registers[64 bits]

    // comparison and jump instructions
    ASP_CMP_IMM_REG, // this may not work on floating point numbers but that is a problem for when floating point is implemented
    ASP_CMP_REG_REG,
    ASP_JMP,

    // conditional jumps
    ASP_JZ,  // jump if zero
    ASP_JNZ, // jump if not zero
    ASP_JE,  // jump if equal
    ASP_JNE, // jump if not equal
    ASP_JG,  // jump if greater
    ASP_JS,  // jump if not greater
    ASP_JO,  // jump if overflow
    ASP_JNO, // jump if no overflow
    ASP_JN,  // jump if negative
    ASP_JNN, // jump if no negative
    ASP_JGE, // jump if greater or equal
    ASP_JSE, // jump if smaller or equal

    // logical instructions
    ASP_AND_IMM_REG, // logical AND instruction
    ASP_AND_REG_REG, // logical AND instruction

    ASP_OR_IMM_REG, // logical OR instruction
    ASP_OR_REG_REG, // logical OR instruction

    ASP_XOR_IMM_REG, // logical XOR instruction
    ASP_XOR_REG_REG, // logical XOR instruction

    ASP_NOT, // logical NOT instruction

    // these don't get their own variants
    ASP_LSHIFT, // bit-shift left instruction
    ASP_RSHIFT, // bit-shift right instruction

    ASP_INC, // increment instruction[floats will need their own]
    ASP_DEC, // decrement instruction[floats will need their own]

    ASP_RESET, // the reset instruction[resets all the registers except program counter, bp, sp to zero. resets flags to zero as well.]

    ASP_CFLAGS, // clear the flag register
    ASP_CLZ,    // clear the zero flag
    ASP_CLN,    // clear the negative flag
    ASP_CLE,    // clear the equal flag
    ASP_CLG,    // clear the greater flag
    ASP_CLO,    // clear the overflow flag
    ASP_CLC,    // clear the carrt flag

    // memory related instructions
    // These instructions only interact with data memory and not with the instruction memory
    ASP_LOAD,  // load instruction to load from memory to a given register
    ASP_STORE, // store instruction to store to memory from a given register

    // this can be used in various ways
    /*
      The lea instruction has variants for different type of memory address calculations:
      1) simple address lea instruction takes the address as the opearnd and puts the address into a register
      2) another takes a base register as one operand and the offset as the next operand
      3) This variant takes a base register, an index and calculates the offset. Since each address in the memory is 8 bytes long, the program either has to manipulate the read(past) value to access the desired byte else the index will be multiplied by 8 to calculate the address
    */
    ASP_LEA, // lea instruction to load the address to a register
    ASP_LEA_OFF,
    ASP_LEA_IND,

    // stack related instructions
    ASP_PUSH_IMM, // push immediate to stack
    ASP_PUSH_REG, // push register to stack
    ASP_POP,      // pop a value off stack to a register
    ASP_PUSHA,    // push all of the register values to the stack in some order
    ASP_POPA,     // pop all of the register values from the stack in opposite order

    ASP_CALL, // the in-built instruction for calling modules[pushes the registers onto the stack]
    ASP_RET,  // the return instruction for returning from a module call[pops them back]
    ASP_SBA,  // Stack Base Access instruction to access a certain value from the stack above the bp[use for parameter addressing]

    // unsigned arithmetic instructions
    ASP_ADDC_IMM_REG, // add with carry instruction[for unsigned operation: checks the carry flag]
    ASP_ADDC_REG_REG, // add with carry instruction[for unsigned operation: checks the carry flag]
    ASP_ADD_IMM_REG,  // add instruction
    ASP_ADD_REG_REG,  // add instruction

    ASP_SUB_IMM_REG, // subtraction instruction
    ASP_SUB_REG_REG, // subtraction instruction

    ASP_MUL_IMM_REG, // multiply
    ASP_MUL_REG_REG, // multiply

    ASP_DIV_IMM_REG, // divide instruction
    ASP_DIV_REG_REG, // divide instruction

    ASP_MOD_IMM_REG, // modulus instruction
    ASP_MOD_REG_REG, // modulus instruction

    // signed arithmetic instructions
    ASP_IADDC_IMM_REG, // add with carry instruction[for signed operation: checks the overflow flag]
    ASP_IADDC_REG_REG, // add with carry instruction[for signed operation: checks the overflow flag]
    ASP_IADD_IMM_REG,  // signed add
    ASP_IADD_REG_REG,  // signed add

    ASP_ISUB_IMM_REG, // signed sub
    ASP_ISUB_REG_REG, // signed sub

    ASP_IMUL_IMM_REG, // signed multiply
    ASP_IMUL_REG_REG, // signed multiply

    ASP_IDIV_IMM_REG, // signed divide
    ASP_IDIV_REG_REG, // signed divide

    ASP_IMOD_IMM_REG, // signed modulus
    ASP_IMOD_REG_REG, // signed modulus

    // floating point arithmetic
    // Aspire VM follows the IEEE754 representation of floating point numbers
    // only the representation is adapted
    // there are no unsigned floating point numbers
    // // ASP_FDADD, // add instruction for 32-bit floating point numbers
    // // ASP_FDSUB, // sub instruction for 32-bit floating point numbers
    // // ASP_FDMUL, // mul instruction for 32-bit floating point numbers
    // // ASP_FDDIV, // div instruction for 32-bit floating point numbers
    // // ASP_FMOD,  // mod instruction for 32-bit floating point numbers

    // // ASP_FQADD, // add instruction for 64-bit floating point numbers
    // // ASP_FQSUB, // sub instruction for 64-bit floating point numbers
    // // ASP_FQMUL, // mul instruction for 64-bit floating point numbers
    // // ASP_FQDIV, // div instruction for 64-bit floating point numbers
    // // ASP_FQMOD, // mod instruction for 64-bit floating point numbers
    ASP_INTR, // generate an interrupt
};

enum _Asp_Registers
{
    Aa, // general purpose register
    Ab, // general purpose register
    Ac, // general purpose register
    Ad, // general purpose register
    Ae, // general purpose register
    Af, // general purpose register

    Ar1, // extra register for anything
    Ar2, // extra register for anything
    Ar3, // extra register for anything
    Ar4, // extra register for anything
    Ar5, // extra register for anything

    // most likely to be used for floating point numbers
    Ax1, // Unknown register for now
    Ax2, // Unknown register for now
    Ax3, // Unknown register for now
    Ax4, // Unknown register for now
    Ax5,

    // 4 inaccessible registers, 16 registers for any use but this count may be increased in the future

    REGR_COUNT,
};

typedef struct _Asp_CPU _Asp_CPU;

#include "../../Manager/aspire_interrupt_handler.h"

struct _Asp_CPU
{
    _Asp_Memory *memory; // memory will be shared if there comes a decision for multiple CPUs in which case we need shared memory[Multiprocessing][Cores managed by the manager]
    _Asp_Inst_Mem *imem; // instruction memory will also be shared
    // _Asp_Cache *l1; // cache has been removed for now
    aQword _asp_registers[REGR_COUNT];
    aQword pc, sp, cid, ir, bp; // these registers are inaccessible to anything
    _Asp_Flags flags;
    aQptr_t stack;
    aBool running; // flag for if the core is running
    aBool should_wait;
    _Asp_Intr_Hdlr *hdlr;
    _Asp_Mutex *lock;
    _Asp_Cond *cond;
};

// we need to start the CPU
_Asp_CPU *_asp_start_cpu(_Asp_Memory *mem, _Asp_Inst_Mem *imem, _Asp_Intr_Hdlr *hdlr);

void *_asp_start_execution(void *cpu);

// we also need to stop the cpu
void _asp_stop_cpu(_Asp_CPU *cpu);

/*<---- The instructions ---->*/

// _ASP_ALWAYS_INLINE void _asp_MOVE(_Asp_CPU *cpu, aQword regr, aQword value)
// {
//     cpu->_asp_registers[regr] = value;
// }

// void _asp_inst_move_imm(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_movesx(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_excg(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_add(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_sub(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_mul(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_div(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_mod(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_iadd(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_isub(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_imul(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_idiv(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_imod(_Asp_CPU *cpu, aQword inst);

void _asp_inst_inc(_Asp_CPU *cpu, aQword inst);
void _asp_inst_dec(_Asp_CPU *cpu, aQword inst);
int _asp_inst_pusha(_Asp_CPU *cpu);
int _asp_inst_popa(_Asp_CPU *cpu);
int _asp_inst_call(_Asp_CPU *cpu);
int _asp_inst_ret(_Asp_CPU *cpu);
int _asp_inst_sba(_Asp_CPU *cpu);

void _asp_inst_addc_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_addc_reg_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_add_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_add_reg_reg(_Asp_CPU *cpu, aQword inst);

void _asp_inst_sub_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_sub_reg_reg(_Asp_CPU *cpu, aQword inst);

void _asp_inst_mul_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_mul_reg_reg(_Asp_CPU *cpu, aQword inst);

int _asp_inst_div_imm_reg(_Asp_CPU *cpu, aQword inst);
int _asp_inst_div_reg_reg(_Asp_CPU *cpu, aQword inst);

int _asp_inst_mod_imm_reg(_Asp_CPU *cpu, aQword inst);
int _asp_inst_mod_reg_reg(_Asp_CPU *cpu, aQword inst);

void _asp_inst_iaddc_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_iaddc_reg_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_iadd_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_iadd_reg_reg(_Asp_CPU *cpu, aQword inst);

void _asp_inst_isub_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_isub_reg_reg(_Asp_CPU *cpu, aQword inst);

void _asp_inst_imul_imm_reg(_Asp_CPU *cpu, aQword inst);
void _asp_inst_imul_reg_reg(_Asp_CPU *cpu, aQword inst);

int _asp_inst_idiv_imm_reg(_Asp_CPU *cpu, aQword inst);
int _asp_inst_idiv_reg_reg(_Asp_CPU *cpu, aQword inst);

int _asp_inst_imod_imm_reg(_Asp_CPU *cpu, aQword inst);
int _asp_inst_imod_reg_reg(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_addc(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_cmp(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_jmp(_Asp_CPU *cpu, aQword inst);

// // void _asp_inst_fdadd(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_and(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_or(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_xor(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_lshift(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_rshift(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_load(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_store(_Asp_CPU *cpu, aQword inst);

// void _asp_inst_push(_Asp_CPU *cpu, aQword inst);
// void _asp_inst_pop(_Asp_CPU *cpu, aQword inst);

/*<---- SOME STATIC FUNCTIONS ---->*/

static void _asp_reset_all(_Asp_CPU *cpu)
{
    cpu->flags.equal = 0;
    cpu->flags.greater = 0;
    cpu->flags.negative = 0;
    cpu->flags.overflow = 0;
    cpu->flags.zero = 0;
    cpu->flags.carry = 0;
    cpu->_asp_registers[Aa] = 0;
    cpu->_asp_registers[Ab] = 0;
    cpu->_asp_registers[Ac] = 0;
    cpu->_asp_registers[Ad] = 0;
    cpu->_asp_registers[Ae] = 0;
    cpu->_asp_registers[Af] = 0;
    cpu->_asp_registers[Ar1] = 0;
    cpu->_asp_registers[Ar2] = 0;
    cpu->_asp_registers[Ar3] = 0;
    cpu->_asp_registers[Ar4] = 0;
    cpu->_asp_registers[Ar5] = 0;
    cpu->_asp_registers[Ax1] = 0;
    cpu->_asp_registers[Ax2] = 0;
    cpu->_asp_registers[Ax3] = 0;
    cpu->_asp_registers[Ax4] = 0;
    cpu->_asp_registers[Ax5] = 0;
}

_ASP_ALWAYS_INLINE void _asp_reset_flags(_Asp_CPU *cpu)
{
    cpu->flags.equal = 0;
    cpu->flags.greater = 0;
    cpu->flags.negative = 0;
    cpu->flags.overflow = 0;
    cpu->flags.zero = 0;
    cpu->flags.carry = 0;
}

_ASP_ALWAYS_INLINE aQword _asp_sign_ext(aQword val, aSize_t signpos)
{
    // extend the binary at signpos to the rest of the value
    // with this we can use movesx instruction easily
    if ((val >> signpos) == 0)
    {
        return val;
    }
    return _ASP_SIGN_EXT(val);
}

// give it the number and it will provide a full error message with what type of error it is
// aStr_t _asp_get_cpu_error(_Asp_CPU_Errors err);

#endif