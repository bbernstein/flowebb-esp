#pragma once
#include <Arduino.h>
#include <Arduino_JSON.h>
#include "../models/TideData.h"
#include "../config/config.h"

class JsonHelper {
public:
    static String serializeTideData(const TideData& tideData);
    static bool deserializeTideData(const String& jsonString, TideData& tideData);
    
private:
    static JSONVar serializeExtreme(const TideExtreme& extreme);
    static void deserializeExtreme(const JSONVar& jsonExtreme, TideExtreme& extreme);
    static void adjustTimestampForTimezone(time_t& timestamp, bool toUTC);
};
