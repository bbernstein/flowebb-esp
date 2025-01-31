#pragma once
#include <cstdint>

// Station configuration
const char* const TIDE_STATION_ID = "8447525";

// Time configuration
const int GMT_OFFSET_SEC = -18000;  // EST: UTC-5 = -5 * 3600 = -18000
const int DAYLIGHT_OFFSET_SEC = 3600; // 1 hour DST

// Debug configuration
const bool ENABLE_DEBUG_PRINTS = true;

// NTP Server settings
const char* const NTP_SERVER = "pool.ntp.org";

// NeoPixel LED configuration
const int LED_PIN = 48;     // WS2812 LED is on GPIO48
const int NUM_LEDS = 1;     // Only one LED
const int BRIGHTNESS = 64;   // Reduced brightness for power saving
const int TRANSITION_SPEED = 10;  // Transition speed in ms

// Power management configuration
const unsigned long DEEP_SLEEP_DURATION = 300000000; // 5 minutes in microseconds
const int WIFI_TIMEOUT = 30000;  // WiFi connection timeout in ms
const int PROG_PIN = 0;     // GPIO0 is typically used for programming mode detection
const int PROG_MODE_CHECK_DELAY = 500; // ms to wait before checking programming mode

// User location
const char* const LATITUDE = "41.6540367";
const char* const LONGITUDE = "-70.1630046";

// Preferences settings
const char* const PREF_NAMESPACE = "tidedata";
const char* const TIDE_DATA_KEY = "tidestate";

// LED colors
const uint32_t COLOR_RED = 0xFF0000;   // For falling tide
const uint32_t COLOR_GREEN = 0x00FF00;  // For rising tide

// Wave animation parameters
const unsigned long MIN_WAVE_INTERVAL = 10000;  // Minimum time between waves (ms)
const unsigned long MAX_WAVE_INTERVAL = 20000;  // Maximum time between waves (ms)

// Update intervals
const unsigned long TIDE_CHECK_INTERVAL = 900000; // Check every 15 minutes
