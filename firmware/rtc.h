/* ========== rtc.h ==========
 * 
 * Logging RTC (on-IC) interface functions
 * Onboard RTC (external) communication interface
 * 
 * enum RTCMode declared in config.h
*/

#pragma once

#include "buffer.h"
#include "config.h"

#include "hardware/rtc.h"
#include "hardware/i2c.h"

#define RTC_I2C i2c1
#define RTC_I2C_BAUD 100000

/*! \brief Initialise the logging RTC depending on config.rtc_mode - will fetch current time from onboard RTC if necessary. */
static inline void log_rtc_init() {
    if(config.rtc_mode == NO_RTC) return;
    // initialise hardware RTC on RP2040
    rtc_init();
    // initialise the I2C channel for the external RTC
    i2c_init(RTC_I2C, RTC_I2C_BAUD);
    //fetch time
    // rtc_set_datetime()
}

void log_rtc_insert_timestamp(Buffer *buffer) {
    //rtc_get_datetime()
    //convert to string and insert into buffer
}