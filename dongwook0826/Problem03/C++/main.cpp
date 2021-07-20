#include <iostream>
#include "chip_8.hpp"

int main(int argc, char *argv[]) {
    Chip8 chip8;
    std::cout << "Start the emulator\n";
    chip8.init();
    if (chip8.load(argv[1]))
        chip8.emulate();
    return 0;
}