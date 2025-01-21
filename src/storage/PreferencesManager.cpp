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
    
    // First clear any existing data
    preferences.clear();
    
    JSONVar tideJson;
    
    // Basic data
    tideJson["type"] = tideData.type;
    tideJson["currentHeight"] = tideData.currentHeight;
    tideJson["lastUpdateTime"] = (double)tideData.lastUpdateTime;
    
    // Current extreme
    JSONVar currentExtreme;
    currentExtreme["timestamp"] = (double)tideData.current.timestamp;
    currentExtreme["height"] = tideData.current.height;
    currentExtreme["isHigh"] = tideData.current.isHigh;
    tideJson["current"] = currentExtreme;
    
    // Future extremes
    JSONVar extremesArray;
    for(int i = 0; i < tideData.numExtremes && i < MAX_EXTREMES; i++) {
        JSONVar extreme;
        extreme["timestamp"] = (double)tideData.extremes[i].timestamp;
        extreme["height"] = tideData.extremes[i].height;
        extreme["isHigh"] = tideData.extremes[i].isHigh;
        extremesArray[i] = extreme;
    }
    tideJson["extremes"] = extremesArray;
    tideJson["numExtremes"] = tideData.numExtremes;
    
    String jsonString = JSON.stringify(tideJson);
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
    
    JSONVar tideJson = JSON.parse(jsonString);
    if (JSON.typeof(tideJson) == "undefined") {
        Serial.println("Failed to parse saved tide data JSON");
        return false;
    }
    
    try {
        // Load basic data with type checking
        if (tideJson.hasOwnProperty("type")) {
            String typeStr = JSON.stringify(tideJson["type"]);
            // Remove quotes that stringify adds
            tideData.type = typeStr.substring(1, typeStr.length() - 1);
        } else {
            tideData.type = "UNKNOWN";
        }
        
        if (tideJson.hasOwnProperty("currentHeight")) {
            tideData.currentHeight = (double)tideJson["currentHeight"];
        } else {
            tideData.currentHeight = 0.0;
        }
        
        if (tideJson.hasOwnProperty("lastUpdateTime")) {
            tideData.lastUpdateTime = (unsigned long)((double)tideJson["lastUpdateTime"]);
        } else {
            tideData.lastUpdateTime = 0;
        }
        
        // Load current extreme
        if (tideJson.hasOwnProperty("current")) {
            JSONVar current = tideJson["current"];
            tideData.current.timestamp = (time_t)((double)current["timestamp"]);
            tideData.current.height = (double)current["height"];
            tideData.current.isHigh = (bool)current["isHigh"];
        }
        
        // Load future extremes
        tideData.numExtremes = 0;
        if (tideJson.hasOwnProperty("extremes")) {
            JSONVar extremes = tideJson["extremes"];
            int numExtremes = tideJson.hasOwnProperty("numExtremes") ? 
                            (int)tideJson["numExtremes"] : 0;
                            
            for(int i = 0; i < numExtremes && i < MAX_EXTREMES; i++) {
                JSONVar extreme = extremes[i];
                tideData.extremes[i].timestamp = (time_t)((double)extreme["timestamp"]);
                tideData.extremes[i].height = (double)extreme["height"];
                tideData.extremes[i].isHigh = (bool)extreme["isHigh"];
                tideData.numExtremes++;
            }
        }
        
        Serial.println("Tide data loaded successfully");
        return true;
        
    } catch (...) {
        Serial.println("Exception while parsing tide data");
        return false;
    }
}
