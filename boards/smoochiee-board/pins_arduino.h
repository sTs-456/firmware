#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// ============================================================================
// 📌 UART / Serial
// ============================================================================
static const uint8_t TX = 1;
static const uint8_t RX = 2;
#define SERIAL_RX 2
#define SERIAL_TX 1
#define BAD_RX SERIAL_RX
#define BAD_TX SERIAL_TX

// ============================================================================
// 📌 I2C (Safe pins,避开 USB-CDC D+/D-)
// ============================================================================
#ifndef GROVE_SDA
#define GROVE_SDA 19
#endif
#ifndef GROVE_SCL
#define GROVE_SCL 20
#endif
static const uint8_t SDA = GROVE_SDA;
static const uint8_t SCL = GROVE_SCL;

// ============================================================================
// 📌 DEFAULT SPI PINS (WAJIB ADA BIAR SPI.cpp ESP32 CORE COMPILE)
// Dipetakan ke HSPI (Bus Modules/SD/RF)
// ============================================================================
static const uint8_t SS   = 3;
static const uint8_t MOSI = 17;
static const uint8_t MISO = 8;
static const uint8_t SCK  = 18;

// ============================================================================
// 📌 MODULES SPI (HSPI/SPI3) - Shared Bus
// ============================================================================
#define MODULE_SPI_HOST SPI3_HOST
#define SPI_SCK_PIN     SCK
#define SPI_MOSI_PIN    MOSI
#define SPI_MISO_PIN    MISO
#define SPI_SS_PIN      SS

// ============================================================================
// 📌 TFT SPI (VSPI/SPI2) - Dedicated for ST7789 240x240 (NO CS)
// ============================================================================
#define TFT_SPI_HOST    SPI2_HOST
#define TFT_SCLK        13
#define TFT_MOSI        11
#define TFT_MISO        -1
#define TFT_CS          -1

// ============================================================================
// 📌 BUTTONS
// ============================================================================
#define BTN_ALIAS "\"OK\""
#define HAS_5_BUTTONS
#define SEL_BTN 21
#define UP_BTN  41
#define DW_BTN  40
#define R_BTN   38
#define L_BTN   39
#define BTN_ACT LOW

// ============================================================================
// 📌 LEDs / IR
// ============================================================================
#define RXLED 4
#define TXLED 5
#define LED_ON  HIGH
#define LED_OFF LOW

// ============================================================================
// 📌 CC1101 (Sub-GHz RF)
// ============================================================================
#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN  9
#define CC1101_GDO2_PIN  -1
#define CC1101_SS_PIN    46
#define CC1101_MOSI_PIN  SPI_MOSI_PIN
#define CC1101_SCK_PIN   SPI_SCK_PIN
#define CC1101_MISO_PIN  SPI_MISO_PIN

// ============================================================================
// 📌 NRF24 (2.4GHz RF)
// ============================================================================
#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN     14
#define NRF24_SS_PIN     42
#define NRF24_MOSI_PIN   SPI_MOSI_PIN
#define NRF24_SCK_PIN    SPI_SCK_PIN
#define NRF24_MISO_PIN   SPI_MISO_PIN

// ============================================================================
// 📌 DISPLAY (ST7789 240x240 - NO CS)
// ============================================================================
#ifndef HAS_SCREEN
#define HAS_SCREEN 1
#endif
#define ROTATION 0
#ifndef MINBRIGHT
#define MINBRIGHT (uint8_t)30
#endif
#define USER_SETUP_LOADED 1
#define ST7789_DRIVER 1
#define TFT_RGB_ORDER 0
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
#define TFT_BACKLIGHT_ON 1
#define TFT_BL       5
#define TFT_RST      16
#define TFT_DC       15
// TFT_MOSI & TFT_SCLK sudah didefinisikan di block VSPI di atas
#define TOUCH_CS     -1
#define SMOOTH_FONT  1
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 20000000

// ============================================================================
// 📌 SD CARD (On HSPI)
// ============================================================================
#define SDCARD_CS    3
#define SDCARD_SCK   SPI_SCK_PIN
#define SDCARD_MISO  SPI_MISO_PIN
#define SDCARD_MOSI  SPI_MOSI_PIN

// ============================================================================
// 📌 POWER & AUDIO
// ============================================================================
#define XPOWERS_CHIP_BQ25896
#define USE_BOOST
#ifndef MIC_INMP441
#define MIC_INMP441
#endif
#define PIN_CLK  1
#define PIN_DATA 12
#define PIN_WS   2

// ============================================================================
// 📌 RGB LED
// ============================================================================
#define HAS_RGB_LED 1
#define RGB_LED 45
#define LED_TYPE WS2812B
#define LED_ORDER GRB
#define LED_COUNT 16
#define LED_COLOR_STEP 15

#endif /* Pins_Arduino_h */
