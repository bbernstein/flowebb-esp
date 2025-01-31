#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "esp32-hal.h"  // For ESP32 specific functions
#include "../models/TideData.h"
#include "../config/config.h"
#include "../config/wifi_credentials.h"
#include "TimeService.h"
#include "WiFiService.h"

class TideService {
public:
    static bool fetchTideData(TideData& tideData);
    
private:
    static String buildGraphQLQuery(time_t startTime, time_t endTime);
    static void processTideExtremes(JSONVar& extremes, TideData& tideData, time_t now);
    static void feedWatchdog();
};
