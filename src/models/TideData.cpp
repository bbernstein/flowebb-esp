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
    const unsigned long SIX_HOURS = 6 * 3600; // 6 hours in seconds
    return numExtremes == 0 ||
           currentTime > extremes[numExtremes - 1].timestamp ||
           (currentTime - lastUpdateTime) > SIX_HOURS;
}
