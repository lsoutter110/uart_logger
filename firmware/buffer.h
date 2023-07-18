#pragma once

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/sync.h"

#include <stdint.h>
#include <stdio.h>

#include "error.h"
#include "config.h"

#define BUFFER_SIZE 1024
#define BUFFER_THRESHOLD 8
#define SPINLOCK_OFFSET 14

/*! \brief Temporary data buffer, implemented as a circular queue. */
typedef struct {
    uint32_t head;
    uint32_t tail;
    uint64_t b1;
    spin_lock_t *lock;
    uint64_t b2;
    char data[BUFFER_SIZE];
} Buffer;

/*! \brief Initialise all data buffers and claim spinlocks.
 * \param buffers buffer array
 */
static inline void buffer_init_all(Buffer *buffers) {
    for(int i=0; i<10; i++) {
        buffers[i].head = 0;
        buffers[i].tail = 0;
        buffers[i].lock = spin_lock_init(SPINLOCK_OFFSET + i);
        spin_lock_claim(SPINLOCK_OFFSET + i);
        printf("claimed spinlock %d\n", i);
    }
}

/*! \brief Push a character to buffer.
 * \param buffer pointer to buffer
 * \param c character to push
 */
void buffer_push_char(Buffer *buffer, char c) {
    // printf("pushing %c to buffer %d\n", c, buffer_index);
    uint32_t save_irq = spin_lock_blocking(buffer->lock);

    //write to buffer and increment head
    buffer->data[buffer->head++] = c;
    if(buffer->head > BUFFER_SIZE-1)
        buffer->head = 0; //wrap head

    //check if buffer is full (guaranteed to be >0 items long)
    if(buffer->head == buffer->tail) {
        if(config.overflow_error_cont) {
            buffer->tail++; //discard oldest item
            if(buffer->tail > BUFFER_SIZE-1)
                buffer->tail = 0; //wrap tail
        } else {
            handle_error(BUFFER_OVERFLOW); //buffer overflow - kill execution with error
        }
    }
    spin_unlock(buffer->lock, save_irq);
}

/*! \brief Flush a buffer to the SD card. - CHANGE BACK TO PASSING IN BUFFER
 * \param buffer pointer to buffer
 * \param ignore_lock flag to ignore spinlock - should only be true if the other core is definitely not accessing the buffer
*/
void buffer_flush_to_sd(Buffer *buffer, bool ignore_lock) {
    //open file
    printf("flushing buffer\n");
    //claim spinlock
    uint32_t save_irq;
    if(!ignore_lock)
        save_irq = spin_lock_blocking(buffer->lock);
    printf("locked\n");
    while(buffer->head != buffer->tail) {
        // putchar(buffer->data[buffer->tail++]);
        buffer->tail++;
        if(buffer->tail > BUFFER_SIZE-1)
            buffer->tail = 0;
    }
    printf("\nflushed\n");
    //unlock spinlock
    if(!ignore_lock)
        spin_unlock(buffer->lock, save_irq);
    printf("unlocked\n");
}