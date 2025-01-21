#pragma once
#include <Arduino.h>
#include "time.h"
#include "../config/config.h"

class TimeService {
public:
    static void initialize();
    static String formatSecondsToTime(unsigned long totalSeconds);
    static String formatLocalTime(time_t timestamp = 0);
    static void printLocalTime(time_t timestamp = 0);
    
    static time_t getCurrentTime() {
        return time(nullptr);
    }
};
