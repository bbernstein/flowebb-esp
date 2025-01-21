#include "JsonHelper.h"

String JsonHelper::serializeTideData(const TideData& tideData) {
    JSONVar tideJson;
    
    tideJson["type"] = tideData.type;
    tideJson["currentHeight"] = tideData.currentHeight;
    tideJson["lastUpdateTime"] = (double)tideData.lastUpdateTime;
    
    // Serialize current extreme
    tideJson["current"] = serializeExtreme(tideData.current);
    
    // Serialize future extremes array
    JSONVar extremesArray;
    for(int i = 0; i < tideData.numExtremes; i++) {
        extremesArray[i] = serializeExtreme(tideData.extremes[i]);
    }
    tideJson["extremes"] = extremesArray;
    tideJson["numExtremes"] = tideData.numExtremes;
    
    return JSON.stringify(tideJson);
}

bool JsonHelper::deserializeTideData(const String& jsonString, TideData& tideData) {
    if (jsonString.length() == 0) {
        return false;
    }
    
    JSONVar tideJson = JSON.parse(jsonString);
    if (JSON.typeof(tideJson) == "undefined") {
        return false;
    }
    
    // Load basic data
    tideData.type = (const char*)tideJson["type"];
    tideData.currentHeight = (double)tideJson["currentHeight"];
    tideData.lastUpdateTime = (unsigned long)((double)tideJson["lastUpdateTime"]);
    
    // Load current extreme
    deserializeExtreme(tideJson["current"], tideData.current);
    
    // Load future extremes
    tideData.numExtremes = (int)tideJson["numExtremes"];
    JSONVar extremesArray = tideJson["extremes"];
    for(int i = 0; i < tideData.numExtremes; i++) {
        deserializeExtreme(extremesArray[i], tideData.extremes[i]);
    }
    
    return true;
}

JSONVar JsonHelper::serializeExtreme(const TideExtreme& extreme) {
    JSONVar jsonExtreme;
    time_t timestamp = extreme.timestamp;
    adjustTimestampForTimezone(timestamp, true);
    
    jsonExtreme["timestamp"] = (double)timestamp;
    jsonExtreme["height"] = extreme.height;
    jsonExtreme["isHigh"] = extreme.isHigh;
    
    return jsonExtreme;
}

void JsonHelper::deserializeExtreme(const JSONVar& jsonExtreme, TideExtreme& extreme) {
    extreme.timestamp = (time_t)((double)jsonExtreme["timestamp"]);
    adjustTimestampForTimezone(extreme.timestamp, false);
    extreme.height = (double)jsonExtreme["height"];
    extreme.isHigh = (bool)jsonExtreme["isHigh"];
}

void JsonHelper::adjustTimestampForTimezone(time_t& timestamp, bool toUTC) {
    if (toUTC) {
        timestamp += GMT_OFFSET_SEC + DAYLIGHT_OFFSET_SEC;
    } else {
        timestamp -= GMT_OFFSET_SEC + DAYLIGHT_OFFSET_SEC;
    }
}
