/* ========== sd.h ==========
 * 
 * SPI SD Card driver
 *
 * Exposes a basic interface for R/W operations to an SD card
 * 
*/

// includes
#include <pico/stdlib.h>
#include <hardware/spi.h>

// macros
#define SD_SPI spi0
#define SD_SPI_MOSI 11
#define SD_SPI_MISO 8
#define SD_SPI_CLK 10
#define SD_SPI_CS 9

// data structures


// functions
static inline void sd_init() {
    // initialise SPI interface
    gpio_init(SD_SPI_MOSI);
    gpio_init(SD_SPI_MISO);
    gpio_init(SD_SPI_CLK);
    gpio_init(SD_SPI_CS);

    gpio_set_function(SD_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SD_SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SD_SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(SD_SPI_CS, GPIO_FUNC_SPI);

    spi_init(SD_SPI, 1?1:1);
}