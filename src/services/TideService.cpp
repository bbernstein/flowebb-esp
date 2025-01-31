/*
 * Created on Tue Jan 21 2025
 *
 * Copyright (c) 2025 Bernard Bernstein
 */

#include "TideService.h"
#include "WiFiService.h"

bool TideService::fetchTideData(TideData& tideData) {
    if (!WiFiService::isConnected()) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("WiFi not connected");
        }
        return false;
    }

    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Creating secure client...");
    }
    std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
    if (!client) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("Failed to create client");
        }
        return false;
    }
    
    client->setInsecure(); // Disable SSL certificate verification
    client->setTimeout(5); // Reduce timeout to 5 seconds
    
    HTTPClient http;
    http.setTimeout(5000); // 5 second timeout
    
    // Calculate time range
    time_t now = TimeService::getCurrentTime();
    time_t startTime = now - (24 * 60 * 60); // 12 hours ago (reduced from 24)
    time_t endTime = now + (24 * 5 * 60 * 60);   // 12 hours ahead
    
    String query = buildGraphQLQuery(startTime, endTime);
    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("GraphQL query: %s\n", query.c_str());
    }
    
    if (!http.begin(*client, TIDE_API_ENDPOINT)) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("HTTP begin failed");
        }
        return false;
    }

    http.addHeader("Content-Type", "application/json");
    
    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Starting HTTP POST...");
    }
    int httpCode = http.POST(query);

    if (httpCode != HTTP_CODE_OK) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
        return false;
    }

    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Reading response...");
    }
    String payload = http.getString();
    http.end();

    if (payload.length() == 0) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("Empty response received");
        }
        return false;
    }

    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("Response length: %d\n", payload.length());
        Serial.printf("Response: %s\n", payload.c_str());
        Serial.println("Parsing JSON...");
    }
    JSONVar doc = JSON.parse(payload);
    if (JSON.typeof(doc) == "undefined") {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("JSON parsing failed");
        }
        return false;
    }

    // Navigate through GraphQL response structure
    if (!doc.hasOwnProperty("data") || !doc["data"].hasOwnProperty("tides")) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("Invalid GraphQL response structure");
        }
        return false;
    }

    JSONVar tides = doc["data"]["tides"];

    // Update current tide data
    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Updating tide data...");
    }
    String typeStr = JSON.stringify(tides["tideType"]);
    if (typeStr.length() > 2) {
        tideData.type = typeStr.substring(1, typeStr.length() - 1); // Remove quotes
    } else {
        tideData.type = "UNKNOWN";
    }
    
    tideData.currentHeight = (double)tides["waterLevel"];
    tideData.lastUpdateTime = now;

    // Process tide extremes
    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Processing extremes...");
    }
    if (tides.hasOwnProperty("extremes")) {
        JSONVar extremes = tides["extremes"];
        processTideExtremes(extremes, tideData, now);
    } else {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("No extremes data in response");
        }
        return false;
    }

    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Tide data fetch completed successfully");
    }
    return true;
}

String TideService::buildGraphQLQuery(time_t startTime, time_t endTime) {
    char startBuff[25], endBuff[25];
    struct tm timeinfo;
    
    localtime_r(&startTime, &timeinfo);
    strftime(startBuff, sizeof(startBuff), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    
    localtime_r(&endTime, &timeinfo);
    strftime(endBuff, sizeof(endBuff), "%Y-%m-%dT%H:%M:%S", &timeinfo);

    String query = "{"
                  "\"operationName\": \"GetTides\","
                  "\"variables\": {"
                  "\"stationId\": \"" + String(TIDE_STATION_ID) + "\","
                  "\"startDateTime\": \"" + String(startBuff) + "\","
                  "\"endDateTime\": \"" + String(endBuff) + "\""
                  "},"
                  "\"query\": \"query GetTides($stationId: ID!, $startDateTime: String!, $endDateTime: String!) {\\n"
                  "  tides(\\n"
                  "    stationId: $stationId\\n"
                  "    startDateTime: $startDateTime\\n"
                  "    endDateTime: $endDateTime\\n"
                  "  ) {\\n"
                  "    localTime\\n"
                  "    waterLevel\\n"
                  "    tideType\\n"
                  "    timeZoneOffsetSeconds\\n"
                  "    extremes {\\n"
                  "      type\\n"
                  "      timestamp\\n"
                  "      height\\n"
                  "    }\\n"
                  "  }\\n"
                  "}\""
                  "}";
    return query;
}

void TideService::processTideExtremes(JSONVar& extremes, TideData& tideData, time_t now) {
    // Find most recent past extreme
    int mostRecentPastIndex = -1;
    time_t now_utc = now + GMT_OFFSET_SEC + DAYLIGHT_OFFSET_SEC;
    
    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("Processing %d extremes...\n", (int)extremes.length());
    }
    
    for (int i = 0; i < extremes.length(); i++) {
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
        if (ENABLE_DEBUG_PRINTS) {
            Serial.println("Found recent past extreme");
        }
        time_t timestamp = (time_t)((double)extremes[mostRecentPastIndex]["timestamp"] / 1000);
        tideData.current.timestamp = timestamp;
        tideData.current.height = (double)extremes[mostRecentPastIndex]["height"];
        String typeStr = JSON.stringify(extremes[mostRecentPastIndex]["type"]);
        tideData.current.isHigh = typeStr.indexOf("HIGH") != -1;
    }

    // Store future extremes
    if (ENABLE_DEBUG_PRINTS) {
        Serial.println("Processing future extremes...");
    }
    tideData.numExtremes = 0;
    for (int i = 0; i < extremes.length() && tideData.numExtremes < MAX_EXTREMES; i++) {
        time_t timestamp = (time_t)((double)extremes[i]["timestamp"] / 1000);
        if (timestamp > now) {
            tideData.extremes[tideData.numExtremes].timestamp = timestamp;
            tideData.extremes[tideData.numExtremes].height = (double)extremes[i]["height"];
            String typeStr = JSON.stringify(extremes[i]["type"]);
            tideData.extremes[tideData.numExtremes].isHigh = typeStr.indexOf("HIGH") != -1;
            tideData.numExtremes++;
        }
    }
    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("Stored %d future extremes\n", tideData.numExtremes);
    }
}
