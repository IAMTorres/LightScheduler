/**
 * BasicScheduler.ino
 *
 * Minimal example using LightScheduler with a software clock.
 * In a real project replace the manual time tracking with an RTC module.
 *
 * Hardware:
 *   - LED (or LED driver) on pin 9 (PWM)
 */

#include <LightScheduler.h>

LightScheduler scheduler(9);  // PWM pin 9

// Simulated time — replace with your RTC reads
uint8_t currentHour   = 7;
uint8_t currentMinute = 55;
unsigned long lastTick = 0;

void setup() {
    Serial.begin(9600);

    scheduler.setOnTime(8, 0);    // turn on  at 08:00
    scheduler.setOffTime(22, 0);  // dim start at 22:00
    scheduler.setDimDuration(60); // 60-minute gradual sunset

    Serial.println("LightScheduler ready.");
    Serial.print("ON  time: 08:00");
    Serial.print("OFF time: 22:00 (60-min dim)");
}

void loop() {
    // Advance simulated clock every real second
    if (millis() - lastTick >= 1000) {
        lastTick = millis();
        currentMinute++;
        if (currentMinute >= 60) {
            currentMinute = 0;
            currentHour++;
            if (currentHour >= 24) currentHour = 0;
        }
    }

    scheduler.update(currentHour, currentMinute);

    // Print status every minute change
    static uint8_t lastMinute = 255;
    if (currentMinute != lastMinute) {
        lastMinute = currentMinute;
        Serial.print("Time: ");
        if (currentHour < 10) Serial.print("0");
        Serial.print(currentHour);
        Serial.print(":");
        if (currentMinute < 10) Serial.print("0");
        Serial.print(currentMinute);
        Serial.print("  Light: ");
        Serial.print(scheduler.isOn() ? "ON" : "OFF");
        if (scheduler.isDimming()) {
            Serial.print("  Dimming: ");
            Serial.print(scheduler.getBrightness());
            Serial.print("/255");
        }
        Serial.println();
    }
}
