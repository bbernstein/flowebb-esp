/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 */

#include "LedController.h"

Adafruit_NeoPixel LedController::pixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t LedController::currentBlueLevel = 0;
unsigned long LedController::nextWaveTime = 0;
unsigned long LedController::lastPrintTime = 0;

void LedController::initialize() {
    pixel.begin();
    pixel.setBrightness(BRIGHTNESS);
    pixel.setPixelColor(0, 0); // Start with LED off
    pixel.show();
    Serial.println("NeoPixel LED initialized");
}

void LedController::updateDisplay(const TideData& tideData) {
    if (tideData.numExtremes == 0) return;
    
    time_t now = TimeService::getCurrentTime();
    unsigned long currentMillis = millis();
    
    // If we're past all stored extremes, return early
    if (now > tideData.extremes[tideData.numExtremes - 2].timestamp) {
        return;
    }

    TideExtreme currentExtreme = tideData.current;
    int nextIndex = 0;
    if (now > tideData.extremes[0].timestamp) {
        // find last extreme before now
        for (int i = tideData.numExtremes - 1; i >= 0; i--) {
            if (tideData.extremes[i].timestamp <= now) {
                currentExtreme = tideData.extremes[i];
                nextIndex = i + 1;
                break;
            }
        }
    }

    // set nextExtreme to the next extreme after now
   const  TideExtreme& nextExtreme = tideData.extremes[nextIndex];

    // Calculate progress and update animation
    float progress = calculateTideProgress(currentExtreme, nextExtreme, now);
    updateWaveAnimation(currentMillis);
    uint32_t color = calculateTideColor(progress, currentExtreme, nextExtreme);
    
    // Debug output (once per second)
    if (currentMillis - lastPrintTime >= 10000) {
        debugPrintStatus(progress, color, nextExtreme);
        lastPrintTime = currentMillis;
    }

    pixel.setPixelColor(0, color);
    pixel.show();
}

float LedController::calculateTideProgress(const TideExtreme& current, const TideExtreme& next, time_t now) {
    int64_t timeToNext = next.timestamp - now;
    int64_t totalTime = next.timestamp - current.timestamp;
    
    return 1.0 - ((float)timeToNext / totalTime);
}

void LedController::updateWaveAnimation(unsigned long currentMillis) {
    static unsigned long waveCycleStartTime = 0;
    static unsigned long waveCycleDuration = 0;
    const uint8_t MIN_BLUE = 0;
    const uint8_t MAX_BLUE = 32;

    if ((currentMillis >= nextWaveTime) || (waveCycleStartTime == 0)) {
        waveCycleDuration = random(MIN_WAVE_INTERVAL, MAX_WAVE_INTERVAL);
        waveCycleStartTime = currentMillis;
        nextWaveTime = currentMillis + waveCycleDuration;
    }
    
    unsigned long cyclePosition = (currentMillis - waveCycleStartTime) % waveCycleDuration;
    float cycleProgress = (float)cyclePosition / waveCycleDuration;
    float waveProgress = sin(cycleProgress * PI);
    
    currentBlueLevel = MIN_BLUE + (waveProgress * (MAX_BLUE - MIN_BLUE));
}

uint32_t LedController::calculateTideColor(float progress, const TideExtreme& current, const TideExtreme& next) {
    uint32_t prevColorValue = (uint8_t)(0xFF * progress) << (current.isHigh ? 16 : 8);
    uint32_t nextColorValue = (uint8_t)(0xFF * (1 - progress)) << (current.isHigh ? 8 : 16);
    uint32_t baseColor = prevColorValue | nextColorValue;

    uint8_t red = (baseColor >> 16) & 0xFF;
    uint8_t green = (baseColor >> 8) & 0xFF;

    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | currentBlueLevel;
}

void LedController::debugPrintStatus(float progress, uint32_t color, const TideExtreme& nextExtreme) {
    time_t now = TimeService::getCurrentTime();
    unsigned long timeToNext = nextExtreme.timestamp - now;
    
    Serial.printf("Time until %s: %s\n", 
        (nextExtreme.isHigh ? "HIGH" : "LOW"), 
        TimeService::formatSecondsToTime(timeToNext).c_str());
    
    Serial.printf("Progress: %.2f, Color: %06X\n", progress, color);
}
