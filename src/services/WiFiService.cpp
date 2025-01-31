/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 */

#include "WiFiService.h"
#include "../config/config.h"

bool WiFiService::_isConnected = false;

bool WiFiService::connect() {
    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("Connecting to %s ", WIFI_SSID);
    }
    
    // Set WiFi to low power mode
    WiFi.setSleep(true);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    // Use timeout instead of infinite wait
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startAttemptTime > WIFI_TIMEOUT) {
            if (ENABLE_DEBUG_PRINTS) {
                Serial.println("\nWiFi connection timeout");
            }
            WiFi.disconnect(true);
            _isConnected = false;
            return false;
        }
        delay(100);
        if (ENABLE_DEBUG_PRINTS) {
            Serial.print(".");
        }
    }
    
    if (ENABLE_DEBUG_PRINTS) {
        Serial.println(" CONNECTED");
        printWiFiStatus();
    }
    
    _isConnected = true;
    return true;
}

void WiFiService::disconnect() {
    WiFi.disconnect(true);
    _isConnected = false;
    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("WiFi disconnected");
    }
}

bool WiFiService::checkConnection() {
    if (!_isConnected || WiFi.status() != WL_CONNECTED) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("WiFi connection lost. Reconnecting...");
        }
        return connect();
    }
    return true;
}

void WiFiService::printWiFiStatus() {
    Serial.println("\nWiFi Status:");
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Signal Strength (RSSI): %d dBm\n", WiFi.RSSI());
}
