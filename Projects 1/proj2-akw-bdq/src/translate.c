#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* SOLUTION CODE BELOW */
const int TWO_POW_SEVENTEEN = 131072;    // 2^17

int write_mf(uint8_t funct, FILE* output, char** args, size_t num_args);
int write_multdiv(uint8_t funct, FILE* output, char** args, size_t num_args);

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and blt pseudoinstructions. Your pseudoinstruction 
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are 
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number 
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into 
        a lui-ori pair.
        

   For move, blt, bgt, traddu, swpr:
    - your expansion should use the fewest number of instructions possible.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that 
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */



unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    if (strcmp(name, "li") == 0) {
        /* YOUR CODE HERE */
      if (num_args != 2) {
        return 0;
      }
      long int imm;
      int err = translate_num(&imm, args[1], INT32_MIN, UINT32_MAX);
      if (err == -1) {
        return 0;
      }
      else {
        err = translate_num(&imm, args[1], INT16_MIN, UINT16_MAX);
        if (err == -1) {
          int upper = (imm>>16) & (0xffff);
          int lower = imm & (0xffff);
          fprintf(output, "lui $at %d\n", upper);
          fprintf(output, "ori %s $at %d\n", args[0], lower);
          return 2;
        }
        else {
          fprintf(output, "addiu %s $0 %lu\n", args[0], imm);
          return 1;
        }
      }

    } else if (strcmp(name, "move") == 0) {
        /* YOUR CODE HERE */
        if (num_args!=2) {
          return 0;
        }
        
        fprintf(output, "addu %s %s $0\n", args[0], args[1]);
        return 1;
  
    } else if (strcmp(name, "blt") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }

        fprintf(output, "slt $at %s %s\n", args[0], args[1]);
        fprintf(output, "bne $at $0 %s\n", args[2]);
        return 2;

    } else if (strcmp(name, "bgt") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }

        fprintf(output, "slt $at %s %s\n", args[1], args[0]);
        fprintf(output, "bne $at $0 %s\n", args[2] );
        return 2;

    } else if (strcmp(name, "traddu") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }

        fprintf(output, "addu $at %s %s\n", args[1], args[2]);
        fprintf(output, "addu %s %s $at\n", args[2], args[2]);
        return 2;      

    } else if (strcmp(name, "swpr") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 2) {
          return 0;
        }

        fprintf(output, "addu $at %s $0\n", args[0]);
        fprintf(output, "addu %s %s $0\n", args[0], args[1]);
        fprintf(output, "addu %s $at $0\n", args[1]);
        return 3;
            
    } else if (strcmp(name, "mul") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }

        fprintf(output, "mult %s %s\n", args[1], args[2]);
        fprintf(output, "mflo %s\n", args[0]);
        return 2;
        
    } else if (strcmp(name, "div") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 2)
        {
		if (num_args !=3)
		{
			return -1;
		}
		else
		{
          	fprintf(output, "div %s %s\n", args[1], args[2]);
        	fprintf(output, "mflo %s\n", args[0]);
        	return 2;
		}
          
        }

        
        
    } else if (strcmp(name, "rem") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3)
        {
          return 0;
        }

        fprintf(output, "div %s %s\n", args[1], args[2]);
        fprintf(output, "mfhi %s\n", args[0]);
        return 2;
        
    } 

    write_inst_string(output, name, args, num_args);
    return 1;
}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS. 

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros. 

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write 
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Some function declarations for the write_*() functions are provided in translate.h, and you MUST implement these
   and use these as function headers. You may use helper functions, but you still must implement
   the provided write_* functions declared in translate.h.

   Returns 0 on success and -1 on error. 
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
    SymbolTable* symtbl, SymbolTable* reltbl) {
    if (strcmp(name, "addu") == 0)       return write_rtype (0x21, output, args, num_args);
    else if (strcmp(name, "or") == 0)    return write_rtype (0x25, output, args, num_args);
    else if (strcmp(name, "slt") == 0)   return write_rtype (0x2a, output, args, num_args);
    else if (strcmp(name, "sltu") == 0)  return write_rtype (0x2b, output, args, num_args);
    else if (strcmp(name, "sll") == 0)   return write_shift (0x00, output, args, num_args);
    /* YOUR CODE HERE */
    else if (strcmp(name, "jr") == 0)      return write_jr (0x08, output, args, num_args);
    else if (strcmp(name, "addiu") == 0)   return write_addiu (0x09, output, args, num_args);
    else if (strcmp(name, "ori") == 0)     return write_ori (0x0d, output, args, num_args);
    else if (strcmp(name, "lui") == 0)     return write_lui (0x0f, output, args, num_args);
    else if (strcmp(name, "lb") == 0)      return write_mem (0x20, output, args, num_args);
    else if (strcmp(name, "lbu") == 0)     return write_mem (0x24, output, args, num_args);
    else if (strcmp(name, "lw") == 0)      return write_mem (0x23, output, args, num_args);
    else if (strcmp(name, "sb") == 0)      return write_mem (0x28, output, args, num_args);
    else if (strcmp(name, "sw") == 0)      return write_mem (0x2b, output, args, num_args);
    else if (strcmp(name, "beq") == 0)     return write_branch (0x04, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "bne") == 0)     return write_branch (0x05, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "j") == 0)       return write_jump (0x02, output, args, num_args, addr, reltbl);
    else if (strcmp(name, "jal") == 0)     return write_jump (0x03, output, args, num_args, addr, reltbl);

    // for pseudoinstructions
    else if (strcmp(name, "mult") == 0)    return write_multdiv (0x18, output, args, num_args);
    else if (strcmp(name, "div") == 0)     return write_multdiv (0x1a, output, args, num_args);
    else if (strcmp(name, "mfhi") == 0)    return write_mf (0x10, output, args, num_args);
    else if (strcmp(name, "mflo") == 0)    return write_mf (0x12, output, args, num_args);

    else                                 return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to 
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }

    int rd = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int rt = translate_reg(args[2]);
    if (rd == -1 || rs == -1 || rt == -1) {
      return -1;
    }

    uint32_t instruction = funct + (rd << 11) + (rt << 16) + (rs << 21);
    write_inst_hex(output, instruction);
    return 0;
}

/* A helper function for writing shift instructions. You should use 
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */   
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
	// Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }

    long int shamt;
    int rd = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int err = translate_num(&shamt, args[2], 0, 31);
    if (rd == -1 || rt == -1 || err == -1) {
      return -1;
    }

    uint32_t instruction = funct + (shamt << 6) + (rd << 11) + (rt << 16);
    write_inst_hex(output, instruction);
    return 0;
}

/* The rest of your write_*() functions below */

int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 1) {
      return -1;
    }

    int rs = translate_reg(args[0]);
    if (rs == -1){
      return -1;
    }

    uint32_t instruction = (rs << 21) + funct;
    write_inst_hex(output, instruction);
    return 0;
}

int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }

    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);
    if (rt == -1 || rs == -1 || err == -1){
      return -1;
    }

    uint32_t instruction = (opcode << 26) + (rs << 21) + (rt << 16) + (imm & 0xffff);
    write_inst_hex(output, instruction);
    return 0;
}

int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }

    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], 0, UINT16_MAX);
    if (rt == -1 || rs == -1 || err == -1){
      return -1;
    }

    uint32_t instruction = (opcode << 26) + (rs << 21) + (rt << 16) + (imm & 0xffff);
    write_inst_hex(output, instruction);
    return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 2) {
      return -1;
    }

    long int imm;
    int rt = translate_reg(args[0]);
    int err = translate_num(&imm, args[1], 0, UINT16_MAX);
    if (rt == -1 || err == -1){
      return -1;
    }

    uint32_t instruction = (opcode << 26) + (rt << 16) + (imm & 0xffff);
    write_inst_hex(output, instruction);
    return 0;
}

int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }

    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[2]);
    int err = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
    if (rt == -1 || rs == -1 || err == -1){
      return -1;
    }

    uint32_t instruction = (opcode << 26) + (rs << 21) + (rt << 16) + (imm & 0xffff);
    write_inst_hex(output, instruction);
    return 0;
}

/*  A helper function to determine if a destination address
    can be branched to
*/
static int can_branch_to(uint32_t src_addr, uint32_t dest_addr) {
    int32_t diff = dest_addr - src_addr;
    return (diff >= 0 && diff <= TWO_POW_SEVENTEEN) || (diff < 0 && diff >= -(TWO_POW_SEVENTEEN - 4));
}


int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }

    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    uint32_t label_addr = get_addr_for_symbol(symtbl, args[2]);
    if (rs == -1 || rt == -1 || label_addr == -1) {
      return -1;
    }
	if (!can_branch_to(addr, label_addr)) {
      return -1;
    }
    int32_t offset = (label_addr - addr +4) / 4;
    
    uint32_t instruction = (opcode << 26) + (rs << 21) + (rt << 16) + offset;
    write_inst_hex(output, instruction);        
    return 0;
}

int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* reltbl) {
    /* YOUR CODE HERE */
    if (num_args != 1) {
      return -1;
    }

    int retval = add_to_table(reltbl, args[0], addr);
    if (retval == -1) {
      return -1;
    }

    uint32_t instruction = opcode << 26;
    write_inst_hex(output, instruction);
    return 0;
}

int write_multdiv(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 2) {
      return -1;
    }

    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    if (rs == -1 || rt == -1) {
      return -1;
    }

    uint32_t instruction = funct + (rt << 16) + (rs << 21);
    write_inst_hex(output, instruction);
    return 0;
}

int write_mf(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 1)
    {
      return -1;
    }

    int rd = translate_reg(args[0]);
    if (rd == -1) {
      return -1;
    }
    
    uint32_t instruction = (rd << 11) + funct;
    write_inst_hex(output, instruction);
    return 0;
}
