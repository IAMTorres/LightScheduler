/**
 * WithDS1302.ino
 *
 * LightScheduler with a DS1302 RTC module and EEPROM persistence.
 * This is the setup used in the arduino-bird-light project.
 *
 * Hardware:
 *   - DS1302 RTC: RST→D5, DAT→D6, CLK→D7
 *   - L298N motor driver ENA→D9 (PWM), IN1→D2, IN2→D3
 *   - LED light on L298N output with external 12V supply
 *   - Button 1 on D11, Button 2 on D12 (for on-device config)
 *
 * Libraries required:
 *   - DS1302 by Makuna  (install via Library Manager)
 *   - LightScheduler   (this library)
 */

#include <DS1302.h>
#include <LightScheduler.h>

// --- Pin definitions ---
const int RST_PIN  = 5;
const int DAT_PIN  = 6;
const int CLK_PIN  = 7;
const int PWM_PIN  = 9;
const int IN1_PIN  = 2;
const int IN2_PIN  = 3;
const int BTN1_PIN = 11;
const int BTN2_PIN = 12;

// --- EEPROM address for schedule ---
const int EEPROM_ADDR = 0;

// --- Objects ---
DS1302          rtc(RST_PIN, DAT_PIN, CLK_PIN);
LightScheduler  scheduler(PWM_PIN, 60);  // 60-minute dim

void setup() {
    Serial.begin(9600);

    // Motor driver direction — always forward
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);

    // Buttons with pull-up
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);

    // RTC init
    rtc.halt(false);
    rtc.writeProtect(false);

    // Load saved schedule from EEPROM
    scheduler.loadFromEEPROM(EEPROM_ADDR);

    ScheduleTime on  = scheduler.getOnTime();
    ScheduleTime off = scheduler.getOffTime();
    Serial.print("Schedule loaded — ON: ");
    Serial.print(on.hour);  Serial.print(":"); Serial.print(on.minute);
    Serial.print("  OFF: ");
    Serial.print(off.hour); Serial.print(":"); Serial.println(off.minute);
}

void loop() {
    Time t = rtc.time();

    // Update light based on current RTC time
    scheduler.update(t.hr, t.min);

    // Button 1: increase ON time by 1 minute and save
    if (digitalRead(BTN1_PIN) == LOW) {
        delay(200); // debounce
        ScheduleTime on = scheduler.getOnTime();
        on.minute++;
        if (on.minute >= 60) { on.minute = 0; on.hour = (on.hour + 1) % 24; }
        scheduler.setOnTime(on.hour, on.minute);
        scheduler.saveToEEPROM(EEPROM_ADDR);
        Serial.print("ON time set to ");
        Serial.print(on.hour); Serial.print(":"); Serial.println(on.minute);
    }

    // Button 2: increase OFF time by 1 minute and save
    if (digitalRead(BTN2_PIN) == LOW) {
        delay(200); // debounce
        ScheduleTime off = scheduler.getOffTime();
        off.minute++;
        if (off.minute >= 60) { off.minute = 0; off.hour = (off.hour + 1) % 24; }
        scheduler.setOffTime(off.hour, off.minute);
        scheduler.saveToEEPROM(EEPROM_ADDR);
        Serial.print("OFF time set to ");
        Serial.print(off.hour); Serial.print(":"); Serial.println(off.minute);
    }
}
