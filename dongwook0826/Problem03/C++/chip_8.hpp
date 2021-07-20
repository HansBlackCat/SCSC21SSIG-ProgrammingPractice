#ifndef CHIP_8_EMULATOR
#define CHIP_8_EMULATOR
#include <cstdint>
#include <string>
#include <SFML/Window.hpp> // for display & key input
#include <SFML/Graphics.hpp> // for display emul
// #include <SFML/Audio.hpp> // for audio, i.e. beeping sound

class Chip8 {
    public:
        Chip8() {
            emul_window = new sf::RenderWindow(
                sf::VideoMode(8 * 64, 8 * 32),
                "TenDong's CHIP-8 Emulator"
            );
            emul_window->clear(sf::Color::Black);
            for (int p = 0; p < 0x800; p++) {
                pixels[p].setPosition((p & 0x3f) * 8, (p >> 6) * 8);
                pixels[p].setSize(sf::Vector2f(8.f, 8.f));
                pixels[p].setFillColor(sf::Color::Transparent);
                emul_window->draw(pixels[p]);
            } emul_window->display();
        }
        ~Chip8() { delete[] emul_window; }
        void init();
        bool load(const std::string &);
        void emulate();
        void terminate();

    private:
        void update_display();
        uint8_t key_pressed();
        void run_opcode(uint16_t);
        uint16_t opcode;
    
        uint8_t memory[0x1000];
        uint8_t reg_mem[0x10];
        uint16_t ix_reg;
        uint16_t prog_cnt;

        uint16_t stack[0x10];
        uint16_t stack_ptr;

        uint8_t delay_timer;
        uint8_t sound_timer;

        uint8_t display[0x800]; // 0x40 * 0x20
        bool display_updated;
        sf::RenderWindow *emul_window;
        sf::RectangleShape pixels[0x800];
        
        static const uint8_t font[80];
};

#endif