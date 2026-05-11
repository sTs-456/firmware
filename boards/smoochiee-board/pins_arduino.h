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
#define GPS_SERIAL_TX SERIAL_TX
#define GPS_SERIAL_RX SERIAL_RX

// ============================================================================
// 📌 I2C (DIPINDAH dari 47/48 karena bentrok dengan USB-CDC)
// ============================================================================
#define GROVE_SDA 19
#define GROVE_SCL 20
static const uint8_t SDA = 19;
static const uint8_t SCL = 20;

// ============================================================================
// 📌 DUAL SPI CONFIGURATION (CRITICAL FOR NO-CS TFT)
// ============================================================================
// VSPI (SPI2) -> DEDICATED FOR ST7789 240x240 (NO CS PIN)
#define TFT_SPI_HOST   SPI2_HOST
#define TFT_SCLK       13
#define TFT_MOSI       11
#define TFT_MISO       -1  // Write-only
#define TFT_CS         -1  // ✅ NO CS PIN

// HSPI (SPI3) -> SHARED FOR CC1101, NRF24, SD CARD
#define MODULE_SPI_HOST SPI3_HOST
#define SPI_SCK_PIN    18
#define SPI_MOSI_PIN   17
#define SPI_MISO_PIN   8
#define SPI_SS_PIN     3

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
#define CC1101_GDO2_PIN  -1 // Non-aktifkan biar gak bentrok Mic
#define CC1101_SS_PIN    46  // ✅ CS Unik di HSPI
#define CC1101_MOSI_PIN  SPI_MOSI_PIN
#define CC1101_SCK_PIN   SPI_SCK_PIN
#define CC1101_MISO_PIN  SPI_MISO_PIN

// ============================================================================
// 📌 NRF24 (2.4GHz RF)
// ============================================================================
#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN     14
#define NRF24_SS_PIN     42  // ✅ CS Unik di HSPI
#define NRF24_MOSI_PIN   SPI_MOSI_PIN
#define NRF24_SCK_PIN    SPI_SCK_PIN
#define NRF24_MISO_PIN   SPI_MISO_PIN

// ============================================================================
// 📌 DISPLAY (ST7789 240x240 - NO CS)
// ============================================================================
#define HAS_SCREEN 1
#define ROTATION 0
#define MINBRIGHT (uint8_t)30
#define USER_SETUP_LOADED 1
#define ST7789_DRIVER 1
#define TFT_RGB_ORDER 0
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
#define TFT_BACKLIGHT_ON 1
#define TFT_BL       5
#define TFT_RST      16
#define TFT_DC       15
#define TFT_MISO     -1
#define TFT_MOSI     TFT_MOSI
#define TFT_SCLK     TFT_SCLK
#define TFT_CS       -1
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
#define MIC_INMP441
#define PIN_CLK  1
#define PIN_DATA 12  // Ganti ke 12 biar gak bentrok CC1101_GDO0
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
