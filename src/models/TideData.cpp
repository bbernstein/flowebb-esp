/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 */

#include "TideData.h"

TideData::TideData() : 
    type(""),
    currentHeight(0),
    numExtremes(0),
    lastUpdateTime(0) {
}

bool TideData::hasValidFutureExtremes(time_t currentTime) const {
    return numExtremes > 0 && currentTime < extremes[numExtremes - 1].timestamp;
}

bool TideData::needsUpdate(time_t currentTime) const {
    return numExtremes == 0 ||
           currentTime > extremes[numExtremes - 1].timestamp ||
           (currentTime - lastUpdateTime) > UPDATE_INTERVAL;
}

time_t TideData::getNextUpdateTime() const {
    time_t nextUpdate;
    
    if (numExtremes == 0) {
        // If no data, update immediately
        return time(nullptr);
    }
    
    // Get time of next update based on last update time
    time_t updateBasedOnInterval = lastUpdateTime + UPDATE_INTERVAL;
    
    // Get time of next update based on last extreme
    time_t updateBasedOnExtremes = extremes[numExtremes - 1].timestamp;
    
    // Use the earlier of the two times
    nextUpdate = min(updateBasedOnInterval, updateBasedOnExtremes);
    
    // If next update time is in the past, return current time
    time_t currentTime = time(nullptr);
    if (nextUpdate <= currentTime) {
        return currentTime;
    }
    
    return nextUpdate;
}
