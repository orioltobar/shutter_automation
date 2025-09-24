#ifndef RADIO_H
#define RADIO_H
#include "common.h"

    void init_radio(unsigned char pin_radio);
    void send_up_signal();
    void send_up_signal_with_blind(BlindType blind_type);
    void send_down_signal();
    void send_down_signal_with_blind(BlindType blind_type);
    void send_stop_signal();

#endif