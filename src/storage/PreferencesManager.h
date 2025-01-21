#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "../models/TideData.h"
#include "../utils/JsonHelper.h"
#include "../config/config.h"

class PreferencesManager {
public:
    static void initialize();
    static bool saveTideData(const TideData& tideData);
    static bool loadTideData(TideData& tideData);
    
private:
    static Preferences preferences;
};
