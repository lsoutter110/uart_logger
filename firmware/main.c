#include "pico/stdlib.h"
#include "pico/multicore.h"
// #include "hardware/pio.h"
#include "hardware/uart.h"

#include <stdint.h>
#include <stdio.h>

#include "buffer.h"
#include "error.h"
#include "rtc.h"

//assembled PIO
// #include "pio_test.pio.h"

Buffer buffers[10];

// PORT SIDE CORE

/*! \brief Core 1 main function - UART port controller. */
void core1_main() {
    printf("enter core 1\n");
    //PIO Controller
    // const Config config = *(Config*)multicore_fifo_pop_blocking();
    // Buffer *buffers = (Buffer*)multicore_fifo_pop_blocking();
    printf("recv bufptr = %u\n", buffers);
    printf("(%d)", buffers[6].tail);
    printf("buffers init\n");
    
    //init uarts
    gpio_init(4);
    gpio_init(5);
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);
    // uart_init(uart0, config.a_baud);
    uart_init(uart1, config.b_baud);
    printf("uart init\n");

    //setup PIOs

    // port reader main loop
    for(;;) {
        if(multicore_fifo_rvalid()) {
            // ERROR
            while(1);
        }
        //read RX 0, 1
        // if(uart_is_readable(uart0))
            // push_to_buffer(uart_getc(uart0), buffers, config.overflow_error_cont);
        if(config_port_is_enabled(config, 1) && uart_is_readable(uart1))
            buffer_push_char(&buffers[1], uart_getc(uart1));

        // push_to_buffer('L', buffers+3, config.overflow_error_cont);
        // printf("pushing to buffer\n");
        // sleep_ms(500);
        //read PIOs
    }
}

// INITIALISATION AND STORAGE SIDE CORE
/*! \brief Core 0 main function - initialisation routine and storage controller. */
int main() {
    stdio_init_all();
    //init LED
    ws2812_init();
    
    //Setup from SD card
    config_parse();
    printf("Config parsed\n");

    //set up buffers
    // Buffer buffers[10];
    buffer_init_all(buffers);
    printf("Buffers setup\n");
    printf("{%d}", buffers[6].tail);

    //initialise RTC
    log_rtc_init();
    printf("RTC setup\n");
    printf("{%d}", buffers[6].tail);

    //start core 1
    multicore_launch_core1(core1_main);
    printf("{%d}", buffers[6].tail);

    //push data to core1
    printf("send bufptr = %u\n", buffers);
    // multicore_fifo_push_blocking((uintptr_t)&config);
    // multicore_fifo_push_blocking((uintptr_t)buffers);
    printf("{%d}", buffers[6].tail);
    printf("Core 1 started\n");
    // handle_error(BUFFER_OVERFLOW);

    printf("{%d}", buffers[6].tail);
    // SD writer main loop
    for(;;) {
        // test stack for error
        if(multicore_fifo_rvalid()) {
            // ERROR
            // flush all buffers if possible
            for(Buffer *buffer=buffers; buffer<buffers+10; buffer++)
                buffer_flush_to_sd(buffer, true);

            while(1);
        }
        //test buffers
        printf("Buffer sizes: ");
        Buffer *buffer = buffers;
        for(int port=0; port<10; port++, buffer++) {
            // skip if port is disabled
            if(!config_port_is_enabled(config, port))
                continue;

            printf("{%d}", buffers[6].tail);
            int bufsize = buffer->head - buffers->tail;
            //if buffer not full enough
            printf("[%d, %d]%d, ", buffer->head, buffer->tail, bufsize);
            if((bufsize<0 ? bufsize+BUFFER_SIZE : bufsize) < BUFFER_THRESHOLD)
                continue;
            //buffer > threshold
            buffer_flush_to_sd(buffer, true);
        }
        printf("\n");
        sleep_ms(500);
        // printf("loop\n");
    }
}