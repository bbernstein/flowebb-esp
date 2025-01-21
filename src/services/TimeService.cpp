#include "TimeService.h"

void TimeService::initialize() {
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    Serial.println("\nTime configuration:");
    Serial.printf("GMT Offset: %ld seconds (%d hours)\n", GMT_OFFSET_SEC, GMT_OFFSET_SEC/3600);
    Serial.printf("DST Offset: %d seconds (%d hours)\n", DAYLIGHT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC/3600);
    Serial.printf("Current time: %s\n", formatLocalTime().c_str());
}

String TimeService::formatSecondsToTime(unsigned long totalSeconds) {
    if (totalSeconds > 31536000) {
        totalSeconds = totalSeconds % 86400;
    }
    
    unsigned long hours = totalSeconds / 3600;
    unsigned long minutes = (totalSeconds % 3600) / 60;
    
    return String(hours) + "h " + String(minutes) + "m";
}

String TimeService::formatLocalTime(time_t timestamp) {
    struct tm timeinfo;
    char timeStringBuff[50];
    char tzBuff[10];
    
    if (timestamp == 0) {
        if(!getLocalTime(&timeinfo)){
            return "Failed to obtain time";
        }
    } else {
        localtime_r(&timestamp, &timeinfo);
    }
    
    strftime(timeStringBuff, sizeof(timeStringBuff), "%a, %Y-%m-%d %H:%M:%S", &timeinfo);
    strftime(tzBuff, sizeof(tzBuff), "%z", &timeinfo);
    return String(timeStringBuff) + " " + String(tzBuff);
}

void TimeService::printLocalTime(time_t timestamp) {
    Serial.println(formatLocalTime(timestamp));
}
