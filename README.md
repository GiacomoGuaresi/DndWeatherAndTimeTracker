# FIX 1
https://github.com/Bodmer/TFT_eSPI/issues/3304
per il problema di loop bisogna usare la versione 2.0.14 delle board ESP32 

# TFT_eSPI > User_Setup.h configurations 
#define SPI_FREQUENCY  80000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
#define GC9A01_DRIVER        // ST7789_DRIVER o GC9A01_DRIVER se il tuo LCD è tondo

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define TFT_SCLK  6   // SCL
#define TFT_MOSI  7   // SDA
#define TFT_DC    2   // DC
#define TFT_CS   10   // CS
#define TFT_RST  -1   // RST

#define TFT_BL    3   // BACKLIGHT

