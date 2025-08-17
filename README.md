# Battery Cycle Management System

A smart battery management system that monitors and maintains battery cycles while providing real-time power monitoring and automatic cutoff features. This project was developed as part of my 4th semester coursework.

## Features

### Core Functionality
- Real-time battery cycle monitoring and management
- Automatic power cutoff at 95% charge level
- Battery percentage and cycle count tracking
- Power consumption metrics (Current, Voltage, Watts)

### Hardware Integration
- Bluetooth connectivity with mobile app interface
- Real-time display on 16x2 LCD screen
- Relay-based power control system
- LED status indicators

### Data Management
- Firebase integration for data storage
- Historical cycle data tracking
- Power consumption analytics

## Hardware Components

### Sensors
- ZMCT103C Current Sensor (5A version)
- ZMPT101B Voltage Sensor (220V AC)

### Control & Display
- ESP32 Microcontroller
- 16x2 I2C LCD Display
- 2-Channel Relay Module
- Push Buttons
- Status LEDs (Red, Blue)

### Power Supply
- 220V to 5V 1.5A converter

## Pin Configuration

### ESP32 Pins
- ACS712: GPIO26 (ADC2_CH9)
- ZMPT101B: GPIO13 (ADC2_CH4)
- I2C LCD: 
  - SDA: GPIO21
  - SCL: GPIO22
- Relay: GPIO32
- Push Button: GPIO14
- LEDs:
  - Red: GPIO17
  - Blue1: GPIO16
  - Blue2: GPIO4

## Setup and Installation

### Hardware Setup
1. Connect components according to pin configuration
2. Ensure proper power supply connections
3. Verify sensor calibration

### Software Setup
1. Install required development tools:
   - Arduino IDE
   - VS Code (optional)
   - MIT App Inventor
2. Configure Firebase:
   - Create new Firebase project
   - Set up Realtime Database
   - Add authentication credentials
3. Upload ESP32 code
4. Install and configure mobile app

## Mobile Application

### APK Installation
1. Download the provided APK file (`BatteryCycleManager.apk`)
2. Install the APK on your Android device
3. Allow necessary permissions when prompted:
   - Bluetooth
   - Location (required for Bluetooth scanning)
   - Storage (for data logging)

### App Features
1. Device Connection:
   - Scan for ESP32 device ("ESP32_BT")
   - Connect/Disconnect functionality
   - Connection status indicator

2. Real-time Monitoring:
   - Battery percentage display
   - Current cycle count
   - Power consumption metrics:
     - Current (A)
     - Voltage (V)
     - Power (W)

3. Data Management:
   - Historical cycle data
   - Power usage graphs
   - Firebase synchronization
   - Export data functionality

### Troubleshooting
- Ensure Bluetooth is enabled on your device
- Check if ESP32 is powered and in range
- Restart app if connection fails
- Clear app cache if experiencing issues

### Minimum Requirements
- Android 6.0 (Marshmallow) or higher
- Bluetooth 4.0+ support
- 50MB free storage

## Usage

### Initial Setup
1. Power up the system
2. Connect mobile app via Bluetooth
3. Verify LCD display functionality
4. Check sensor readings

### Operation Modes
1. Normal Operation:
   - Monitors battery percentage
   - Tracks charging cycles
   - Displays power metrics
2. Protection Mode:
   - Automatic cutoff at 95% charge
   - Low battery warnings
   - Overcharge protection

### Mobile App Features
- Real-time battery status
- Cycle count display
- Power consumption metrics
- Historical data viewing

## Future Improvements

- Machine learning for battery life prediction
- Solar charging integration
- Extended device compatibility
- Enhanced power analytics
- Remote monitoring capabilities


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

- Karthik S

## Acknowledgments

- RV College of Engineering, Bangalore

