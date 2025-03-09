# RISC-V Assembler

This is a simple RISC-V assembler that converts RISC-V assembly code into machine code. The assembler handles various RISC-V instruction formats, including R-type, I-type, S-type, SB-type, U-type, and UJ-type instructions.

## Overview

The assembler reads RISC-V assembly code from an input file (`input.asm`), processes it, and generates the corresponding machine code in an output file (`output.mc`). The machine code is presented in both binary and hexadecimal formats, along with the original instructions and their component fields.

## Files

- **main.cpp**: Main implementation of the RISC-V assembler
- **isa.h**: Contains mappings for RISC-V instruction opcodes, function codes, and related functionality
- **registers.h**: Handles the encoding of register names to their binary representations
- **memory.h**: Manages memory operations, data handling, and label resolution

## Features

- Supports multiple RISC-V instruction formats:
  - R-type (add, sub, sll, slt, xor, srl, or, and, rem, mul, div)
  - I-type (addi, andi, ori, jalr, lb, ld, lw, lh)
  - S-type (sb, sh, sw, sd)
  - SB-type (beq, bne, bge, blt)
  - U-type (auipc, lui)
  - UJ-type (jal)
- Handles memory directives (.byte, .word, .half, .dword, .asciiz)
- Supports labels for branch and jump instructions
- Provides detailed output with formatted machine code fields

## Input Format

The input assembly file (`input.asm`) should follow this structure:

```
.data
    # Data section with variable declarations
    var1: .word 100
    string1: .asciiz "Hello"

.text
    # Code section with RISC-V instructions
    main:
        addi x1, x0, 10
        add x2, x1, x0
        beq x1, x2, end
    end:
        # More instructions
```

## Output Format

The generated output file (`output.mc`) contains:

1. Program counter (PC) address
2. Machine code in hexadecimal
3. Original assembly instruction
4. Formatted fields of the instruction

Example output:
```
PC     MACHINE CODE  INSTRUCTIONS              FIELDS
0x0    0x00A00093    addi x1, x0, 10          #000000001010-00000-000-00001-0010011
0x4    0x00008133    add x2, x1, x0           #0000000-00000-00001-000-00010-0110011
```

## How to Use

1. Create an input file named `input.asm` with your RISC-V assembly code
2. Compile the assembler:
   ```
   g++ -o assembler main.cpp
   ```
3. Run the assembler:
   ```
   ./assembler
   ```
4. Check the output in `output.mc`

