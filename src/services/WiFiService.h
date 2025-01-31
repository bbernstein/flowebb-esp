#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "../config/wifi_credentials.h"

class WiFiService {
public:
    static bool connect();
    static void disconnect();
    static bool checkConnection();
    
    // Make isConnected accessible but read-only to other classes
    static bool isConnected() { return _isConnected; }
    
private:
    static void printWiFiStatus();
    static bool _isConnected;  // Internal state tracking
};
