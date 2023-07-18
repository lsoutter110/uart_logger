/* ========== config.h ==========
 * 
 * Logger configuration setup code
 * 
 * Parses code from file 'config' on SD card
 * If 'config' file does not exist, replace with default values
*/

#pragma once

#define config_port_is_enabled(config, port) (config.enabled_ports & 1<<port)

typedef enum {
    NO_RTC = 0,
    NORM_RTC,
    MILLIS_RTC,
} RTCMode;

typedef struct {
    int a_baud; // UART 0
    int b_baud; // UART 1
    int c_baud; // UART 2:5
    int d_baud; // UART 6:9
    uint8_t overflow_error_cont; //if 1: continue if overflow error (data is lost)
    RTCMode rtc_mode;
    uint enabled_ports;
} Config;

Config config;

static inline void config_parse() {
    config.a_baud = 9600;
    config.b_baud = 115200;
    config.c_baud = 9600;
    config.d_baud = 115200;
    config.overflow_error_cont = false;
    config.rtc_mode = NO_RTC;
    config.enabled_ports = 0x000003FF;
}