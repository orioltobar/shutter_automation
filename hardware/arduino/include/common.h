#ifndef COMMON_H
#define COMMON_H
#define DAY_NOT_SET -1
#include "stdbool.h"
#include "stdint.h"

struct Time {
    uint8_t minutes;
    uint8_t hours;
};

struct Alarm {
    bool is_active;
    bool triggered; // True if alarm has been triggered today
    struct Time time;
};

struct State {
    int8_t day;
    struct Time current_time;
    struct Alarm alarm_down;
    struct Alarm alarm_up;
};

extern struct State state;

#endif