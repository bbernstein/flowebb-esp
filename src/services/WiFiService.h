#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "../config/wifi_credentials.h"

class WiFiService {
public:
    static void connect();
    static bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
    static void checkConnection();
    
private:
    static void printWiFiStatus();
};
