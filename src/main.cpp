/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 * MIT License - See LICENSE file in the project root for full license information.
 */

#include <Arduino.h>
#include "config/config.h"
#include "models/TideData.h"
#include "services/TimeService.h"
#include "services/WiFiService.h"
#include "services/TideService.h"
#include "storage/PreferencesManager.h"
#include "display/LedController.h"
#include "utils/JsonHelper.h"

// Global state
TideData tideData;
unsigned long lastTideCheck = 0;
int retryCount = 0;

void tryInitialDataLoad() {
    bool hasValidData = false;
    Serial.println("Starting initial data load...");
    
    // Add a small delay to make sure everything is ready
    delay(100);
    
    // Try to load saved tide data first
    Serial.println("Attempting to load saved data...");
    if (PreferencesManager::loadTideData(tideData)) {
        Serial.println("Successfully loaded saved data, checking validity...");
        time_t now = TimeService::getCurrentTime();
        
        // Check if saved data is still valid
        if (!tideData.needsUpdate(now)) {
            Serial.println("Using saved tide data");
            hasValidData = true;
        } else {
            Serial.println("Saved data is too old or expired, fetching new data");
        }
    } else {
        Serial.println("No valid saved data found");
    }
    
    // Fetch new data if needed
    if (!hasValidData) {
        Serial.println("Attempting to fetch new tide data...");
        if (TideService::fetchTideData(tideData)) {
            Serial.println("Initial tide data fetched successfully");
            delay(100); // Small delay before saving
            if (PreferencesManager::saveTideData(tideData)) {
                Serial.println("New data saved successfully");
            } else {
                Serial.println("Failed to save new data");
            }
            retryCount = 0;
        } else {
            Serial.println("Failed to fetch initial tide data");
            if (++retryCount >= 3) {
                Serial.println("Too many failures, rebooting...");
                delay(1000);
                ESP.restart();
            }
            delay(1000); // Wait before retry
        }
    }
}

bool inProgrammingMode() {
    pinMode(PROG_PIN, INPUT_PULLUP);
    delay(PROG_MODE_CHECK_DELAY);  // Give pin time to stabilize
    if (ENABLE_DEBUG_PRINTS) {
        Serial.print("PROG_PIN state: ");
        Serial.println(digitalRead(PROG_PIN));
    }
    return digitalRead(PROG_PIN) == LOW;
}

void setup() {
    if (ENABLE_DEBUG_PRINTS) {
        pinMode(PROG_PIN, OUTPUT);
        digitalWrite(PROG_PIN, LOW);

        Serial.begin(115200);
        delay(1000);  // Give USB CDC time to initialize
        while (!Serial) delay(100);  // Wait for Serial to be ready
        Serial.println("ESP32-S3 Tide Tracker Starting...");
    }
    
    // Check if we're in programming mode
    if (inProgrammingMode()) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("Programming mode detected, disabling deep sleep");
        }
        return;
    }
    
    // Initialize components
    PreferencesManager::initialize();
    LedController::initialize();
    
    // Check if this is a wake from deep sleep
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    bool needsDataUpdate = true;
    
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        // Try to use saved data first
        if (PreferencesManager::loadTideData(tideData)) {
            time_t now = TimeService::getCurrentTime();
            if (!tideData.needsUpdate(now)) {
                needsDataUpdate = false;
            }
        }
    }
    
    // Only connect to WiFi if we need to update data
    if (needsDataUpdate) {
        if (WiFiService::connect()) {
            TimeService::initialize();
            tryInitialDataLoad();
            WiFiService::disconnect();
        } else {
            // If WiFi fails, try to use saved data anyway
            PreferencesManager::loadTideData(tideData);
        }
    }
}

void loop() {
    try {
        time_t now = TimeService::getCurrentTime();
        
        // Update LED display
        LedController::updateDisplay(tideData);
        
        // Check if we need to update tide data
        if (tideData.needsUpdate(now)) {
            if (ENABLE_DEBUG_PRINTS) {
                Serial.println("Tide data needs update, connecting to WiFi...");
            }
            
            if (WiFiService::connect()) {
                if (TideService::fetchTideData(tideData)) {
                    if (ENABLE_DEBUG_PRINTS) {
                        Serial.println("Tide data updated successfully");
                    }
                    PreferencesManager::saveTideData(tideData);
                    retryCount = 0;
                } else {
                    if (ENABLE_DEBUG_PRINTS) {
                        Serial.println("Failed to update tide data");
                    }
                    if (++retryCount >= 3) {
                        ESP.restart();
                    }
                }
                WiFiService::disconnect();
            }
        }
        
        // Only use a small delay in the loop to allow LED updates
        delay(10); // Small delay to prevent tight loop
        
    } catch (...) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("Caught exception in main loop");
        }
    }
}
