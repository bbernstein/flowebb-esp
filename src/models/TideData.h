#pragma once
#include <Arduino.h>

// Reduce maximum number of extremes to a more reasonable size
const int MAX_EXTREMES = 20;  // We really only need the next few extremes

struct TideExtreme {
    time_t timestamp;
    float height;
    bool isHigh;
};

struct TideData {
    String type;           // RISING or FALLING
    float currentHeight;   // Current water level
    TideExtreme current;   // Most recent past extreme
    TideExtreme extremes[MAX_EXTREMES];  // Store up to MAX_EXTREMES extremes
    int numExtremes;           // Number of stored extremes
    unsigned long lastUpdateTime; // When the data was last fetched

    TideData();
    bool hasValidFutureExtremes(time_t currentTime) const;
    bool needsUpdate(time_t currentTime) const;
    time_t getNextUpdateTime() const;
    
private:
    static const unsigned long UPDATE_INTERVAL = 6 * 3600; // 6 hours in seconds
};
