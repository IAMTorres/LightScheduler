#ifndef LightScheduler_h
#define LightScheduler_h

#include <Arduino.h>
#include <EEPROM.h>

/**
 * LightScheduler - Arduino library for scheduled LED dimming
 *
 * Schedules a PWM-controlled light to turn on and off at configurable times,
 * with a gradual sunset dimming effect. Settings persist across power cycles
 * via EEPROM. Works with any RTC module.
 *
 * Usage:
 *   LightScheduler scheduler(9);          // PWM pin
 *   scheduler.setOnTime(8, 0);            // turn on at 08:00
 *   scheduler.setOffTime(22, 0);          // start dimming at 22:00
 *   scheduler.setDimDuration(60);         // 60-minute gradual fade
 *   scheduler.loadFromEEPROM();           // restore saved schedule
 *
 *   void loop() {
 *     scheduler.update(currentHour, currentMinute);
 *   }
 */

struct ScheduleTime {
    uint8_t hour;
    uint8_t minute;
};

class LightScheduler {
public:
    /**
     * @param pwmPin         Arduino pin connected to PWM input of driver (must support analogWrite)
     * @param dimDuration    Duration of the gradual sunset dimming in minutes (default: 60)
     */
    LightScheduler(int pwmPin, uint16_t dimDurationMinutes = 60);

    /** Set the time when the light should turn on at full brightness */
    void setOnTime(uint8_t hour, uint8_t minute);

    /** Set the time when the gradual dimming begins */
    void setOffTime(uint8_t hour, uint8_t minute);

    /** Set how long the sunset dimming takes in minutes */
    void setDimDuration(uint16_t minutes);

    /** Get currently configured on time */
    ScheduleTime getOnTime();

    /** Get currently configured off time */
    ScheduleTime getOffTime();

    /**
     * Save current on/off schedule to EEPROM.
     * @param address  EEPROM start address (default: 0). Uses 4 bytes.
     */
    void saveToEEPROM(int address = 0);

    /**
     * Load on/off schedule from EEPROM.
     * @param address  EEPROM start address (default: 0).
     */
    void loadFromEEPROM(int address = 0);

    /**
     * Call this every loop iteration with the current time from your RTC.
     * Handles turn-on, dimming and turn-off automatically.
     *
     * @param currentHour    Current hour   (0-23)
     * @param currentMinute  Current minute (0-59)
     */
    void update(uint8_t currentHour, uint8_t currentMinute);

    /** Returns true if the light is currently on (including during dimming) */
    bool isOn();

    /** Returns true if the gradual dimming is currently in progress */
    bool isDimming();

    /** Returns current PWM brightness level (0-255) */
    uint8_t getBrightness();

private:
    int          _pwmPin;
    ScheduleTime _onTime;
    ScheduleTime _offTime;
    uint16_t     _dimDurationMinutes;

    bool          _lightOn;
    bool          _dimming;
    unsigned long _dimStartTime;
    uint8_t       _brightness;

    bool _isInOnWindow(uint8_t h, uint8_t m);
};

#endif
