# Flowebb Go Backend

This document describes the GraphQL API available in the Flowebb backend service.

## Prerequisites

- Go 1.21 or later
- Docker (required for running tests and local development)
- AWS credentials configured (for deployment)

## Getting Started

1. Clone the repository
2. Install dependencies:
```bash
go mod download
```
3. Run tests:
```bash
docker-compose up -d  # Start required services
go test ./...
```

## GraphQL API

The API is accessible through a single GraphQL endpoint. All data requests are made using GraphQL queries.

### Schema

```graphql
type Query {
    # Get tide stations near a location or all stations
    stations(
        lat: Float,    # Latitude (-90 to 90)
        lon: Float,    # Longitude (-180 to 180)
        limit: Int     # Maximum number of stations to return
    ): [Station!]!

    # Get tide predictions for a station
    tides(
        stationId: ID!,           # Station identifier
        startDateTime: String!,    # Start time (ISO8601 format)
        endDateTime: String!       # End time (ISO8601 format)
    ): TideData!
}

type Station {
    id: ID!                    # Station identifier
    name: String!             # Station name
    state: String            # State/region where station is located
    region: String           # Region information
    distance: Float!         # Distance from requested coordinates in kilometers
    latitude: Float!         # Station latitude in decimal degrees
    longitude: Float!        # Station longitude in decimal degrees
    source: String!          # Data source (NOAA, UKHO, or CHS)
    capabilities: [String!]! # Array of station capabilities
    timeZoneOffset: Int!     # Timezone offset in seconds
}

type TideData {
    timestamp: Int!               # Current timestamp
    localTime: String!           # Local time in ISO8601 format
    waterLevel: Float!           # Current water level in feet
    predictedLevel: Float!       # Predicted water level in feet
    nearestStation: String!      # ID of the nearest station
    location: String            # Location name
    latitude: Float!            # Location latitude
    longitude: Float!           # Location longitude
    stationDistance: Float!     # Distance to station in kilometers
    tideType: String!           # Current tide type: "RISING", "FALLING", "HIGH", or "LOW"
    calculationMethod: String!  # Method used for calculations
    predictions: [TidePrediction!]! # Array of tide predictions
    extremes: [TideExtreme!]!      # Array of tide extremes
    timeZoneOffsetSeconds: Int!    # Station's timezone offset in seconds
}

type TidePrediction {
    timestamp: Int!     # Time in milliseconds
    localTime: String! # Local time in ISO8601 format
    height: Float!     # Water height in feet
}

type TideExtreme {
    type: String!      # Tide type ("HIGH" or "LOW")
    timestamp: Int!    # Time in milliseconds
    localTime: String! # Local time in ISO8601 format
    height: Float!     # Water height in feet
}
```

### Example Queries

1. Get nearby stations:
```bash
curl -X POST -H "Content-Type: application/json" \
  -d '{
    "query": "query GetStations($lat: Float!, $lon: Float!, $limit: Int) { 
      stations(lat: $lat, lon: $lon, limit: $limit) { 
        id 
        name 
        distance 
        latitude 
        longitude 
      } 
    }",
    "variables": {
      "lat": 37.8199,
      "lon": -122.4783,
      "limit": 5
    }
  }' \
  http://localhost:8080/query
```

2. Get tide predictions for a station:
```bash
curl -X POST -H "Content-Type: application/json" \
  -d '{
    "query": "query GetTides($stationId: ID!, $start: String!, $end: String!) { 
      tides(stationId: $stationId, startDateTime: $start, endDateTime: $end) { 
        waterLevel 
        tideType 
        predictions { 
          localTime 
          height 
        } 
        extremes { 
          type 
          localTime 
          height 
        } 
      } 
    }",
    "variables": {
      "stationId": "9414290",
      "start": "2024-01-30T00:00:00Z",
      "end": "2024-01-31T00:00:00Z"
    }
  }' \
  http://localhost:8080/query
```

## Project Structure

- `/cmd/graphql`: Main Lambda function entry point
- `/graph`: GraphQL schema and resolvers
- `/internal`:
  - `/api`: HTTP API handlers
  - `/cache`: Caching implementations (LRU, DynamoDB, S3)
  - `/models`: Data models and interfaces
  - `/station`: Station finder implementation
  - `/tide`: Tide prediction service
- `/pkg`: Shared packages

## Development

The service is designed to run as an AWS Lambda function but can be run locally for development:

1. Start required services:
```bash
docker-compose up -d
```

2. Run the service locally:
```bash
go run cmd/graphql/main.go
```

## Testing

The project includes unit tests and integration tests. Docker is required for running integration tests that use DynamoDB and S3.

1. Start test dependencies:
```bash
docker-compose up -d
```

2. Run tests:
```bash
go test ./...
```

## Notes

- All timestamps are in Unix milliseconds format
- Local times are in ISO8601 format
- Distances are returned in kilometers
- Water heights are returned in feet
- Latitude must be between -90 and 90 degrees
- Longitude must be between -180 and 180 degrees
- Timezone offsets are in seconds
- The API supports multiple data sources: NOAA (US), UKHO (UK), and CHS (Canada)
- Responses are cached using a multi-layer caching strategy (LRU, DynamoDB)
