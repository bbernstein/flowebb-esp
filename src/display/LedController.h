#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../models/TideData.h"
#include "../config/config.h"
#include "../services/TimeService.h"

class LedController {
public:
    static void initialize();
    static void updateDisplay(const TideData& tideData);

private:
    static Adafruit_NeoPixel pixel;
    static uint8_t currentBlueLevel;
    static unsigned long nextWaveTime;

    static float calculateTideProgress(const TideExtreme& current, const TideExtreme& next, time_t now);
    static void updateWaveAnimation(unsigned long currentMillis);
    static uint32_t calculateTideColor(float progress, const TideExtreme& current, const TideExtreme& next);
    static void debugPrintStatus(float progress, uint32_t color, const TideExtreme& nextExtreme);

    static unsigned long lastPrintTime;
};
