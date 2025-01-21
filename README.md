# ESP32-S3 Tide Tracker

An ESP32-S3 based tide tracking device that displays tide status using a NeoPixel LED. The LED color and animation indicate the current tide state (rising/falling) and time until the next tide extreme.

## Features

- Real-time tide tracking using online tide API
- Visual tide status indication using NeoPixel LED
- Automatic time synchronization via NTP
- Persistent storage of tide data
- Automatic recovery and failsafe mechanisms
- WiFi connectivity with connection monitoring

## Hardware Requirements

- ESP32-S3 development board (tested with ESP32-S3-DevKitM-1)
- WS2812B NeoPixel LED
- USB-C cable for programming
- Power supply (via USB or external)

## Dependencies

- PlatformIO IDE
- Arduino framework for ESP32-S3
- Required libraries (automatically installed via PlatformIO):
  - Adafruit NeoPixel
  - Arduino_JSON

## Setup Instructions

1. Clone this repository:
   ```bash
   git clone https://github.com/bbernstein/flowebb-esp
   cd flowebb-esp
   ```

2. Copy and configure WiFi credentials:
   ```bash
   cp src/config/wifi_credentials.h.template src/config/wifi_credentials.h
   ```
   Edit `src/config/wifi_credentials.h` with your WiFi credentials and API endpoint.

3. Configure your location:
   Edit `src/config/config.h` and update:
   - `LATITUDE` and `LONGITUDE` for your location
   - `TIDE_STATION_ID` with your nearest NOAA tide station ID
   - Time zone settings (`GMT_OFFSET_SEC` and `DAYLIGHT_OFFSET_SEC`)

4. Build and upload using PlatformIO:
   ```bash
   pio run -t upload
   ```

5. Monitor serial output:
   ```bash
   pio device monitor
   ```

## LED Indicators

- Red -> Green: Rising tide
- Green -> Red: Falling tide
- Blue animation: Wave effect just to feel like the sea

## Configuration

Key configuration files:
- `src/config/config.h`: General configuration settings
- `src/config/wifi_credentials.h`: Network and API credentials
- `platformio.ini`: Build configuration and library dependencies

## Development

To modify or extend this project:

1. Main application logic is in `src/main.cpp`
2. LED control logic is in `src/display/LedController.cpp`
3. Tide data processing is in `src/services/TideService.cpp`
4. Data models are in `src/models/`

## Troubleshooting

1. LED not working:
   - Check LED_PIN configuration in config.h
   - Verify NeoPixel connections
   - Try adjusting BRIGHTNESS setting

2. WiFi connection issues:
   - Verify credentials in wifi_credentials.h
   - Check WiFi signal strength
   - Monitor serial output for connection status

3. No tide data:
   - Verify TIDE_STATION_ID is correct
   - Check API endpoint configuration
   - Monitor serial output for API responses

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- NOAA for tide data
- Adafruit for NeoPixel library
- ESP32 community for various examples and inspiration

## Project Structure

```
esp32-tide-tracker/
├── src/
│   ├── main.cpp           # Main application code
│   ├── config/           # Configuration files
│   ├── display/          # LED control code
│   ├── models/           # Data structures
│   ├── services/         # Core services
│   ├── storage/          # Data persistence
│   └── utils/            # Utility functions
├── lib/                  # Project libraries
├── include/             # Header files
├── test/                # Test files
└── docs/                # Documentation
```
