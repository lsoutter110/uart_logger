/* ========== error.h ==========
 * 
 * Error handler and WS2812 RGB LED driver
 * 
*/

#pragma once

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define WS2812_PIN 0

/*! \brief Error codes, the value is a colour in GRB format. */
typedef enum {
    SD_NO_DETECT    = 0x0000FF00, // red
    SD_READ_ERR     = 0x00FFFF00, // yellow
    SD_WRITE_ERR    = 0x00FFFF00, // yellow
    CONFIG_ERR      = 0x0080FF00, // orange
    UART_BAD_BYTE   = 0x000000FF, // blue
    BUFFER_OVERFLOW = 0x0000FFFF, // purple
} ErrCode;

/*! \brief Sets the colour of a single WS2812 RGB LED on pin WS2812_PIN, using a bitbanged protocol.
 * \param data the colour in GRB format
*/
void ws2812_set_colour(uint32_t data) {
    //takes a colour and writes it to the pin specified 
    //TODO : disable interrupts
    for(uint32_t i=1<<23; i!=0; i>>=1) {
        if(i & data) {
            gpio_put(WS2812_PIN, 1);
            busy_wait_at_least_cycles(90);
            gpio_put(WS2812_PIN, 0);
            busy_wait_at_least_cycles(50); //can get away with 50ns less
        } else {
            gpio_put(WS2812_PIN, 1);
            busy_wait_at_least_cycles(40);
            gpio_put(WS2812_PIN, 0);
            busy_wait_at_least_cycles(100); //can get away with 50ns less
        }
    }
}

/*! \brief Initialises the WS2812 LED on pin WS2812_PIN. */
static inline void ws2812_init() {
    gpio_init(WS2812_PIN);
    gpio_set_dir(WS2812_PIN, GPIO_OUT);
    sleep_ms(50);
    ws2812_set_colour(0x00FF0000);
}

/*! \brief Handle an error and stop program execution.
 * \param errcode Error code
 */
void handle_error(ErrCode errcode) {
    multicore_fifo_push_blocking(errcode);
    ws2812_set_colour(errcode);
    while(1);
}