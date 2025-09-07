#include "Arduino.h"
#include "ui.h"
#define MAX_MILLIS_NO_INTERACTION 10000

enum Screen {
  TIME,
  UP,
  DOWN,
  MENU_SET_DOWN_TIME,
  MENU_SET_UP_TIME,
  SET_ACTIVATE_UP,
  SET_ACTIVATE_DOWN,
  ACTIVATE_UP,
  ACTIVATE_DOWN,
  ANIMATION_UP,
  ANIMATION_DOWN,
  SET_UP_TIME,
  ANIMATION_STOP,
  SET_DOWN_TIME,
  DELETE_ALARMS
}; 

enum MenuType {
    TEXT,
    SELECTABLE,
    ANIMATION,
    ONE_TIME_MOVING,
};

struct MenuItem {
  Screen screen;
  MenuType type;
  char* text;
  unsigned int text_length; // Dots '.' do not count!!
  MenuItem* next;
};

static MenuItem menu_time;
static MenuItem menu_up;
static MenuItem menu_down;
static MenuItem menu_set_up_time;
static MenuItem menu_set_down_time;
static MenuItem menu_animate_up;
static MenuItem menu_animate_down;
static MenuItem menu_setting_down_time;
static MenuItem menu_setting_up_time;
static MenuItem menu_activate_set_up;
static MenuItem menu_activate_up;
static MenuItem menu_activate_set_down;
static MenuItem menu_activate_down;
static MenuItem menu_animate_stop;

struct UiState {
  bool should_update_text = true;
  bool is_blinking = true;
  unsigned int text_pos = 0;
  unsigned long last_interaction = 0;
  unsigned long last_millis_moving_text = 0;
  char selected_mask = B0000;
  MenuItem* current_screen = &menu_time;
};

static UiState _ui_state = UiState {};
static Display* _segment_display;
static void (*up_persiana)();
static void (*down_persiana)();
static void (*_store_alarm_down)();
static void (*_store_alarm_up)();

static char* time_to_char(Time* time);
static void reset_text_state();
static void show_moving_text(char* text, unsigned int max_position);
static void change_screen(MenuItem* next_screen);
static void next_screen();
static void check_if_should_update_ui();
static void show_annimation();
static void show_menu_text();
static void manage_blink();
static void set_initial_select_mask();
static void set_activate_initial_select_mask(Alarm* alarm);
static void set_alarm_time(Time* alarm_time);
static void on_select_press_menu_activate_up();
static void on_select_press_menu_activate_down();
static void on_select_press_alarm_set(MenuItem* menu_item, Time* alarm_time);
static void on_next_press_setting_time();
static void on_next_press_activate_screen();
static void on_select_press_menu_activate_up();
static void on_select_press_menu_activate_down();
static void initialise_menu();
static void change_to_time_if_no_interaction();

void init_ui(Display* _display) {
    _segment_display = _display;
    initialise_menu();
}

void set_on_ui_down_persiona_callback(void (*callback)()) {
    down_persiana = callback;
}

void set_on_ui_up_persiona_callback(void (*callback)()) {
    up_persiana = callback;
}

void set_on_alarm_down_set_callback(void (*callback)()) {
  _store_alarm_down = callback;
}

void set_on_alarm_up_set_callback(void (*callback)()) {
  _store_alarm_up = callback;
}

void on_time_updated_ui() {
    free(menu_time.text);
    menu_time.text = time_to_char(&state.current_time);

    if (_ui_state.current_screen->screen == TIME) {
      _ui_state.should_update_text = true;
    }
}

void on_alarm_up_updated_ui() {
    free(menu_setting_up_time.text);
    menu_setting_up_time.text = time_to_char(&state.alarm_up.time);
}

void on_alarm_down_updated_ui() {
    free(menu_setting_down_time.text);
    menu_setting_down_time.text = time_to_char(&state.alarm_down.time);
}

void show_ui() {
    change_to_time_if_no_interaction();
    check_if_should_update_ui();
    
    if (_ui_state.should_update_text) {
        _ui_state.last_millis_moving_text = millis();
        _ui_state.should_update_text = false;

        switch (_ui_state.current_screen->type) {
            case TEXT:
                show_menu_text();
                break;

            case ANIMATION:
                show_annimation();
                break;

            case SELECTABLE:
                if (_ui_state.is_blinking) {
                    manage_blink();
                } else {
                    show_menu_text();
                }
                break;

            case ONE_TIME_MOVING:
                if (_ui_state.text_pos == 0) {
                    next_screen();
                } else {
                    show_menu_text();
                }
                break;
        }
    }
    _segment_display->refresh();
}

void on_next_press() {
    _ui_state.last_interaction = millis();

    if (_ui_state.current_screen->type == SELECTABLE) {
        if (_ui_state.current_screen->screen == SET_DOWN_TIME 
                || _ui_state.current_screen->screen == SET_UP_TIME) {
            on_next_press_setting_time();
        } else if (_ui_state.current_screen->screen == SET_ACTIVATE_UP 
                || _ui_state.current_screen->screen == SET_ACTIVATE_DOWN) {
            on_next_press_activate_screen();
        }
    } else {
        next_screen();
    }
}

void on_select_press() {
    _ui_state.last_interaction = millis();
    
    switch (_ui_state.current_screen->screen) {
        case TIME:
            break;
        
        case MENU_SET_DOWN_TIME:
            change_screen(&menu_setting_down_time);
            break;
        
        case MENU_SET_UP_TIME:
            change_screen(&menu_setting_up_time);
            break;
        
        case SET_ACTIVATE_UP: 
            on_select_press_menu_activate_up();
            break;

        case SET_ACTIVATE_DOWN: 
            on_select_press_menu_activate_down();
            break;

        case SET_DOWN_TIME:
            on_select_press_alarm_set(&menu_setting_down_time, &state.alarm_down.time);
            break;

        case SET_UP_TIME:
            on_select_press_alarm_set(&menu_setting_up_time, &state.alarm_up.time);
            break;

        case UP:
            up_persiana();
            change_screen(&menu_animate_up);
            break;

        case DOWN:
            down_persiana();
            change_screen(&menu_animate_down);
            break;

        case ANIMATION_DOWN:
        case ANIMATION_UP:
        case ANIMATION_STOP:
        case DELETE_ALARMS:
        case ACTIVATE_DOWN:
        case ACTIVATE_UP:
            break;
    }
}

void show_up_animation() {
    _ui_state.last_interaction = millis();
    change_screen(&menu_animate_up);
}

void show_down_animation() {
    _ui_state.last_interaction = millis();
    change_screen(&menu_animate_down);
}

void show_stop_animation() {
    _ui_state.last_interaction = millis();
    change_screen(&menu_animate_stop);
}

static void initialise_menu() {
    menu_time = { TIME, TEXT, time_to_char(&state.current_time), 4, &menu_up };
    menu_up = { UP, TEXT, (char*) "    pujar persiana   ", 22, &menu_down, };
    menu_down = { DOWN, TEXT, (char*) "    baixar persiana   ", 22, &menu_set_up_time, };
    menu_set_up_time = { MENU_SET_UP_TIME, TEXT, (char*) "    definir hora pujada   ", 27, &menu_set_down_time};
    menu_set_down_time = { MENU_SET_DOWN_TIME, TEXT, (char*) "    definir hora baixada   ", 28, &menu_time };
    menu_animate_up = { ANIMATION_UP, ANIMATION, (char*) "8888AAAA****^^^^~~~~    ", 0, &menu_time };
    menu_animate_down = { ANIMATION_DOWN, ANIMATION, (char*) "    ~~~~^^^^****AAAA8888", 0, &menu_time};
    menu_animate_stop = { ANIMATION_STOP, ANIMATION, (char*) "    ~~~~^^^^STOP^^^^~~~~     ", 29, &menu_time};
    menu_setting_down_time = { SET_DOWN_TIME, SELECTABLE, time_to_char(&state.alarm_down.time), 4, &menu_activate_down};
    menu_setting_up_time = { SET_UP_TIME, SELECTABLE, time_to_char(&state.alarm_up.time), 4, &menu_activate_up};
    menu_activate_set_up = { SET_ACTIVATE_UP, SELECTABLE, (char*) "si.no", 4, &menu_time};
    menu_activate_up = { ACTIVATE_UP, ONE_TIME_MOVING, (char*) "    Activate   ", 16, &menu_activate_set_up};
    menu_activate_set_down = { SET_ACTIVATE_DOWN, SELECTABLE, (char*) "si.no", 4, &menu_time};
    menu_activate_down = { ACTIVATE_DOWN, ONE_TIME_MOVING, (char*) "    Activate   ", 16, &menu_activate_set_down};
}

static void set_initial_select_mask() {
    switch (_ui_state.current_screen->screen)
    {
        case SET_ACTIVATE_DOWN:
            set_activate_initial_select_mask(&state.alarm_down);
            break;

        case SET_ACTIVATE_UP:
            set_activate_initial_select_mask(&state.alarm_up);
            break;

        case SET_DOWN_TIME:
        case SET_UP_TIME:
            _ui_state.selected_mask = B1000; 
        
        default:
            break;
    }
}

static void set_activate_initial_select_mask(Alarm* alarm) {
    if (alarm->is_active) {
        _ui_state.selected_mask = B1100;
    } else {
        _ui_state.selected_mask = B0011;
    }
}

static void manage_blink(){
    int text_size = sizeof(_ui_state.current_screen->text);
    char* text = (char*) malloc(text_size);
    int count = 0;
    int shift_mask = B1000;

    if (_ui_state.selected_mask == 0) {
        set_initial_select_mask();
    }

    while (_ui_state.current_screen->text[count] != '\0') {
        if (_ui_state.current_screen->text[count] != '.') {
            if (_ui_state.selected_mask & shift_mask) {
                text[count] = ' ';
            } else {
                text[count] = _ui_state.current_screen->text[count];
            }
            shift_mask = shift_mask >> 1;
        } else {
            text[count] = _ui_state.current_screen->text[count];
        }
        count++;
    }
    text[count] = '\0';
    _segment_display->set_text(text);
    free(text);
}

static void show_menu_text() {
  if (_ui_state.current_screen->text_length > 4) {
    show_moving_text(_ui_state.current_screen->text, _ui_state.current_screen->text_length);
  } else {
    _segment_display->set_text(_ui_state.current_screen->text);
  }
}

static void show_moving_text(char* text, unsigned int max_position) {
    char to_print[5] = { text[_ui_state.text_pos], text[_ui_state.text_pos+1], text[_ui_state.text_pos+2], text[_ui_state.text_pos+3], '\0'};
    _segment_display->set_text(to_print);

    if (_ui_state.text_pos == max_position - 4) {
        _ui_state.text_pos = 0;
    } else {
        _ui_state.text_pos++;
    }
}

static char* time_to_char(Time* time) {
    char* new_text = (char*) malloc(8);
    sprintf(new_text, "%02d.%02d", time->hours, time->minutes);
    return new_text;
}

static void change_to_time_if_no_interaction() {
    if (_ui_state.current_screen->screen != TIME
            && millis() - _ui_state.last_interaction > MAX_MILLIS_NO_INTERACTION) {
        change_screen(&menu_time);
    }
}  

static void check_if_should_update_ui() {
    if (!_ui_state.should_update_text) {
        _ui_state.should_update_text = _ui_state.current_screen->screen != TIME 
            && (millis() - _ui_state.last_millis_moving_text > 200);

        if (_ui_state.current_screen->type == SELECTABLE && _ui_state.should_update_text) {
            _ui_state.is_blinking = !_ui_state.is_blinking;
        }
    }
}

static void show_annimation() {
    bool is_annimation_finished = _ui_state.current_screen->text[_ui_state.text_pos] == '\0';

    if (is_annimation_finished) {
        next_screen();
        return;
        }
    const char text[] = { 
        _ui_state.current_screen->text[_ui_state.text_pos],
        _ui_state.current_screen->text[_ui_state.text_pos + 1],
        _ui_state.current_screen->text[_ui_state.text_pos + 2],
        _ui_state.current_screen->text[_ui_state.text_pos + 3],
        '\0'
    };
    _segment_display->set_text(text);
    _ui_state.text_pos += 4;
}

static void set_alarm_time(Time* alarm_time) {
    if (_ui_state.selected_mask == B1000) {
      if (alarm_time->hours >= 20) {
        alarm_time->hours -= 20;
      } else {
        alarm_time->hours += 10;
      }
    } else if (_ui_state.selected_mask == B0100) {
      if (alarm_time->hours % 10 == 9) {
        alarm_time->hours -= 9;
      } else {
        alarm_time->hours += 1;
      }
    } else if (_ui_state.selected_mask == B0010) {
      if (alarm_time->minutes >= 50) {
        alarm_time->minutes -= 50;
      } else {
        alarm_time->minutes += 10;
      }
    } else if (_ui_state.selected_mask == B0001) {
      if (alarm_time->minutes % 10 == 9) {
        alarm_time->minutes -= 9;
      } else {
        alarm_time->minutes += 1;
      }
    }
}

static void change_screen(MenuItem* next_screen) {
    _ui_state.current_screen = next_screen;
    reset_text_state();
}

static void next_screen() {
    _ui_state.current_screen = _ui_state.current_screen->next;
    reset_text_state();
}

static void reset_text_state() {
  _ui_state.selected_mask = B0000;
  _ui_state.text_pos = 0;
  _ui_state.should_update_text = true;
}

// On select press
static void on_select_press_alarm_set(MenuItem* menu_item, Time* alarm_time) {
    _ui_state.should_update_text = true;
    set_alarm_time(alarm_time);
    free(menu_item->text);
    menu_item->text = time_to_char(alarm_time);
}

static void on_select_press_menu_activate_up() {
    state.alarm_up.is_active = _ui_state.selected_mask == B1100;
    _store_alarm_up();
    next_screen();
}

static void on_select_press_menu_activate_down() {
    state.alarm_down.is_active = _ui_state.selected_mask == B1100;
    _store_alarm_down();
    next_screen();
}

// On next press
static void on_next_press_setting_time() {
    _ui_state.selected_mask = _ui_state.selected_mask >> 1;

    if (_ui_state.selected_mask == 0) {
        next_screen();
    }
}

static void on_next_press_activate_screen() {
    _ui_state.selected_mask = _ui_state.selected_mask >> 2;

    if (_ui_state.selected_mask == 0) {
        _ui_state.selected_mask = B1100;
    }
}
