#include "core/powerSave.h"
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // Aktifkan internal pull-up (untuk pin yang support)
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);
    pinMode(R_BTN, INPUT_PULLUP);
    pinMode(L_BTN, INPUT_PULLUP);

    // Pastikan pin HIGH saat idle (released)
    digitalWrite(UP_BTN, HIGH);
    digitalWrite(SEL_BTN, HIGH);
    digitalWrite(DW_BTN, HIGH);
    digitalWrite(R_BTN, HIGH);
    digitalWrite(L_BTN, HIGH);

    bruceConfig.colorInverted = 0;
    bruceConfigPins.rotation = 0; // portrait mode for Phantom
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() { 
    pinMode(TFT_BL, OUTPUT); 
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { 
    return 0; 
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    pinMode(TFT_BL, OUTPUT);
    if (brightval > 5) {
        digitalWrite(TFT_BL, LOW);
        digitalWrite(TFT_BL, HIGH);
    } else {
        digitalWrite(TFT_BL, HIGH);
        digitalWrite(TFT_BL, LOW);
    }
}

/*********************************************************************
** Function: readButtonStable
** Helper: Baca tombol dengan filter noise (anti ghost press)
**********************************************************************/
bool readButtonStable(int pin) {
    // Baca pin 5x dengan delay 5ms antar baca
    int lowCount = 0;
    for (int i = 0; i < 5; i++) {
        if (digitalRead(pin) == LOW) {
            lowCount++;
        }
        delay(5);
    }
    // Terima sebagai "tekan" hanya jika minimal 4x dari 5x baca = LOW
    return (lowCount >= 4);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = millis();
    static unsigned long esc_tm = millis();
    static bool esc_armed = false;
    static bool lastState[5] = {HIGH, HIGH, HIGH, HIGH, HIGH}; // Track state terakhir
    
    // Cek interval minimal 150ms (responsif tapi stabil)
    if (!(millis() - tm > 150 || LongPress)) {
        return;
    }

    // Baca tombol dengan filter noise
    bool u = readButtonStable(UP_BTN);
    bool d = readButtonStable(DW_BTN);
    bool r = readButtonStable(R_BTN);
    bool l = readButtonStable(L_BTN);
    bool s = readButtonStable(SEL_BTN);

    // Debounce: abaikan jika state berubah mendadak (false trigger)
    bool currentStates[5] = {u, d, r, l, s};
    bool stateChanged = false;
    for (int i = 0; i < 5; i++) {
        if (currentStates[i] != lastState[i]) {
            stateChanged = true;
            lastState[i] = currentStates[i];
        }
    }
    if (stateChanged) {
        delay(30); // Tunggu 30ms buat verifikasi
        // Re-read untuk konfirmasi
        u = readButtonStable(UP_BTN);
        d = readButtonStable(DW_BTN);
        r = readButtonStable(R_BTN);
        l = readButtonStable(L_BTN);
        s = readButtonStable(SEL_BTN);
    }

    // Deteksi tombol ditekan (active LOW)
    bool anyPressed = (!s || !u || !d || !r || !l);
    
    if (anyPressed) {
        tm = millis();
        if (!wakeUpScreen()) {
            AnyKeyPress = true;
        } else {
            return;
        }
    }
    
    // Handle Esc (L + SEL)
    if (!l && !s) {
        EscPress = true;
        return;
    }
    
    // Handle Prev (L saja)
    if (!l) {
        PrevPress = true;
        if (esc_armed == false) {
            esc_tm = millis();
            esc_armed = true;
        }
    }
    
    // Handle long-press L jadi Esc
    if (esc_armed && millis() - esc_tm > 1000) {
        esc_armed = false;
        esc_tm = millis();
        PrevPress = false;
        EscPress = true;
    }
    
    // Handle tombol lain
    if (!r) NextPress = true;
    if (!u) UpPress = true;
    if (!d) DownPress = true;
    if (!s) SelPress = true;
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
