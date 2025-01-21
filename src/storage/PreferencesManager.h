#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <Arduino_JSON.h>
#include "../models/TideData.h"
#include "../config/config.h"

class PreferencesManager {
public:
    static void initialize();
    static bool saveTideData(const TideData& tideData);
    static bool loadTideData(TideData& tideData);
    
private:
    static Preferences preferences;
    static JSONVar serializeTideData(const TideData& tideData);
    static bool deserializeTideData(const JSONVar& json, TideData& tideData);
    static JSONVar serializeTideExtreme(const TideExtreme& extreme);
    static bool deserializeTideExtreme(const JSONVar& json, TideExtreme& extreme);
};
