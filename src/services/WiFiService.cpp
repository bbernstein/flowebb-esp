/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 */

#include "WiFiService.h"

void WiFiService::connect() {
    Serial.printf("Connecting to %s ", WIFI_SSID);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println(" CONNECTED");
    printWiFiStatus();
}

void WiFiService::checkConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        connect();
    }
}

void WiFiService::printWiFiStatus() {
    Serial.println("\nWiFi Status:");
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Signal Strength (RSSI): %d dBm\n", WiFi.RSSI());
}
