#ifndef CHIP_8_DEBUG
#define CHIP_8_DEBUG

#define show_opcode(M, N, I) \
    printf("Opcode read: 0x%4X; prog_cnt: 0x%3X; ix_reg: 0x%3X\n", M, N, I)
#define warn_opcode(M) \
    printf("Unexpected opcode: 0x%4X\n", M)

#endif