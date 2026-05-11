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
#define USB_as_HID 1

// ============================================================================
// 📌 I2C / GROVE PORT (WAJIB UNCOMMENT - buat PMU BQ25896 & sensor)
// ============================================================================
static const uint8_t SDA = 47;
static const uint8_t SCL = 48;
#define GROVE_SDA 47    // ✅ UNCOMMENTED - I2C SDA
#define GROVE_SCL 48    // ✅ UNCOMMENTED - I2C SCL

// ============================================================================
// 📌 SPI Global (Shared bus untuk Display, SD, CC1101, NRF24)
// ============================================================================
static const uint8_t SS   = 3;
static const uint8_t MOSI = 17;
static const uint8_t MISO = 8;
static const uint8_t SCK  = 18;

#define SPI_SCK_PIN  SCK
#define SPI_MOSI_PIN MOSI
#define SPI_MISO_PIN MISO
#define SPI_SS_PIN   SS

// ============================================================================
// 📌 BUTTONS (5-way + Select)
// ============================================================================
#define BTN_ALIAS "\"OK\""
#define HAS_5_BUTTONS
#define SEL_BTN 21    // ✅ GPIO 21 (bukan 0, aman dari boot conflict)
#define UP_BTN  41
#define DW_BTN  40
#define R_BTN   38
#define L_BTN   39
#define BTN_ACT LOW   // Active LOW (tekan = LOW)

// ============================================================================
// 📌 LED Indicators
// ============================================================================
#define RXLED 4       // ✅ UNCOMMENTED - IR RX indicator
#define TXLED 5       // ✅ UNCOMMENTED - TX indicator
#define LED_ON  HIGH
#define LED_OFF LOW

// ============================================================================
// 📌 CC1101 (Sub-GHz RF) - Via SPI Shared Bus
// ============================================================================
#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN 9     // Interrupt pin 0
#define CC1101_GDO2_PIN 10    // Interrupt pin 2 (opsional)
#define CC1101_SS_PIN   13    // ✅ CS pin unik (bukan share dengan SD/Display)
#define CC1101_MOSI_PIN SPI_MOSI_PIN  // Share MOSI (GPIO 17)
#define CC1101_SCK_PIN  SPI_SCK_PIN   // Share SCK (GPIO 18)
#define CC1101_MISO_PIN SPI_MISO_PIN  // Share MISO (GPIO 8)

// ============================================================================
// 📌 NRF24 (2.4GHz RF) - Via SPI Shared Bus
// ============================================================================
#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN  14    // ✅ CE pin (Chip Enable)
#define NRF24_SS_PIN  42    // ✅ CSN pin unik (bukan conflict dengan yang lain)
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN  SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

// ============================================================================
// 📌 DISPLAY CONFIG (ST7789 240x240 - NO CS PIN)
// ============================================================================
#define HAS_SCREEN 1
#define ROTATION 0
#define MINBRIGHT (uint8_t)1

#define USER_SETUP_LOADED 1
#define ST7789_DRIVER 1
#define TFT_RGB_ORDER 0
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
#define TFT_BACKLIGHT_ON 1

#define TFT_BL   5     // ✅ Backlight PWM (GPIO 5)
#define TFT_RST  16    // Reset pin
#define TFT_DC   15    // Data/Command pin
#define TFT_CS   -1    // ✅ NO CS PIN (mode 3-wire SPI / always selected)
#define TFT_MISO -1    // Tidak dipakai (write-only display)
#define TFT_MOSI 17    // Share SPI MOSI
#define TFT_SCLK 18    // Share SPI SCK
#define TFT_SPI_HOST 2 // HSPI

#define TOUCH_CS -1
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

// ============================================================================
// 📌 SD CARD (Share SPI Bus)
// ============================================================================
#define SDCARD_CS   3     // ✅ CS pin SD Card (GPIO 3 = global SS)
#define SDCARD_SCK  18    // Share SPI SCK
#define SDCARD_MISO 8     // Share SPI MISO
#define SDCARD_MOSI 17    // Share SPI MOSI

// ============================================================================
// 📌 RGB LED (WS2812B) - Opsional
// ============================================================================
#define HAS_RGB_LED 1
#define RGB_LED 45        // ✅ Data pin untuk NeoPixel
#define LED_TYPE WS2812B
#define LED_ORDER GRB
#define LED_TYPE_IS_RGBW 0
#define LED_COUNT 16
#define LED_COLOR_STEP 15

// ============================================================================
// 📌 POWER MANAGEMENT (BQ25896 PMU via I2C)
// ============================================================================
#define XPOWERS_CHIP_BQ25896
#define USE_BOOST   // ✅ Enable 5V boost output

// ============================================================================
// 📌 MICROPHONE (INMP441 I2S)
// ============================================================================
#define MIC_INMP441
#define PIN_CLK  1   // I2S SCK
#define PIN_DATA 10  // I2S SD
#define PIN_WS   2   // I2S WS

// ============================================================================
// 📌 IO EXPANDER (AW9523) - Opsional
// ============================================================================
#define USE_IO_EXPANDER
#define IO_EXPANDER_AW9523
#define IO_EXP_GPS  13
#define IO_EXP_MIC  4
#define IO_EXP_VIBRO 2
#define IO_EXP_CC_RX 7
#define IO_EXP_CC_TX 12

#endif /* Pins_Arduino_h */
