#include "radio.h"
#include <Arduino.h>
#include "common.h"

// This value defines how many times we will send a signal. 
// We send it multiple times because sometimes it is not picked by the receiver.
#define NUM_SEQUENCE_REP 3

static char sequence_l_up[20] = "b6d92592c92d965b248";  
static char sequence_m_up[20] = "b6d92492d924965b248";
static char sequence_r_up[20] = "b6cb65b6cb25965b248";
static char sequence_l_down[20] = "b6d92592c92d965b2c8";
static char sequence_m_down[20] = "b6d92492d924965b2c8";
static char sequence_r_down[20] = "b6cb65b6cb25965b2c8";
static char sequence_l_stop[20] = "b6d92592c92d965b258";
static char sequence_m_stop[20] = "b6d92492d924965b258";
static char sequence_r_stop[20] = "b6cb65b6cb25965b258";
static unsigned char _pin_radio;

void init_radio(unsigned char pin_radio) 
{
    pinMode(pin_radio, OUTPUT);
    _pin_radio = pin_radio;
}

static void send_hex_digit(char c) {
    uint16_t pulse = 330;
    
    switch (c) {
        // hex = 0, send 4 low pulses
        case '0':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 4);
            break;

        // hex = 1, send 3 low pulses and 1 high pulse
        case '1':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 3);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = 2, send 2 low pulses,  1 high pulse, 1 low pulse
        case '2':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            break;

        // hex = 3, send 2 low pulses,  2 high pulses
        case '3':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = 4, send 1 low pulses,  1 high pulse, 2 low pulse
        case '4':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 2);
            break;

        // hex = 5, send 1 low pulses,  1 high pulse, 2 low pulse
        case '5':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = 6, send 1 low pulses,  2 high pulse, 1 low pulse
        case '6':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            break;

        // hex = 7, send 1 low pulse,  3 high pulse
        case '7':
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 3);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = 8, send 1 high pulse,  3 low pulses
        case '8':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 3);
            break;

        // hex = 9, send 1 high pulse,  2 low pulses, 1 high pulse
        case '9':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = a, send 1 high pulse,  1 low pulses, 1 high pulse, 1 low pulse
        case 'a':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            break;

        // hex = b, send 1 high pulse,  1 low pulses, 2 high pulses
        case 'b':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = c, send 2 high pulse,  2 low pulses
        case 'c':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 2);
            break;

        // hex = d, send 2 high pulse,  1 low pulses, 1 high pulses
        case 'd':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 2);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 1);
            digitalWrite(_pin_radio, LOW);
            break;

        // hex = e, send 3 high pulses,  1 low pulses
        case 'e':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 3);
            digitalWrite(_pin_radio, LOW);
            delayMicroseconds(pulse * 1);
            break;

        // hex = f, send 4 high pulses
        case 'f':
            digitalWrite(_pin_radio, HIGH);
            delayMicroseconds(pulse * 4);
            digitalWrite(_pin_radio, LOW);
            break;

        default:
            digitalWrite(_pin_radio, LOW);
            break;
    }; 
}   

static void send_command(char* command) {
  int command_length = 20;

  for (int i = 0; i < command_length; i++) 
   {  
    send_hex_digit(command[i]);
   }
}

void send_down_signal() {
    for (int count = 0; count < NUM_SEQUENCE_REP; count++) {
        send_command(sequence_l_down);
        delay(10);
        send_command(sequence_m_down);
        delay(10);
        send_command(sequence_r_down);
        delay(10);
    }
}

void send_down_signal_with_blind(BlindType blind_type) {
    for (int count = 0; count < NUM_SEQUENCE_REP; count++) {
        if (blind_type == LEFT) {
            send_command(sequence_l_down);
            delay(10);
        }
        if (blind_type == CENTER) {
            send_command(sequence_m_down);
            delay(10);
        }
        if (blind_type == RIGHT) {
            send_command(sequence_r_down);
            delay(10);
        }
        if (blind_type == ALL) {
            send_command(sequence_l_down);
            delay(10);
            send_command(sequence_m_down);
            delay(10);
            send_command(sequence_r_down);
            delay(10);
        }
    }
}

void send_up_signal() {
    for (int count = 0; count < NUM_SEQUENCE_REP; count++) {
        send_command(sequence_l_up);
        delay(10);
        send_command(sequence_m_up);
        delay(10);
        send_command(sequence_r_up);
        delay(10);
    }
}

void send_up_signal_with_blind(BlindType blind_type) {
    for (int count = 0; count < NUM_SEQUENCE_REP; count++) {
        if (blind_type == LEFT) {
            send_command(sequence_l_up);
            delay(10);
        }
        if (blind_type == CENTER) {
            send_command(sequence_m_up);
            delay(10);
        }
        if (blind_type == RIGHT) {
            send_command(sequence_r_up);
            delay(10);
        }
        if (blind_type == ALL) {
            send_command(sequence_l_up);
            delay(10);
            send_command(sequence_m_up);
            delay(10);
            send_command(sequence_r_up);
            delay(10);
        }
    }
}

void send_stop_signal() {
    for (int count = 0; count < NUM_SEQUENCE_REP; count++) {
        send_command(sequence_l_stop);
        delay(10);
        send_command(sequence_m_stop);
        delay(10);
        send_command(sequence_r_stop);
        delay(10);
    }
}

void send_stop_signal_with_blind(BlindType blind_type) {
    for (int count = 0; count < NUM_SEQUENCE_REP; count++) {
        if (blind_type == LEFT) {
            send_command(sequence_l_stop);
            delay(10);
        }
        if (blind_type == CENTER) {
            send_command(sequence_m_stop);
            delay(10);
        }
        if (blind_type == RIGHT) {
            send_command(sequence_r_stop);
            delay(10);
        }
        if (blind_type == ALL) {
            send_command(sequence_l_stop);
            delay(10);
            send_command(sequence_m_stop);
            delay(10);
            send_command(sequence_r_stop);
            delay(10);
        }
    }
}