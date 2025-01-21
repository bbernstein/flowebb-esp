#include "TideService.h"
#include "WiFiService.h"

void TideService::feedWatchdog() {
    // Simplified watchdog feed that works with Arduino framework
    delay(1);  // Minimal yield to prevent watchdog from triggering
}

bool TideService::fetchTideData(TideData& tideData) {
    if (!WiFiService::isConnected()) {
        Serial.println("WiFi not connected");
        return false;
    }

    feedWatchdog();

    Serial.println("Creating secure client...");
    std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
    if (!client) {
        Serial.println("Failed to create client");
        return false;
    }
    
    client->setInsecure(); // Disable SSL certificate verification
    client->setTimeout(5); // Reduce timeout to 5 seconds
    
    HTTPClient http;
    http.setTimeout(5000); // 5 second timeout
    
    // Calculate time range
    time_t now = TimeService::getCurrentTime();
    time_t startTime = now - (12 * 60 * 60); // 12 hours ago (reduced from 24)
    time_t endTime = now + (12 * 60 * 60);   // 12 hours ahead
    
    String url = buildApiUrl(startTime, endTime);
    Serial.printf("Fetching from URL: %s\n", url.c_str());
    
    feedWatchdog();

    if (!http.begin(*client, url)) {
        Serial.println("HTTP begin failed");
        return false;
    }
    
    Serial.println("Starting HTTP GET...");
    int httpCode = http.GET();
    feedWatchdog();

    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    Serial.println("Reading response...");
    String payload = http.getString();
    http.end();

    feedWatchdog();
    
    if (payload.length() == 0) {
        Serial.println("Empty response received");
        return false;
    }

    Serial.println("Parsing JSON...");
    JSONVar doc = JSON.parse(payload);
    if (JSON.typeof(doc) == "undefined") {
        Serial.println("JSON parsing failed");
        return false;
    }

    feedWatchdog();

    // Update current tide data
    Serial.println("Updating tide data...");
    String typeStr = JSON.stringify(doc["tideType"]);
    if (typeStr.length() > 2) {
        tideData.type = typeStr.substring(1, typeStr.length() - 1); // Remove quotes
    } else {
        tideData.type = "UNKNOWN";
    }
    
    tideData.currentHeight = (double)doc["waterLevel"];
    tideData.lastUpdateTime = now;
    
    feedWatchdog();

    // Process tide extremes
    Serial.println("Processing extremes...");
    if (doc.hasOwnProperty("extremes")) {
        JSONVar extremes = doc["extremes"];
        processTideExtremes(extremes, tideData, now);
    } else {
        Serial.println("No extremes data in response");
        return false;
    }

    Serial.println("Tide data fetch completed successfully");
    return true;
}

String TideService::buildApiUrl(time_t startTime, time_t endTime) {
    String startEncoded = urlEncodeDateTime(startTime);
    String endEncoded = urlEncodeDateTime(endTime);
    
    return String(TIDE_API_ENDPOINT) + 
           "?stationId=" + String(TIDE_STATION_ID) + 
           "&startDateTime=" + startEncoded + 
           "&endDateTime=" + endEncoded;
}

String TideService::urlEncodeDateTime(time_t timestamp) {
    char dateBuff[25];
    struct tm timeinfo;
    localtime_r(&timestamp, &timeinfo);
    strftime(dateBuff, sizeof(dateBuff), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    
    String encoded = String(dateBuff);
    encoded.replace(":", "%3A");
    return encoded;
}

void TideService::processTideExtremes(JSONVar& extremes, TideData& tideData, time_t now) {
    // Find most recent past extreme
    int mostRecentPastIndex = -1;
    time_t now_utc = now + GMT_OFFSET_SEC + DAYLIGHT_OFFSET_SEC;
    
    Serial.printf("Processing %d extremes...\n", (int)extremes.length());
    
    for (int i = 0; i < extremes.length(); i++) {
        feedWatchdog();  // Feed watchdog during long loop
        
        double rawTimestamp = (double)extremes[i]["timestamp"];
        time_t timestamp = (time_t)(rawTimestamp / 1000) - GMT_OFFSET_SEC - DAYLIGHT_OFFSET_SEC;
        if (timestamp <= now) {
            if (mostRecentPastIndex == -1 || 
                timestamp > (time_t)((double)extremes[mostRecentPastIndex]["timestamp"] / 1000) - GMT_OFFSET_SEC - DAYLIGHT_OFFSET_SEC) {
                mostRecentPastIndex = i;
            }
        }
    }

    // Store most recent past extreme
    if (mostRecentPastIndex != -1) {
        Serial.println("Found recent past extreme");
        time_t timestamp = (time_t)((double)extremes[mostRecentPastIndex]["timestamp"] / 1000);
        tideData.current.timestamp = timestamp;
        tideData.current.height = (double)extremes[mostRecentPastIndex]["height"];
        String typeStr = JSON.stringify(extremes[mostRecentPastIndex]["type"]);
        tideData.current.isHigh = typeStr.indexOf("HIGH") != -1;
    }

    feedWatchdog();

    // Store future extremes
    Serial.println("Processing future extremes...");
    tideData.numExtremes = 0;
    for (int i = 0; i < extremes.length() && tideData.numExtremes < MAX_EXTREMES; i++) {
        feedWatchdog();  // Feed watchdog during long loop
        
        time_t timestamp = (time_t)((double)extremes[i]["timestamp"] / 1000);
        if (timestamp > now) {
            tideData.extremes[tideData.numExtremes].timestamp = timestamp;
            tideData.extremes[tideData.numExtremes].height = (double)extremes[i]["height"];
            String typeStr = JSON.stringify(extremes[i]["type"]);
            tideData.extremes[tideData.numExtremes].isHigh = typeStr.indexOf("HIGH") != -1;
            tideData.numExtremes++;
        }
    }
    Serial.printf("Stored %d future extremes\n", tideData.numExtremes);
}
