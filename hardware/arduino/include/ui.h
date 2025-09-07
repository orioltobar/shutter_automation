#ifndef UI_H
#define UI_H

#include "common.h"
#include "Display.h"

void on_time_updated_ui();
void on_alarm_up_updated_ui();
void on_alarm_down_updated_ui();
void show_ui();
void on_select_press();
void on_next_press();
void show_up_animation();
void show_down_animation();
void init_ui(Display* display); 
void set_on_ui_up_persiona_callback(void (*callback)());
void set_on_ui_down_persiona_callback(void (*callback)());
void set_on_alarm_down_set_callback(void (*callback)());
void set_on_alarm_up_set_callback(void (*callback)());
void show_stop_animation();

#endif