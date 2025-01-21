# Tide Chart API Documentation

This document describes the API endpoints available in the Tide Chart backend service.

## Endpoints

### 1. Get Tide Stations

```
GET /api/stations
```

Returns either a specific tide station by ID or a list of nearest tide measurement stations based on location parameters.

#### Query Parameters

Either use:
- `stationId` - ID of a specific station to retrieve

OR use location-based search:
- `lat` - Latitude of the user's location (-90 to 90 decimal degrees)
- `lon` - Longitude of the user's location (-180 to 180 decimal degrees)
- `limit` (optional) - Maximum number of stations to return (default: 5)

#### Response

```
{
  "stations": [
    {
      "id": "string",           // Station identifier
      "name": "string",         // Station name
      "state": "string",        // State/region where station is located (optional)
      "region": "string",       // Region information (optional)
      "distance": number,       // Distance from requested coordinates in kilometers
      "latitude": number,       // Station latitude in decimal degrees
      "longitude": number,      // Station longitude in decimal degrees
      "source": "string",       // Data source (NOAA, UKHO, or CHS)
      "capabilities": ["string"], // Array of station capabilities
      "timeZoneOffset": number, // Timezone offset in seconds
      "level": "string",        // Station level information (optional)
      "stationType": "string"   // Type of station (optional)
    }
  ]
}
```

### 2. Get Tide Predictions

```
GET /api/tides
```

Returns tide predictions for either a specific station or location.

#### Query Parameters

Either use:
- `stationId` - ID of a specific station to retrieve predictions for

OR use location-based search:
- `lat` - Latitude (-90 to 90 decimal degrees)
- `lon` - Longitude (-180 to 180 decimal degrees)

Optional time range parameters:
- `startDateTime` - Start time for predictions (ISO8601 format: "2024-01-01T00:00:00")
- `endDateTime` - End time for predictions (ISO8601 format: "2024-01-02T00:00:00")

Note: If time range is not specified, predictions will be returned for the current day in the station's timezone.
Maximum time range is 30 days.

#### Response

```
{
  "responseType": "tide",
  "timestamp": number,         // Current timestamp in milliseconds
  "localTime": "string",       // Local time in ISO8601 format
  "waterLevel": number,        // Current water level in feet
  "predictedLevel": number,    // Predicted water level in feet
  "nearestStation": "string",  // ID of the nearest station
  "location": "string",        // Location name
  "latitude": number,         // Location latitude
  "longitude": number,        // Location longitude
  "stationDistance": number,  // Distance to station in kilometers
  "tideType": "string",       // Current tide type: "RISING", "FALLING", "HIGH", or "LOW"
  "calculationMethod": "string", // Method used for calculations (e.g., "NOAA API")
  "timeZoneOffsetSeconds": number, // Station's timezone offset in seconds
  "extremes": [
    {
      "type": "string",       // Tide type ("HIGH" or "LOW")
      "timestamp": number,    // Time in milliseconds
      "localTime": "string",  // Local time in ISO8601 format
      "height": number       // Water height in feet
    }
  ],
  "predictions": [
    {
      "timestamp": number,    // Time in milliseconds
      "localTime": "string",  // Local time in ISO8601 format
      "height": number       // Water height in feet
    }
  ]
}
```

## Error Responses

Both endpoints may return the following error responses:

- `400 Bad Request` - When required parameters are missing or invalid
- `404 Not Found` - When a station is not found
- `500 Internal Server Error` - When an unexpected error occurs

Error response body:
```
{
  "responseType": "error",
  "error": "string"  // Error message describing what went wrong
}
```

## Notes

- All timestamps are in Unix milliseconds format
- Local times are in ISO8601 format (e.g., "2023-12-25T08:00:00")
- Distances are returned in kilometers
- Water heights are returned in meters
- Latitude must be between -90 and 90 degrees
- Longitude must be between -180 and 180 degrees
- Timezone offsets are in seconds and typically range from -43200 to +50400 (UTC-12 to UTC+14)
- The API supports multiple data sources: NOAA (US), UKHO (UK), and CHS (Canada)
- Responses are cached to improve performance and reduce external API calls
