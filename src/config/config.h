#pragma once
#include <cstdint>

// NTP Server settings
const char* const NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = -18000;  // Eastern Time UTC-5 (in seconds)
const int DAYLIGHT_OFFSET_SEC = 3600; // 1 hour DST offset

// NeoPixel LED configuration
const int LED_PIN = 48;     // WS2812 LED is on GPIO48
const int NUM_LEDS = 1;     // Only one LED
const int BRIGHTNESS = 20;  // Brightness level (0-255)
const int RED_BRIGHTNESS = 20;
const int GREEN_BRIGHTNESS = 20;
const int BLUE_BRIGHTNESS = 20;
const int TRANSITION_SPEED = 10;  // Transition speed in ms

// User location
const char* const LATITUDE = "41.6540367";
const char* const LONGITUDE = "-70.1630046";
const char* const TIDE_STATION_ID = "8447504"; // SOUTH YARMOUTH, BASS RIVER (VI)

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
