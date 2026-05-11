/***************************************************************************************
** File: boards/smoochiee-board/interface.cpp
** Board: Smoochiee (ESP32-S3)
** Fix: Include pins_arduino.h + safety checks + remove duplicate code
***************************************************************************************/

#include "core/powerSave.h"
#include "pins_arduino.h"  // ⚠️ WAJIB: biar macro UP_BTN, GROVE_SDA, CC1101_SS_PIN, dll kebaca!
#include <Wire.h>

// Helper macro: cek pin valid (ESP32: 255/-1 = invalid)
#define IS_VALID_PIN(p) ((p) != 255 && (p) != -1 && (p) >= 0)

// Power handler for battery detection
#ifdef XPOWERS_CHIP_BQ25896
#include <XPowersLib.h>
XPowersPPM PPM;
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Description: Initial setup for Smoochiee board (ESP32-S3)
***************************************************************************************/
void _setup_gpio() {
    // --- BUTTONS (use INPUT_PULLUP untuk hindari floating pin) ---
    if (IS_VALID_PIN(SEL_BTN)) pinMode(SEL_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(UP_BTN))  pinMode(UP_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(DW_BTN))  pinMode(DW_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(L_BTN))   pinMode(L_BTN, INPUT_PULLUP);
    if (IS_VALID_PIN(R_BTN))   pinMode(R_BTN, INPUT_PULLUP);

    // --- RF MODULES SPI Chip Select ---
    if (IS_VALID_PIN(CC1101_SS_PIN)) {
        pinMode(CC1101_SS_PIN, OUTPUT);
        digitalWrite(CC1101_SS_PIN, HIGH);  // Deselect di awal
    }
    if (IS_VALID_PIN(NRF24_SS_PIN)) {
        pinMode(NRF24_SS_PIN, OUTPUT);
        digitalWrite(NRF24_SS_PIN, HIGH);   // Deselect di awal
    }

    // Set RF module type
    bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    
    // IR RX LED indicator
    if (IS_VALID_PIN(RXLED)) {
        bruceConfigPins.irRx = RXLED;
        pinMode(RXLED, OUTPUT);
        digitalWrite(RXLED, LOW);
    }

    // --- I2C / GROVE PORT INIT ---
    // ⚠️ ESP32-S3: setPins() HARUS sebelum begin()
    if (IS_VALID_PIN(GROVE_SDA) && IS_VALID_PIN(GROVE_SCL)) {
        Wire.setPins(GROVE_SDA, GROVE_SCL);  // Set pin DULU
        Wire.begin(GROVE_SDA, GROVE_SCL);    // Baru begin()
        Wire.setClock(400000);               // Fast mode 400kHz (opsional)
        
        // --- PMU BQ25896 INIT ---
        #ifdef XPOWERS_CHIP_BQ25896
        bool pmu_ret = PPM.init(Wire, GROVE_SDA, GROVE_SCL, BQ25896_SLAVE_ADDRESS);
        if (pmu_ret) {
            PPM.setSysPowerDownVoltage(3300);
            PPM.setInputCurrentLimit(3250);
            Serial.printf("[PMU] InputLimit: %d mA\n", PPM.getInputCurrentLimit());
            PPM.disableCurrentLimitPin();
            PPM.setChargeTargetVoltage(4208);
            PPM.setPrechargeCurr(64);
            PPM.setChargerConstantCurr(832);
            PPM.enableMeasure(PowersBQ25896::CONTINUOUS);
            PPM.disableOTG();
            PPM.enableCharge();
            Serial.println("[PMU] BQ25896 initialized OK");
        } else {
            Serial.println("[PMU] BQ25896 init FAILED - cek I2C wiring!");
        }
        #endif
    } else {
        Serial.println("[I2C] GROVE_SDA/SCL not configured - skip I2C init");
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
    // Linear approximation: 3.3V=0%, 4.15V=100%
    int percent = (voltage - 3300) * 100 / (float)(4150 - 3350);
    
    if (percent < 0) return 1;
    if (percent > 100) percent = 100;
    
    if (PPM.isCharging() && percent >= 97) {
        PPM.disableBatLoad();
        percent = 95;  // estimate still charging
    }
    if (PPM.isChargeDone()) { percent = 100; }
    
    return percent;
    #else
    return 100;  // Fallback kalau nggak ada PMU
    #endif
}

/*********************************************************************
** Function: _setBrightness
** Set TFT backlight brightness (PWM via analogWrite)
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (!IS_VALID_PIN(TFT_BL)) return;  // Safety check
    
    // ESP32-S3: analogWrite() pakai LEDC backend
    // Pastikan freq udah diset di tempat lain (misal: ledcAttach(TFT_BL, 5000, 8))
    
    if (brightval == 0) {
        analogWrite(TFT_BL, 0);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Button handler with simple debouncing (200ms)
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    if (millis() - tm < 200 && !LongPress) return;
    
    // Baca tombol (asumsi INPUT_PULLUP: HIGH = nggak ditekan, LOW = ditekan)
    bool _u = digitalRead(UP_BTN);
    bool _d = digitalRead(DW_BTN);
    bool _l = digitalRead(L_BTN);
    bool _r = digitalRead(R_BTN);
    bool _s = digitalRead(SEL_BTN);

    // Deteksi ada tombol ditekan
    if (!_s || !_u || !_d || !_r || !_l) {
        tm = millis();
        if (!wakeUpScreen()) AnyKeyPress = true;
        else return;
    }
    
    // Mapping tombol ke variabel global firmware
    if (!_l) { PrevPress = true; }
    if (!_r) { NextPress = true; }
    if (!_u) { UpPress = true; PrevPagePress = true; }
    if (!_d) { DownPress = true; NextPagePress = true; }
    if (!_s) { SelPress = true; }
    if (!_l && !_r) { EscPress = true; NextPress = false; PrevPress = false; }
}

/*********************************************************************
** Function: powerOff
** Enter deep sleep, wakeup via SEL_BTN (GPIO 21)
**********************************************************************/
void powerOff() {
    if (IS_VALID_PIN(SEL_BTN)) {
        esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN, BTN_ACT);
    }
    esp_deep_sleep_start();
}

/*********************************************************************
** Function: checkReboot
** Long-press L+R buttons (3 detik) untuk power off
**********************************************************************/
void checkReboot() {
    if (!IS_VALID_PIN(L_BTN) || !IS_VALID_PIN(R_BTN)) return;
    
    if (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
        uint32_t time_count = millis();
        int countDown = 0;
        
        while (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
            if (millis() - time_count > 500) {
                // Tampilkan countdown di layar
                if (countDown == 0) {
                    int textWidth = tft.textWidth("PWR OFF IN 3/3", 1);
                    tft.fillRect(tftWidth / 2 - textWidth / 2, 7, textWidth, 18, bruceConfig.bgColor);
                }
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                
                if (countDown < 4) {
                    tft.drawCentreString("PWR OFF IN " + String(countDown) + "/3", tftWidth / 2, 12, 1);
                } else {
                    // 3 detik tercapai → power off
                    tft.fillScreen(bruceConfig.bgColor);
                    while (digitalRead(L_BTN) == BTN_ACT || digitalRead(R_BTN) == BTN_ACT);
                    delay(200);
                    powerOff();
                }
                delay(10);
            }
        }
        
        // Clear teks kalau tombol dilepas sebelum 3 detik
        delay(30);
        if (millis() - time_count > 500) {
            tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
            drawStatusBar();
        }
    }
}
