#include <iostream>
#include "chip_8.hpp"

int main(int argc, char *argv[]) {
    std::cout << "Starting the emulator\n";
    Chip8 chip8;
    chip8.init();
    if (chip8.load(argv[1]))
        chip8.emulate();
    std::cout << "Turning off... goodbye\n";
    return 0;
}