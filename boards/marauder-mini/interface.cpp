#include "core/powerSave.h"
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Description: initial setup for the device
***************************************************************************************/
void _setup_gpio() {

    // Tombol aktif LOW
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);
    pinMode(R_BTN, INPUT_PULLUP);
    pinMode(L_BTN, INPUT_PULLUP);

    bruceConfig.colorInverted = 0;
    bruceConfigPins.rotation = 0;
}

/***************************************************************************************
** Function name: _post_setup_gpio()
***************************************************************************************/
void _post_setup_gpio() {

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
}

/***************************************************************************************
** Function name: getBattery()
***************************************************************************************/
int getBattery() {
    return 0;
}

/*********************************************************************
** Function: setBrightness
**********************************************************************/
void _setBrightness(uint8_t brightval) {

    pinMode(TFT_BL, OUTPUT);

    // simple ON/OFF
    if (brightval > 5) {
        digitalWrite(TFT_BL, HIGH);
    } else {
        digitalWrite(TFT_BL, LOW);
    }
}

/*********************************************************************
** Function: InputHandler
**********************************************************************/
void InputHandler(void) {

    static unsigned long lastRead = 0;

    // debounce 60ms
    if (millis() - lastRead < 60) return;

    lastRead = millis();

    // reset semua flag dulu
    PrevPress = false;
    NextPress = false;
    SelPress = false;
    UpPress = false;
    DownPress = false;
    EscPress = false;
    AnyKeyPress = false;

    // baca tombol
    bool u = (digitalRead(UP_BTN)  == LOW);
    bool d = (digitalRead(DW_BTN)  == LOW);
    bool r = (digitalRead(R_BTN)   == LOW);
    bool l = (digitalRead(L_BTN)   == LOW);
    bool s = (digitalRead(SEL_BTN) == LOW);

    // kalau semua HIGH berarti gak ada tombol
    if (!u && !d && !r && !l && !s) {
        return;
    }

    // wakeup screen
    if (!wakeUpScreen()) {
        AnyKeyPress = true;
    } else {
        return;
    }

    // Kombinasi ESC
    if (l && s) {
        EscPress = true;
        return;
    }

    // tombol normal
    if (l) PrevPress = true;
    if (r) NextPress = true;
    if (u) UpPress = true;
    if (d) DownPress = true;
    if (s) SelPress = true;
}

/*********************************************************************
** Function: powerOff
**********************************************************************/
void powerOff() {
}

/*********************************************************************
** Function: checkReboot
**********************************************************************/
void checkReboot() {
}
