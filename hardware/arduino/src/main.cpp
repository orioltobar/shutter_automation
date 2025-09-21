#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <time.h>         
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "common.h"
#include "memmory.h"
#include "alarm.h"
#include "api.h"
#include "ui.h"
#include "Display.h"
#include "radio.h" 

#define PIN_DISPLAY_CLK 4
#define PIN_DISPLAY_DATA 17
#define PIN_DISPLAY_1 16
#define PIN_DISPLAY_2 12
#define PIN_DISPLAY_3 2
#define PIN_DISPLAY_4 0

#define PIN_BUTTON_SELECT 14
#define PIN_BUTTON_NEXT 5
#define PIN_BUTTON_STOP 15
#define PIN_RADIO 13
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TIME_ZONE "CET-1CEST,M3.5.0/02,M10.5.0/03" 
#define AP_NAME "APAlarmaPersiana"
#define DEBOUNCE_BTN_MS 200
#define TIME_UPDATE_INTERVAL_MS 1000

Display display = Display(PIN_DISPLAY_CLK, PIN_DISPLAY_DATA, PIN_DISPLAY_1, PIN_DISPLAY_2, PIN_DISPLAY_3, PIN_DISPLAY_4); 
bool was_select_btn_pressed = false;
bool was_next_btn_pressed = false;
bool was_stop_btn_pressed = false;
unsigned long millis_select_press = 0;
unsigned long millis_next_press = 0;
unsigned long millis_stop_press = 0;
unsigned long last_millis_time = 0;

State state = {
  DAY_NOT_SET,
  {0, 0},
  {false, false, {0, 0}},
  {false, false, {0, 0}}
};

void IRAM_ATTR interrupt_select();
void IRAM_ATTR interrupt_next();
void show_moving_text(char* text, unsigned int max_position);
void configure_buttons();
void initialise_ui();
void initialise_api();
void on_down_api_called();
void on_up_api_called();
void on_alarm_up_fired();
void on_alarm_down_fired();
void on_stop_api_called();
void update_time();

void setup() {
  Serial.begin(115200);
  configure_buttons();
  display.begin();
  init_radio(PIN_RADIO);
  init_memmory();
  load_alarms_from_memmory();
  initialise_ui();
  configTime(MY_TIME_ZONE, MY_NTP_SERVER);
  WiFiManager wifiManager;
  wifiManager.autoConnect(AP_NAME);
  set_on_down_alarm_callback(&on_alarm_down_fired);
  set_on_up_alarm_callback(&on_alarm_up_fired);
  initialise_api();
}

void loop() {
  if (was_select_btn_pressed) {
    was_select_btn_pressed = false;
    on_select_press();
  }

  if (was_next_btn_pressed) {
    was_next_btn_pressed = false;
    on_next_press();
  }

  if (was_stop_btn_pressed) {
    was_stop_btn_pressed = false;
    on_stop_api_called();
  }

  if (millis() - last_millis_time > TIME_UPDATE_INTERVAL_MS) {
    last_millis_time = millis();
    update_time();
  }
  show_ui();
  check_api();
}

void IRAM_ATTR interrupt_next() {
  if (millis() - millis_next_press > DEBOUNCE_BTN_MS) {
    millis_next_press = millis();
    was_next_btn_pressed = true;
  }
}

void IRAM_ATTR interrupt_select() {
  if (millis() - millis_select_press > DEBOUNCE_BTN_MS) {
    millis_select_press = millis();
    was_select_btn_pressed = true;
  }
}

void IRAM_ATTR interrupt_stop() {
  if (millis() - millis_stop_press > DEBOUNCE_BTN_MS) {
     millis_stop_press = millis();
     was_stop_btn_pressed = true;
  }
}

void initialise_api() {
    set_on_up_api_callback(&on_up_api_called);
    set_on_down_api_callback(&on_down_api_called);
    set_on_down_updated_api_callback(&on_alarm_down_updated_ui);
    set_on_up_updated_api_callback(&store_alarm_up);
    set_on_stop_api_callback(&on_stop_api_called);
    init_api();
}

void on_up_api_called() {
  send_up_signal();
  show_up_animation();
}

void on_down_api_called() {
  send_down_signal();
  show_down_animation();
}

void on_stop_api_called() {
  send_stop_signal();
  show_stop_animation();
}

void configure_buttons() {
  pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SELECT, INPUT_PULLUP);
  pinMode(PIN_BUTTON_STOP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_NEXT), interrupt_next, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_SELECT), interrupt_select, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_STOP), interrupt_stop, FALLING);
}

void initialise_ui() {
  init_ui(&display);
  set_on_ui_up_persiona_callback(&send_up_signal);
  set_on_ui_down_persiona_callback(&send_down_signal);
  set_on_alarm_up_set_callback(&store_alarm_up);
  set_on_alarm_down_set_callback(&store_alarm_down);
}

void on_alarm_down_fired() {
  send_down_signal();
  show_down_animation();
}

void on_alarm_up_fired() {
  send_up_signal();
  show_up_animation();
}

void update_time() {
    tm time_info;
    time_t now;
    time(&now);
    localtime_r(&now, &time_info);

    if (state.current_time.minutes != time_info.tm_min) {
      state.current_time.hours = time_info.tm_hour;
      state.current_time.minutes = time_info.tm_min;
      state.day = time_info.tm_wday;
      on_time_updated_ui();
      on_time_updated_alarm();
    };
}
