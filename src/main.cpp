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

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);
    Serial.println("ESP32-S3 Tide Tracker Starting...");
    
    // Initialize components with delays between
    PreferencesManager::initialize();
    delay(100);
    
    WiFiService::connect();
    delay(1000);  // Give WiFi connection time to stabilize
    
    TimeService::initialize();
    delay(1000);  // Give NTP time to sync
    
    LedController::initialize();
    delay(100);
    
    // Load or fetch initial tide data
    tryInitialDataLoad();
}

void loop() {
    // Add try-catch to prevent crashes in main loop
    try {
        unsigned long currentMillis = millis();
        time_t now = TimeService::getCurrentTime();
        
        // Check WiFi connection
        WiFiService::checkConnection();
        
        // Update tide data if needed
        if (tideData.needsUpdate(now) && 
            (currentMillis - lastTideCheck >= TIDE_CHECK_INTERVAL)) {
            
            Serial.println("Updating tide data...");
            if (TideService::fetchTideData(tideData)) {
                Serial.println("Tide data updated successfully");
                if (PreferencesManager::saveTideData(tideData)) {
                    Serial.println("Updated data saved successfully");
                }
                retryCount = 0;
            } else {
                Serial.println("Failed to update tide data");
                if (++retryCount >= 3) {
                    Serial.println("Too many failures, rebooting...");
                    delay(1000);
                    ESP.restart();
                }
            }
            lastTideCheck = currentMillis;
        }
        
        // Update LED display
        LedController::updateDisplay(tideData);
        
    } catch (...) {
        Serial.println("Caught exception in main loop");
    }
    
    delay(10); // Basic rate limiting
}
