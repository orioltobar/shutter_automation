#include "alarm.h"

static void (*_alarm_down_callback)();
static void (*_alarm_up_callback)();

static bool is_alarm_fired(struct Alarm* alarm) {
  bool should_check = alarm->is_active && !alarm->triggered;
  int current_hour = state.current_time.hours;
  int current_minute = state.current_time.minutes;

  return should_check 
      && current_hour == alarm->time.hours
      && current_minute == alarm->time.minutes;
}

static void check_alarms() {
  if (is_alarm_fired(&state.alarm_down)) {
    state.alarm_down.triggered = true;
    _alarm_down_callback();
  }

  if (is_alarm_fired(&state.alarm_up)) {
    state.alarm_up.triggered = true;
    _alarm_up_callback();
  }
}

void on_time_updated_alarm() {
  static int last_day = DAY_NOT_SET;  
  
  if (state.day != last_day) {
    last_day = state.day;
    state.alarm_down.triggered = false;
    state.alarm_up.triggered = false;
  }
  check_alarms();
}

void set_on_down_alarm_callback(void (*callback) ()) {
    _alarm_down_callback = callback;
}

void set_on_up_alarm_callback(void (*callback) ()) {
    _alarm_up_callback = callback;
}