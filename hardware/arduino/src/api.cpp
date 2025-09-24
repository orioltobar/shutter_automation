#include <ESP8266WiFi.h>
#include <aWOT.h>
#include "common.h"

#define TIME_PARAM "time"
#define ACTIVE_PARAM "is_active"
#define TRIGGERED_PARAM "is_triggered"
#define TRUE_STR "true"
#define FALSE_STR "false"
#define BOOL_TO_STR(b) (b ? TRUE_STR : FALSE_STR)
#define BLINDS_PARAM "blinds"

WiFiServer _server(80);
Application _app;
static void (*_alarm_down_callback)();
static void (*_alarm_up_callback)();
static void (*_alarm_up_updated_callback)();
static void (*_alarm_down_updated_callback)();
static void (*_stop_callback)();
static void (*_up_callback_with_blind)(BlindType blind_type);
static void (*_down_callback_with_blind)(BlindType blind_type);

static void on_up(Request &req, Response &res);
static void send_alarm_info(Response &res, Alarm* alarm);
static void on_up_trigger(Request &req, Response &res);
static void on_activate_up(Request &req, Response &res);
static void on_deactivate_up(Request &req, Response &res);
static void on_set_up_alarm(Request &req, Response &res);
static bool on_set_alarm(Request &req, Response &res, Alarm &alarm);
static void on_down(Request &req, Response &res);
static void on_down_trigger(Request &req, Response &res);
static void on_activate_down(Request &req, Response &res);
static void on_deactivate_down(Request &req, Response &res);
static void on_set_down_alarm(Request &req, Response &res);
static void on_get_status(Request &req, Response &res);
static void on_stop(Request &req, Response &res);
static bool str_to_time(char* str_time, Time* time);
static bool string_compare(const char* str1, const char* str2);
static void send_wrapped_response(Response &res, const char* status, char* data);
static void send_empty_success(Response &res);
static void send_fail(Response &res, const char* title);
static void set_content_type();
static BlindType get_blind_type_from_string(char* blind_str);

void init_api() {
    _app.get("/up/trigger", &on_up_trigger);
    _app.get("/up", &on_up);
    _app.get("/up/activate", &on_activate_up);
    _app.get("/up/deactivate", &on_deactivate_up);
    _app.put("/up", &on_set_up_alarm);
    _app.get("/down/trigger", &on_down_trigger);
    _app.get("/down", &on_down);
    _app.get("/down/activate", &on_activate_down);
    _app.get("/down/deactivate", &on_deactivate_down);
    _app.put("/down", &on_set_down_alarm);
    _app.get("/status", &on_get_status);
    _app.get("/stop", &on_stop);
    set_content_type();
    _server.begin();
}

void check_api() {
    WiFiClient client = _server.accept();
  
    if (client.connected()) {
        _app.process(&client);
    }
}

static void set_content_type() {
    char* contentType = (char*) "application/json";
    _app.header("Content-Type", contentType, sizeof(*contentType));
}

void set_on_up_api_callback(void (*callback)()) {
    _alarm_up_callback = callback;
}

void set_on_down_api_callback(void (*callback)()) {
    _alarm_down_callback = callback;
}


void set_on_down_updated_api_callback(void (*callback)()) {
    _alarm_down_updated_callback = callback;
}

void set_on_up_updated_api_callback(void (*callback)()) {
    _alarm_up_updated_callback = callback;
}

void set_on_stop_api_callback(void (*callback)()) {
    _stop_callback = callback;
}

void set_on_down_with_blind_api_callback(void (*callback)(BlindType blind_type)) {
    _down_callback_with_blind = callback;
}

void set_on_up_with_blind_api_callback(void (*callback)(BlindType blind_type)) {
    _up_callback_with_blind = callback;
}

static void on_up(Request &req, Response &res) {
    send_alarm_info(res, &state.alarm_up);
}

static void on_down(Request &req, Response &res) {
    send_alarm_info(res, &state.alarm_down);
}

static void on_stop(Request &req, Response &res) {
    _stop_callback();
    send_empty_success(res);
}

static void send_alarm_info(Response &res, Alarm* alarm) {
    const char* success = PSTR("success");
    char data[100];
    sprintf(data, PSTR("{ \"%s\": \"%d:%d\", \"%s\": \"%s\"}}"),
        TIME_PARAM, 
        alarm->time.hours, 
        alarm->time.minutes, 
        ACTIVE_PARAM, 
        BOOL_TO_STR(alarm->is_active));
    send_wrapped_response(res, success, data);
}

static void send_wrapped_response(Response &res, const char* success, char* data) {
    res.printf(PSTR("{\"status\": \"%s\", \"data\": %s }"), success, data);
}

static void on_up_trigger(Request &req, Response &res) {
    char blind_param[10]; 
    if (req.query(BLINDS_PARAM, blind_param, sizeof(blind_param))) {
        BlindType blind_type = get_blind_type_from_string(blind_param);
        _up_callback_with_blind(blind_type);
    } else {
        _alarm_up_callback();
    }
    send_empty_success(res);
}

static void on_down_trigger(Request &req, Response &res) {
    char blind_param[10]; 
    if (req.query(BLINDS_PARAM, blind_param, sizeof(blind_param))) {
        BlindType blind_type = get_blind_type_from_string(blind_param);
        _down_callback_with_blind(blind_type);
    } else {
        _alarm_down_callback();
    }
    send_empty_success(res);
}

static void on_activate_up(Request &req, Response &res) {
    state.alarm_up.is_active = true;
    send_empty_success(res);
}

static void on_activate_down(Request &req, Response &res) {
    state.alarm_down.is_active = true;
    send_empty_success(res);
}

static void on_deactivate_up(Request &req, Response &res) {
    state.alarm_down.is_active = false;
    send_empty_success(res);
}

static void on_deactivate_down(Request &req, Response &res) {
    state.alarm_down.is_active = false;
    send_empty_success(res);
}

static void send_empty_success(Response &res) {
    const char* success = PSTR("success");
    char* data = (char*) "{}";
    send_wrapped_response(res, success, data);
}

static void on_set_up_alarm(Request &req, Response &res) {
    if (on_set_alarm(req, res, state.alarm_up)) {
        _alarm_up_updated_callback();
    }
}

static void on_set_down_alarm(Request &req, Response &res) {
    if (on_set_alarm(req, res, state.alarm_down)) {
        _alarm_down_updated_callback();
    } 
}

static bool on_set_alarm(Request &req, Response &res, Alarm &alarm) { 
    char time[6];
    char is_active[6];

    if (req.query(TIME_PARAM, time, 6)) {
        Time tmp_time = {0, 0};
        
        if (!str_to_time(time, &tmp_time)) {
            send_fail(res, PSTR("Invalid time. Should be hh:mm"));
            return false;
        }
        alarm.time.hours = tmp_time.hours;
        alarm.time.minutes = tmp_time.minutes;
        alarm.triggered = false;
    }

    if (req.query(ACTIVE_PARAM, is_active, 6)) {
        
        if (string_compare(is_active, TRUE_STR)) {
            alarm.is_active = true;
        } else if (string_compare(is_active, FALSE_STR)) {
            alarm.is_active = false;
        } else {
            send_fail(res, PSTR("is_active must be a booelan"));
            return false;
        }
    }

    const char* status = PSTR("success");
    char data[100];
    sprintf(data, PSTR("{ \"%s\": \"%d:%d\", \"%s\": %s, \"%s\": %s }"), 
        TIME_PARAM, alarm.time.hours, alarm.time.minutes,
        ACTIVE_PARAM, BOOL_TO_STR(alarm.is_active),
        TRIGGERED_PARAM, BOOL_TO_STR(alarm.triggered));
    send_wrapped_response(res, status, data);
    return true;
}

static void send_fail(Response &res, const char* title) {
    res.status(400);
    const char* status = PSTR("fail");
    char data[50];
    sprintf(data, PSTR("{ \"title\": \"%s\" }"), title);
    send_wrapped_response(res, status, data);
}

static bool str_to_time(char* str_time, Time* time) {
    int i = 0;
    int cont_tmp = 0;
    char temp[3];

    while (str_time[i] != '\0') {
        if (str_time[i] == ':') {
            int hours = atoi(temp);

            if (hours < 0 || hours > 23) {
                return false;
            }
            time->hours = hours;
            cont_tmp = 0;
        } else {
            temp[cont_tmp] = str_time[i];
            cont_tmp++;
        } 
        i++;
    }
    int minutes = atoi(temp);

    if (minutes < 0 && minutes > 59) {
        return false;
    }
    time->minutes = minutes;
    return true;
}

static bool string_compare(const char* str1, const char* str2) {
    int ctr=0;

    while(str1[ctr]==str2[ctr]) {
        if(str1[ctr]=='\0' || str2[ctr]=='\0')
            break;
        ctr++;
    }
    return str1[ctr]=='\0' && str2[ctr]=='\0' ? true : false;
}

void on_get_status(Request &req, Response &res) {
    const char* status = PSTR("success");
    char data[300];
    sprintf(data, PSTR("{ \"day\": %d, \"current_time\": \"%d:%d\", \"alarm_up\": {\"%s\": \"%d:%d\", \"%s\": %s, \"%s\": %s}, \"alarm_down\": {\"%s\": \"%d:%d\", \"%s\": %s, \"%s\": %s}}"),
        state.day,
        state.current_time.hours,
        state.current_time.minutes,
        TIME_PARAM,
        state.alarm_up.time.hours,
        state.alarm_up.time.minutes,
        ACTIVE_PARAM,
        BOOL_TO_STR(state.alarm_up.is_active),
        TRIGGERED_PARAM,
        BOOL_TO_STR(state.alarm_up.triggered),
        TIME_PARAM,
        state.alarm_down.time.hours,
        state.alarm_down.time.minutes,
        ACTIVE_PARAM,
        BOOL_TO_STR(state.alarm_down.is_active),
        TRIGGERED_PARAM,
        BOOL_TO_STR(state.alarm_down.triggered));
    send_wrapped_response(res, status, data);
}

static BlindType get_blind_type_from_string(char* blind_str) {
  if (string_compare(blind_str, "left")) {
    return LEFT;
  } else if (string_compare(blind_str, "right")) {
    return RIGHT;
  } else if (string_compare(blind_str, "center")) {
    return CENTER;
  }
  return ALL;
}