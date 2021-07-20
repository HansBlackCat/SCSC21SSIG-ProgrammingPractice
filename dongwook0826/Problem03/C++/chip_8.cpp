#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "chip_8.hpp"
#include "debug.hpp"

const uint8_t Chip8::font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::init() {
    opcode = 0;
    std::fill_n(memory, 0x1000, 0);
    std::fill_n(reg_mem, 0x10, 0);
    ix_reg = 0;
    prog_cnt = 0x200;

    std::fill_n(stack, 0x10, 0);
    stack_ptr = 0;

    for (int i = 0; i < 80; i++)
        memory[i] = font[i];

    delay_timer = sound_timer = 0;

    std::fill_n(display, 0x800, 0);
    update_display();
}

bool Chip8::load(const std::string &fileName) {
    std::ifstream fileInput(fileName, std::ifstream::binary);
    if (!fileInput.is_open()) {
        std::cerr << "File not opened: "
                  << fileName << '\n';
        return false;
    }
    std::cout << "Nicely opened: " << fileName << '\n';
    fileInput.seekg(0, std::ios::end);
    size_t fileSize = fileInput.tellg();
    fileInput.seekg(0, std::ios::beg);

    if (fileSize > sizeof(memory) - prog_cnt)
        fileSize = sizeof(memory) - prog_cnt;
    fileInput.read((char*) memory + prog_cnt, fileSize);

    return true;
}

void Chip8::emulate() {
    // this runs forever, unless the emul_window is closed
    sf::Event event;
    auto zero = std::chrono::duration<double, std::ratio<1, 60>>::zero();
    while (emul_window->isOpen()) {
        auto start = std::chrono::high_resolution_clock::now();
        while (emul_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return;       
        }
        // fetch opcode
        for (int l = 0; l < 10; l++) {
            opcode = memory[prog_cnt] << 8 | memory[prog_cnt + 1];
            // show_opcode(opcode, prog_cnt, ix_reg);
            run_opcode(opcode);
            prog_cnt += 2;
        }
        if (delay_timer)
            delay_timer--;
        if (sound_timer) {
            std::cout << "BEEP! (" << (int)sound_timer << " tick left)" << '\n';
            sound_timer--;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::ratio<1, 60>> time_left =
            start - end + std::chrono::seconds(1)/60.0;
        if (time_left > zero)
            std::this_thread::sleep_for(time_left);
    }
    // if closed(== hidden)
    terminate();
}

void Chip8::terminate() {
    emul_window->close();
}

void Chip8::update_display() {
    for (int p = 0; p < 0x800; p++) {
        pixels[p].setFillColor(display[p] & 1
                               ? sf::Color::White
                               : sf::Color::Black);
        emul_window->draw(pixels[p]);
    } emul_window->display();
}

/*  This emulator follows COSMAC-VIP keypad arrangement
    The whole keypad is mapped into QWERTY keyboard as following:
        1 2 3 C    ->   1 2 3 4
        4 5 6 D    ->   Q W E R
        7 8 9 E    ->   A S D F
        A 0 B F    ->   Z X C V
 */

inline uint8_t Chip8::key_pressed() {
    sf::Event event;
    while (emul_window->waitEvent(event)) {
        if (event.type == sf::Event::Closed)
            break;
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::X:    return 0x0;
                case sf::Keyboard::Num1: return 0x1;
                case sf::Keyboard::Num2: return 0x2;
                case sf::Keyboard::Num3: return 0x3;
                case sf::Keyboard::Q:    return 0x4;
                case sf::Keyboard::W:    return 0x5;
                case sf::Keyboard::E:    return 0x6;
                case sf::Keyboard::A:    return 0x7;
                case sf::Keyboard::S:    return 0x8;
                case sf::Keyboard::D:    return 0x9;
                case sf::Keyboard::Z:    return 0xa;
                case sf::Keyboard::C:    return 0xb;
                case sf::Keyboard::Num4: return 0xc;
                case sf::Keyboard::R:    return 0xd;
                case sf::Keyboard::F:    return 0xe;
                case sf::Keyboard::V:    return 0xf;
            }
        }
    }
    terminate();
    return 0xff;
}

inline sf::Keyboard::Key key_map(uint8_t hex) {
    switch (hex) {
        case 0x0: return sf::Keyboard::X;
        case 0x1: return sf::Keyboard::Num1;
        case 0x2: return sf::Keyboard::Num2;
        case 0x3: return sf::Keyboard::Num3;
        case 0x4: return sf::Keyboard::Q;
        case 0x5: return sf::Keyboard::W;
        case 0x6: return sf::Keyboard::E;
        case 0x7: return sf::Keyboard::A;
        case 0x8: return sf::Keyboard::S;
        case 0x9: return sf::Keyboard::D;
        case 0xa: return sf::Keyboard::Z;
        case 0xb: return sf::Keyboard::C;
        case 0xc: return sf::Keyboard::Num4;
        case 0xd: return sf::Keyboard::R;
        case 0xe: return sf::Keyboard::F;
        case 0xf: return sf::Keyboard::V;
        default:  return sf::Keyboard::Unknown;
    }
}

void Chip8::run_opcode(uint16_t opcode) {
    switch (opcode & 0xf000) {
        case 0x0000:
            switch (opcode & 0x0fff) {
                case 0x0e0: // clear screen
                    std::fill_n(display, 0x800, 0);
                    update_display();
                    break;
                case 0x0ee: // subroutine
                    stack[stack_ptr] = 0;
                    stack_ptr--;
                    prog_cnt = stack[stack_ptr];
                    break;
                default:
                    // including 0NNN
                    warn_opcode(opcode);
            } break;
        case 0x1000: // jump
            prog_cnt = (opcode & 0x0fff) - 2;
            break;
        case 0x2000: // subroutine
            stack[stack_ptr] = prog_cnt;
            stack_ptr++;
            prog_cnt = (opcode & 0x0fff) - 2;
            break;
        case 0x3000: // skip
            if (reg_mem[(opcode & 0x0f00) >> 8] == (opcode & 0x00ff))
                prog_cnt += 2;
            break;
        case 0x4000: // skip
            if (reg_mem[(opcode & 0x0f00) >> 8] != (opcode & 0x00ff))
                prog_cnt += 2;
            break;
        case 0x5000: // skip
            if (opcode & 0x000f)
                warn_opcode(opcode);
            else if (reg_mem[(opcode & 0x0f00) >> 8] == reg_mem[(opcode & 0x00f0) >> 4])
                prog_cnt += 2;
            break;
        case 0x6000: // set
            reg_mem[(opcode & 0x0f00) >> 8] = opcode & 0x00ff;
            break;
        case 0x7000: // add
            if (uint8_t ix = (opcode & 0x0f00) >> 8; ix != 0xf) {
                uint16_t tmpsum = reg_mem[ix] + (opcode & 0x00ff);
                reg_mem[0xf] = tmpsum > 0xff ? 1 : 0;
                reg_mem[ix] = tmpsum;
            } break;
        case 0x8000: // logical & arithmetic operation
        {
            uint8_t x = (opcode & 0x0f00) >> 8;
            uint8_t y = (opcode & 0x00f0) >> 4;
            switch (opcode & 0x000f) {
                case 0x0:
                    reg_mem[x] = reg_mem[y];
                    break;
                case 0x1:
                    reg_mem[x] |= reg_mem[y];
                    break;
                case 0x2:
                    reg_mem[x] &= reg_mem[y];
                    break;
                case 0x3:
                    reg_mem[x] ^= reg_mem[y];
                    break;
                case 0x4:
                {
                    uint16_t tmpsum = reg_mem[x] + reg_mem[y];
                    reg_mem[0xf] = tmpsum > 0xff ? 1 : 0;
                    reg_mem[x] = tmpsum;
                } break;
                case 0x5:
                    reg_mem[0xf] = reg_mem[x] >= reg_mem[y] ? 1 : 0;
                    reg_mem[x] -= reg_mem[y];
                    break;
                case 0x6:
                    reg_mem[0xf] = reg_mem[x] & 0x01 ? 1 : 0;
                    reg_mem[x] >>= 1;
                    break;
                case 0x7:
                    reg_mem[0xf] = reg_mem[y] >= reg_mem[x] ? 1 : 0;
                    reg_mem[x] = reg_mem[y] - reg_mem[x];
                    break;
                case 0xe:
                    reg_mem[0xf] = reg_mem[x] & 0x80 ? 1 : 0;
                    reg_mem[x] <<= 1;
                    break;
                default:
                    warn_opcode(opcode);
            }
        } break;
        case 0x9000: // skip
            if (opcode & 0x000f)
                warn_opcode(opcode);
            else if (reg_mem[(opcode & 0x0f00) >> 8] != reg_mem[(opcode & 0x00f0) >> 4])
                prog_cnt += 2;
            break;
        case 0xa000: // set ix_reg
            ix_reg = opcode & 0x0fff;
            break;
        case 0xb000: // jump with offset
            prog_cnt = reg_mem[0] + (opcode & 0x0fff) - 2;
            break;
        case 0xc000: // random
            reg_mem[(opcode & 0x0f00) >> 8] = rand() & 0xff & opcode;
            break;
        case 0xd000: // display
        {
            reg_mem[0xf] = 0;
            uint8_t x = reg_mem[(opcode & 0x0f00) >> 8] & 0x3f;
            uint8_t y = reg_mem[(opcode & 0x00f0) >> 4] & 0x1f;
            for (uint8_t r = 0; r < (opcode & 0x000f) && r < 0x20 - y; r++) {
                for (uint8_t c = 0; c < 8 && c < 0x40 - x; c++) {
                    if ((memory[ix_reg + r] & (0x80 >> c))) {
                        uint8_t &dtmp = display[(y + r) * 0x40 + x + c]; 
                        reg_mem[0xf] = dtmp & 1 ? 1 : 0;
                        dtmp ^= 1;
                    }
                }
            }
            update_display();
        } break;
        case 0xe000: // skip according to key press
        {
            uint8_t x = reg_mem[(opcode & 0x0f00) >> 8];
            switch (opcode & 0x00ff) {
                case 0x9e: // skip if key pressed
                    if (sf::Keyboard::isKeyPressed(key_map(x)))
                        prog_cnt += 2;
                    break;
                case 0xa1: // skip if key not pressed
                    if (!sf::Keyboard::isKeyPressed(key_map(x)))
                        prog_cnt += 2;
                    break;
                default:
                    warn_opcode(opcode);
            }
        } break;
        default/* case 0xf000 */:
        {
            uint8_t x = (opcode & 0x0f00) >> 8;
            switch (opcode & 0x00ff) {
                case 0x07: // timer: VX <- delay
                    reg_mem[x] = delay_timer;
                    break;
                case 0x0a: // get key
                    reg_mem[x] = key_pressed();
                    break;
                case 0x15: // timer: delay <- VX
                    delay_timer = reg_mem[x];
                    break;
                case 0x18: // timer: sound <- VX
                    sound_timer = reg_mem[x];
                    break;
                case 0x1e: // add to index
                    ix_reg += reg_mem[x];
                    if (ix_reg & 0xf000) {
                        ix_reg &= 0x0fff;
                        reg_mem[0xf] = 1;
                    } break;
                case 0x29: // point to font
                    ix_reg = reg_mem[x] * 5;
                    break;
                case 0x33: // hex -> dec conversion
                    memory[ix_reg]     = reg_mem[x] / 100;
                    memory[ix_reg + 1] = reg_mem[x] / 10 % 10;
                    memory[ix_reg + 2] = reg_mem[x] % 10;
                    break;
                case 0x55: // dump reg -> mem
                    for (uint8_t i = 0; i <= x; i++)
                        memory[ix_reg + i] = reg_mem[i];
                    break;
                case 0x65: // load mem -> reg
                    for (uint8_t i = 0; i <= x; i++)
                        reg_mem[i] = memory[ix_reg + i];
                    break;
                default:
                    warn_opcode(opcode);
            }
        }
    }
}