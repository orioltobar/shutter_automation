#ifndef ALARM_H
#define ALARM_H
#include "common.h"

void set_on_down_alarm_callback(void (*callback)());
void set_on_up_alarm_callback(void (*callback)());
void on_time_updated_alarm();

#endif