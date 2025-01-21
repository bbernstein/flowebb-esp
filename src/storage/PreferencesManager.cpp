/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 */

#include "PreferencesManager.h"

Preferences PreferencesManager::preferences;

void PreferencesManager::initialize() {
    Serial.println("Initializing preferences...");
    if (!preferences.begin(PREF_NAMESPACE, false)) {
        Serial.println("Failed to initialize preferences");
        return;
    }
    Serial.println("Preferences initialized successfully");
}

bool PreferencesManager::saveTideData(const TideData& tideData) {
    Serial.println("Attempting to save tide data...");
    
    // Clear existing data
    preferences.clear();
    
    // Use JsonHelper to serialize
    String jsonString = JsonHelper::serializeTideData(tideData);
    Serial.printf("JSON string length: %d\n", jsonString.length());
    
    if (preferences.putString(TIDE_DATA_KEY, jsonString)) {
        Serial.println("Tide data saved to NVS successfully");
        return true;
    }
    
    Serial.println("Failed to save tide data to NVS");
    return false;
}

bool PreferencesManager::loadTideData(TideData& tideData) {
    Serial.println("Attempting to load tide data...");
    
    String jsonString = preferences.getString(TIDE_DATA_KEY, "");
    if (jsonString.length() == 0) {
        Serial.println("No saved tide data found in NVS");
        return false;
    }
    
    Serial.printf("Loaded JSON string length: %d\n", jsonString.length());
    
    if (JsonHelper::deserializeTideData(jsonString, tideData)) {
        Serial.println("Tide data loaded successfully");
        return true;
    }
    
    Serial.println("Failed to parse saved tide data");
    return false;
}
