#ifndef CHIP_8_DEBUG
#define CHIP_8_DEBUG

#define show_opcode(M) \
    printf("Opcode read: 0x%X\n", M)
#define warn_opcode(M) \
    printf("Unexpected opcode: 0x%X\n", M)

#endif