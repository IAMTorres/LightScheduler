/**
 * LightScheduler.cpp
 *
 * Created by Gonçalo Torres (https://github.com/IAMTorres)
 * https://github.com/IAMTorres/LightScheduler
 *
 * MIT License
 */

#include "LightScheduler.h"

LightScheduler::LightScheduler(int pwmPin, uint16_t dimDurationMinutes) {
    _pwmPin              = pwmPin;
    _dimDurationMinutes  = dimDurationMinutes;
    _lightOn             = false;
    _dimming             = false;
    _dimStartTime        = 0;
    _brightness          = 0;
    _onTime              = {0, 0};
    _offTime             = {0, 0};

    pinMode(_pwmPin, OUTPUT);
    analogWrite(_pwmPin, 0);
}

void LightScheduler::setOnTime(uint8_t hour, uint8_t minute) {
    _onTime.hour   = hour;
    _onTime.minute = minute;
}

void LightScheduler::setOffTime(uint8_t hour, uint8_t minute) {
    _offTime.hour   = hour;
    _offTime.minute = minute;
}

void LightScheduler::setDimDuration(uint16_t minutes) {
    _dimDurationMinutes = minutes;
}

ScheduleTime LightScheduler::getOnTime() {
    return _onTime;
}

ScheduleTime LightScheduler::getOffTime() {
    return _offTime;
}

void LightScheduler::saveToEEPROM(int address) {
    EEPROM.put(address,                  _onTime);
    EEPROM.put(address + sizeof(ScheduleTime), _offTime);
}

void LightScheduler::loadFromEEPROM(int address) {
    EEPROM.get(address,                  _onTime);
    EEPROM.get(address + sizeof(ScheduleTime), _offTime);
}

void LightScheduler::update(uint8_t currentHour, uint8_t currentMinute) {
    if (_isInOnWindow(currentHour, currentMinute)) {
        // Inside on-window: full brightness, reset dim state
        if (!_lightOn) {
            _dimming  = false;
            _lightOn  = true;
        }
        if (!_dimming) {
            _brightness = 255;
            analogWrite(_pwmPin, _brightness);
        }
    } else if (_lightOn) {
        // Outside on-window: begin or continue gradual dimming
        if (!_dimming) {
            _dimming      = true;
            _dimStartTime = millis();
        }

        unsigned long elapsed       = millis() - _dimStartTime;
        unsigned long dimDurationMs = (unsigned long)_dimDurationMinutes * 60000UL;

        if (elapsed < dimDurationMs) {
            _brightness = (uint8_t) map(elapsed, 0, dimDurationMs, 255, 0);
            analogWrite(_pwmPin, _brightness);
        } else {
            // Dimming complete — light off until next on-window
            _brightness = 0;
            analogWrite(_pwmPin, 0);
            _dimming = false;
            _lightOn = false;
        }
    }
}

bool LightScheduler::isOn() {
    return _lightOn;
}

bool LightScheduler::isDimming() {
    return _dimming;
}

uint8_t LightScheduler::getBrightness() {
    return _brightness;
}

// --- Private ---

bool LightScheduler::_isInOnWindow(uint8_t h, uint8_t m) {
    int current = h * 60 + m;
    int onMin   = _onTime.hour  * 60 + _onTime.minute;
    int offMin  = _offTime.hour * 60 + _offTime.minute;

    if (onMin == offMin) return false; // no schedule set

    if (onMin < offMin) {
        // Same-day window  e.g. 08:00 → 22:00
        return current >= onMin && current < offMin;
    } else {
        // Overnight window e.g. 20:00 → 06:00
        return current >= onMin || current < offMin;
    }
}
