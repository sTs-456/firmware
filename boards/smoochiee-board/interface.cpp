#include "core/powerSave.h"
#include "pins_arduino.h"
#include <Wire.h>
#include <SPI.h>

#define IS_VALID_PIN(p) ((p) != 255 && (p) != -1 && (p) >= 0)

#ifdef XPOWERS_CHIP_BQ25896
#include <XPowersLib.h>
XPowersPPM PPM;
#endif

// Global SPI instance untuk modul (HSPI/SPI3)
SPIClass moduleSPI(SPI3_HOST);

void _setup_gpio() {
    // 1️⃣ BUTTONS
    if (IS_VALID_PIN(SEL_BTN)) pinMode(SEL_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(UP_BTN))  pinMode(UP_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(DW_BTN))  pinMode(DW_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(L_BTN))   pinMode(L_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(R_BTN))   pinMode(R_BTN, INPUT_PULLUP);

    // 2️⃣ TFT BACKLIGHT (FIX CRASH "IO 5 is not set as GPIO")
    if (IS_VALID_PIN(TFT_BL)) {
        pinMode(TFT_BL, OUTPUT);
        analogWrite(TFT_BL, MINBRIGHT);
    }

    // 3️⃣ DUAL SPI INIT
    // 🔵 VSPI (SPI2) -> DEDICATED UNTUK TFT (NO CS)
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, -1);
    SPI.setFrequency(SPI_FREQUENCY);
    Serial.println("[SPI] VSPI initialized for ST7789 (No CS, Dedicated)");

    // 🟠 HSPI (SPI3) -> SHARED UNTUK CC1101/NRF24/SD
    moduleSPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, -1);
    moduleSPI.setFrequency(20000000);
    Serial.println("[SPI] HSPI initialized for RF/SD modules");

    // 4️⃣ MODULE CS PINS (Initialize HIGH)
    if (IS_VALID_PIN(CC1101_SS_PIN)) { pinMode(CC1101_SS_PIN, OUTPUT); digitalWrite(CC1101_SS_PIN, HIGH); }
    if (IS_VALID_PIN(NRF24_SS_PIN))  { pinMode(NRF24_SS_PIN, OUTPUT);  digitalWrite(NRF24_SS_PIN, HIGH);  }
    if (IS_VALID_PIN(SDCARD_CS))     { pinMode(SDCARD_CS, OUTPUT);     digitalWrite(SDCARD_CS, HIGH);     }

    bruceConfigPins.rfModule = CC1101_SPI_MODULE;

    // 5️⃣ IR RX LED
    if (IS_VALID_PIN(RXLED)) {
        bruceConfigPins.irRx = RXLED;
        pinMode(RXLED, OUTPUT);
        digitalWrite(RXLED, LOW);
    }

    // 6️⃣ I2C INIT (Safe pins 19/20)
    if (IS_VALID_PIN(GROVE_SDA) && IS_VALID_PIN(GROVE_SCL)) {
        Wire.setPins(GROVE_SDA, GROVE_SCL);
        Wire.begin(GROVE_SDA, GROVE_SCL);
        Wire.setClock(100000); // 100kHz lebih stabil untuk PMU

        #ifdef XPOWERS_CHIP_BQ25896
        bool pmu_ret = PPM.init(Wire, GROVE_SDA, GROVE_SCL, BQ25896_SLAVE_ADDRESS);
        if (pmu_ret) {
            PPM.setSysPowerDownVoltage(3300);
            PPM.setInputCurrentLimit(3250);
            PPM.setChargeTargetVoltage(4208);
            PPM.setChargerConstantCurr(832);
            PPM.enableMeasure(PowersBQ25896::CONTINUOUS);
            PPM.disableOTG();
            PPM.enableCharge();
            Serial.println("[PMU] BQ25896 initialized OK");
        } else {
            Serial.println("[PMU] BQ25896 FAILED - cek I2C wiring & pull-up resistor!");
        }
        #endif
    }
}

bool isCharging() {
    #ifdef XPOWERS_CHIP_BQ25896
    return PPM.isCharging();
    #else
    return false;
    #endif
}

int getBattery() {
    #ifdef XPOWERS_CHIP_BQ25896
    int voltage = PPM.getBattVoltage();
    int percent = (voltage - 3300) * 100 / (float)(4150 - 3350);
    if (percent < 0) return 1;
    if (percent > 100) percent = 100;
    if (PPM.isCharging() && percent >= 97) {
        PPM.disableBatLoad();
        percent = 95;
    }
    if (PPM.isChargeDone()) percent = 100;
    return percent;
    #else
    return 100;
    #endif
}

void _setBrightness(uint8_t brightval) {
    if (!IS_VALID_PIN(TFT_BL)) return;
    if (brightval == 0) analogWrite(TFT_BL, 0);
    else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

void InputHandler(void) {
    static unsigned long tm = 0;
    if (millis() - tm < 200 && !LongPress) return;
    bool _u = digitalRead(UP_BTN);
    bool _d = digitalRead(DW_BTN);
    bool _l = digitalRead(L_BTN);
    bool _r = digitalRead(R_BTN);
    bool _s = digitalRead(SEL_BTN);

    if (!_s || !_u || !_d || !_r || !_l) {
        tm = millis();
        if (!wakeUpScreen()) AnyKeyPress = true;
        else return;
    }
    if (!_l) PrevPress = true;
    if (!_r) NextPress = true;
    if (!_u) { UpPress = true; PrevPagePress = true; }
    if (!_d) { DownPress = true; NextPagePress = true; }
    if (!_s) SelPress = true;
    if (!_l && !_r) { EscPress = true; NextPress = false; PrevPress = false; }
}

void powerOff() {
    if (IS_VALID_PIN(SEL_BTN)) esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN, BTN_ACT);
    esp_deep_sleep_start();
}

void checkReboot() {
    if (!IS_VALID_PIN(L_BTN) || !IS_VALID_PIN(R_BTN)) return;
    if (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
        uint32_t time_count = millis();
        int countDown = 0;
        while (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
            if (millis() - time_count > 500) {
                if (countDown == 0) {
                    int textWidth = tft.textWidth("PWR OFF IN 3/3", 1);
                    tft.fillRect(tftWidth / 2 - textWidth / 2, 7, textWidth, 18, bruceConfig.bgColor);
                }
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if (countDown < 4)
                    tft.drawCentreString("PWR OFF IN " + String(countDown) + "/3", tftWidth / 2, 12, 1);
                else {
                    tft.fillScreen(bruceConfig.bgColor);
                    while (digitalRead(L_BTN) == BTN_ACT || digitalRead(R_BTN) == BTN_ACT);
                    delay(200);
                    powerOff();
                }
                delay(10);
            }
        }
        delay(30);
        if (millis() - time_count > 500) {
            tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
            drawStatusBar();
        }
    }
}
