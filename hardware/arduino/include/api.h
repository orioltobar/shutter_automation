#ifndef API_H
#define API_H

void init_api();
void check_api();
void set_on_up_api_callback(void (*callback)());
void set_on_down_api_callback(void (*callback)());
void set_on_down_updated_api_callback(void (*callback)());
void set_on_up_updated_api_callback(void (*callback)());

#endif