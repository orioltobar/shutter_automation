#include "memmory.h"
#include "EEPROM.h"
#define START_UP 0x00
#define START_DOWN 0x05

void init_memmory() {
    EEPROM.begin(64);
}

void store_alarm_up() {
    EEPROM.write(START_UP, state.alarm_up.time.hours);
    EEPROM.write(START_UP + 1, state.alarm_up.time.minutes);
    EEPROM.write(START_UP + 2, state.alarm_up.is_active);
    EEPROM.commit();
}

void store_alarm_down() {
    EEPROM.write(START_DOWN, state.alarm_down.time.hours);
    EEPROM.write(START_DOWN + 1, state.alarm_down.time.minutes);
    EEPROM.write(START_DOWN + 2, state.alarm_down.is_active);
    EEPROM.commit();
}

void load_alarms_from_memmory() {
    state.alarm_up.time.hours = EEPROM.read(START_UP);
    state.alarm_up.time.minutes = EEPROM.read(START_UP + 1);
    state.alarm_up.is_active = EEPROM.read(START_UP + 2);
    state.alarm_down.time.hours = EEPROM.read(START_DOWN);
    state.alarm_down.time.minutes = EEPROM.read(START_DOWN + 1);
    state.alarm_down.is_active = EEPROM.read(START_DOWN + 2);
}