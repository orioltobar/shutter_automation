#ifndef DISPLAY_H
#define DISPLAY_H

#ifndef DEBUG_DISPLAY
#define DEBUG_DISPLAY 0
#endif

#include "Arduino.h"

class Display {
    public:
        Display(int pin_clock, int pin_data, int pin_1, int pin_2, int pin_3, int pin_4);
        void begin();
        void set_text(const char* text);
        void refresh();

    private:
        int _pin_clock;
        int _pin_data;
        int _pin_pos[4];
        char _text[8];

        int get_string_last_postion(char* text);
        byte letter_to_bits(char letter);
        void print_letter(byte position, char letter, bool has_dot);
};

#endif