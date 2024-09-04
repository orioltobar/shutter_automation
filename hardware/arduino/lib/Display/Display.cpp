#include "Display.h"


/*
       0
       -
    5 | | 1
       -  <- 6  
    4 | | 2
       - . <- 7 
       3
*/
#define DISP_EMPTY B11111111 
#define DISP_DOT B11111110
#define DISP_0 B00000011 
#define DISP_1 B10011111 
#define DISP_2 B00100101 
#define DISP_3 B00001101 
#define DISP_4 B10011001 
#define DISP_5 B01001001    
#define DISP_6 B01000001 
#define DISP_7 B00011111 
#define DISP_8 B00000001 
#define DISP_9 B00001001 
#define DISP_A B00010001 
#define DISP_B B11000001 
#define DISP_C B11100101 
#define DISP_D B10000101 
#define DISP_E B01100001 
#define DISP_F B01110001 
#define DISP_G B00001001 
#define DISP_H B11010001 
#define DISP_I B11110011 
#define DISP_J B10001111 
#define DISP_L B11100011
#define DISP_M B11010101 
#define DISP_N B11010101 
#define DISP_O B11000101 
#define DISP_P B00110001 
#define DISP_Q B00011001 
#define DISP_R B11110101 
#define DISP_S B01001001 
#define DISP_T B11110001 
#define DISP_U B11000111 
#define DISP_V B10000011
#define DISP_X B10010001
#define DISP_Y B10011001
#define DISP_Z B00100101
#define DISP_ASTERISK B00111001
#define DISP_TILDE B01111111
#define DISP_HAT B00111011
#define DISP_MID B11111101
#define DISP_TOP B01111111
#define DISP_BOT B11011111

Display::Display(int pin_clock, int pin_data, int pin_1, int pin_2, int pin_3, int pin_4) {
    _pin_pos[0] = pin_4;
    _pin_pos[1] = pin_3;
    _pin_pos[2] = pin_2;
    _pin_pos[3] = pin_1;
    _pin_clock = pin_clock;
    _pin_data = pin_data;
}

void Display::begin() {
    pinMode(_pin_clock, OUTPUT);
    pinMode(_pin_data , OUTPUT);

    for (int pos = 0; pos < 4; pos++) {
        pinMode(_pin_pos[pos] , OUTPUT);
        digitalWrite(_pin_pos[pos],LOW);
    }

    digitalWrite(_pin_clock, LOW);
    digitalWrite(_pin_data,LOW);
}

int Display::get_string_last_postion(char* text) {
    int position = 0;

    while (text[++position] != '\0') {}
    return position - 1;
}

void Display::set_text(const char* text) {
    int pos = 0;

#ifdef DEBUG_DISPLAY
        Serial.print(F("Changed text to "));
#endif

    while(text[pos] != '\0') {
        _text[pos] = text[pos];

#ifdef DEBUG_DISPLAY
            Serial.print(_text[pos]);
#endif
        pos++;
    }

#ifdef DEBUG_DISPLAY
        Serial.println();
#endif
    _text[pos] = '\0';
}

void Display::refresh() {
    byte letters = 3;
    int last_position = get_string_last_postion(_text);

    while(last_position >= 0) {
        char letter = _text[last_position];

        if (letter == '.') {
            print_letter(letters, _text[last_position - 1], true);
            last_position--;
        } else {
            print_letter(letters, letter, false);
        }

        letters--;
        last_position--;
    }
}

void Display::print_letter(byte position, char letter, bool has_dot) {
    byte parsed_letter = letter_to_bits(letter);

    if (has_dot) {
        parsed_letter &= DISP_DOT;
    }

    digitalWrite(_pin_pos[position], HIGH);
    shiftOut(_pin_data, _pin_clock, LSBFIRST, parsed_letter);
    delay(2);
    digitalWrite(_pin_pos[position], LOW);
}

byte Display::letter_to_bits(char letter) {
    switch (letter) {
        case '0': return DISP_0;
        case '1': return DISP_1;
        case '2': return DISP_2;
        case '3': return DISP_3;
        case '4': return DISP_4;
        case '5': return DISP_5;
        case '6': return DISP_6;
        case '7': return DISP_7;
        case '8': return DISP_8;
        case '9': return DISP_9;
        case 'a':
        case 'A': 
            return DISP_A;
        case 'B':
        case 'b': 
            return DISP_B;
        case 'C':
        case 'c': 
            return DISP_C;
        case 'D':
        case 'd': 
            return DISP_D;
        case 'E':
        case 'e': 
            return DISP_E;
        case 'F':
        case 'f': 
            return DISP_F;
        case 'G':
        case 'g': 
            return DISP_G;
        case 'H':
        case 'h': 
            return DISP_H;
        case 'I':
        case 'i': 
            return DISP_I;
        case 'J':
        case 'j': 
            return DISP_J;
        case 'L':
        case 'l': 
            return DISP_L;
        case 'M':
        case 'm': 
            return DISP_M;
        case 'N':
        case 'n': 
            return DISP_N;
        case 'O':
        case 'o': 
            return DISP_O;
        case 'P':
        case 'p': 
            return DISP_P;
        case 'Q':
        case 'q': 
            return DISP_Q;
        case 'R':
        case 'r': 
            return DISP_R;
        case 'S':
        case 's': 
            return DISP_S;
        case 'T':
        case 't': 
            return DISP_T;
        case 'U':
        case 'u': 
            return DISP_U;
        case 'V':
        case 'v': 
            return DISP_V;
        case 'X':
        case 'x': 
            return DISP_X;
        case 'Y':
        case 'y': 
            return DISP_Y;
        case 'Z':
        case 'z': 
            return DISP_Z;
        case '*': return DISP_ASTERISK;
        case '~': return DISP_TILDE;
        case '^': return DISP_HAT;
        case '-': return DISP_MID;
        case '_': return DISP_BOT;
        case ' ':
        default:
            return DISP_EMPTY;
    }
}