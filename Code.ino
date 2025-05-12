#include <Arduino.h>
#include <BluetoothSerial.h>
#include <LiquidCrystal_I2C.h>
#include <ZMPT101B.h>

// Function prototypes
float getVPP();
void getPower();
void getACCurrent();
void getACVoltage();

// Defines
// ACS712
#define ACS712_PIN          26  // GPIO26 // D26 // ADC2_CHANNEL_9

// ZMPT101B
#define ZMPT101B_PIN        13  // GPIO13 // D13 // ADC2_CHANNEL_4

// 16x2 I2C LCD
#define I2C_SDA             21  // GPIO21 // D21 // GPIO_OUTPUT // I2C SDA
#define I2C_SCL             22  // GPIO22 // D22 // GPIO_OUTPUT // I2C SCL

// Relay 2Channel 
#define RELAY1_PIN_IN2      32  // GPIO32 // D32 // GPIO_OUTPUT // Relay 1

// Push buttons
#define PUSH_BUTTON_1       14  // GPIO14 // D14 // GPIO_INPUT // Push button 1

// LEDS
#define LED_RED             17  // GPIO17 // D17 // GPIO_OUTPUT_LED_RED
#define LED_BLUE_1          16  // GPIO16 // D16 // GPIO_OUTPUT_LED_BLUE_1
#define LED_BLUE_2          4   // GPIO4  // D4  // GPIO_OUTPUT_LED_BLUE_2

// Constants
#define SENSITIVITY         648.75f
#define ADC_RESOLUTION      4096
#define ADC_MAX_VOLTAGE     3.3

// Variables
int lcdColumns = 16;
int lcdRows = 2;
int mVperAmp = 185;           // 5A version of the ACS712
int Watt = 0;

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

// Battery Variables
int batteryPercentage = 0;
int batteryCycle = 0;

bool chargeComplete = false;
bool dischargeComplete = false;

// Objects
BluetoothSerial SerialBT; // Bluetooth
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); // LCD address 0x27 for 16x2 display
ZMPT101B voltageSensor(ZMPT101B_PIN, 50.0); // 50Hz

// Buffer for incoming Bluetooth data
String btBuffer = "";

// Setup
void setup() {
  Serial.begin(115200); // Initialize Serial port
  SerialBT.begin("ESP32_BT"); // Bluetooth device name
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on LCD backlight 
  pinMode(ACS712_PIN, INPUT);
  pinMode(RELAY1_PIN_IN2, OUTPUT);
  pinMode(PUSH_BUTTON_1, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE_1, OUTPUT);
  pinMode(LED_BLUE_2, OUTPUT);

  voltageSensor.setSensitivity(SENSITIVITY); // Set sensor sensitivity

  Serial.println("Setup Complete. Bluetooth Started. Ready to pair.");
}

// Loop
void loop() {
  // Handle incoming Bluetooth data
  handleBluetoothData();

  // Update LCD with sensor data (Current, Voltage, Power)
  static unsigned long lastUpdate = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= 2000) { // Update every 2 seconds
    updateLCD();
    lastUpdate = currentMillis;
  }

  delay(100); // Small delay to prevent overwhelming the loop
}

// Function to handle incoming Bluetooth data
void handleBluetoothData() {
  while (SerialBT.available()) {
    char incoming = SerialBT.read();
    // Append incoming character to buffer
    btBuffer += incoming;

    // Check if buffer has at least 3 characters
    if (btBuffer.length() >= 3) {
      // Extract the first 3 characters
      String packet = btBuffer.substring(0, 3);
      btBuffer.remove(0, 3); // Remove the processed characters from buffer

      // Parse the packet
      parseBatteryData(packet);
    }
  }
}

// Function to parse battery data packet
void parseBatteryData(String packet) {
  if (packet.length() != 3) {
    Serial.println("Invalid packet length.");
    return;
  }

  // Extract battery percentage and cycle count
  String percentStr = packet.substring(0, 2);
  String cycleStr = packet.substring(2, 3);

  // Convert to integers
  batteryPercentage = percentStr.toInt();
  batteryCycle = cycleStr.toInt();

  Serial.println("Parsed Data -> Battery Percentage: " + String(batteryPercentage) + "%, Battery Cycle: " + String(batteryCycle));

  // Update battery cycle based on charging and discharging
  updateBatteryCycle();
  
  // Send updated cycle count back via Bluetooth
  sendUpdatedCycle();
}

// Function to update battery cycle count
void updateBatteryCycle() {
  if (batteryPercentage > 95) {
    chargeComplete = true;
    Serial.println("Charging Complete.");
  }

  if (batteryPercentage < 5 && chargeComplete) {
    dischargeComplete = true;
    Serial.println("Discharging Complete.");
  }

  if (chargeComplete && dischargeComplete) {
    batteryCycle++;
    chargeComplete = false;
    dischargeComplete = false;
    Serial.println("Battery Cycle Incremented. New Cycle Count: " + String(batteryCycle));
  }
}

// Function to send updated cycle count back via Bluetooth
void sendUpdatedCycle() {
  String cycleStr = String(batteryCycle);
  SerialBT.print(cycleStr);
  SerialBT.println(); // Optional: Send a newline for clarity
  Serial.println("Sent Updated Cycle Count: " + cycleStr);
}

// Function to update the LCD with sensor data (Current, Voltage, Power)
void updateLCD() {
  getACVoltage();
  getACCurrent();
  getPower();
  
  // Display only the necessary data
  lcd.setCursor(0, 0);
  lcd.print("V: " + String(VRMS, 2) + "V");

  lcd.setCursor(0, 1);
  lcd.print("A: " + String(AmpsRMS, 2) + "A P: " + String(Watt) + "W");
}

// Function to get AC Voltage
void getACVoltage() {
  float voltage = voltageSensor.getRmsVoltage();
  VRMS = voltage;
  Serial.println("AC Voltage: " + String(voltage) + " V");
}

// Function to get AC Current
void getACCurrent() {
  Voltage = getVPP();
  VRMS = (Voltage / 2.0) * 0.707;   // RMS calculation
  AmpsRMS = ((VRMS * 1000) / mVperAmp) - 0.3; // Adjust for sensor error
  Serial.println("AC Current: " + String(AmpsRMS) + " A");
}

// Function to get Power
void getPower() {
  Watt = (AmpsRMS * 240 / 1.2); // Power calculation with calibration
  Serial.println("Power: " + String(Watt) + " W");
}

// Function to get Peak-to-Peak Voltage
float getVPP() {
  float result;
  int readValue;                // Value read from the sensor
  int maxValue = 0;             // Store max value here
  int minValue = ADC_RESOLUTION; // Store min value here
  
  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) { // Sample for 1 second
    readValue = analogRead(ACS712_PIN);
    if (readValue > maxValue) {
      maxValue = readValue;
    }
    if (readValue < minValue) {
      minValue = readValue;
    }
  }
  
  // Subtract min from max to get peak-to-peak voltage
  result = ((maxValue - minValue) * ADC_MAX_VOLTAGE) / ADC_RESOLUTION;
  return result;
}

// Function to handle additional Bluetooth commands (e.g., LEDs, Relays)
void handleAdditionalBluetoothCommands(char command) {
  if (command == '1') {
    digitalWrite(LED_BLUE_1, HIGH);
    Serial.println("LED_BLUE_1 turned ON.");
  } else if (command == '0') {
    digitalWrite(LED_BLUE_1, LOW);
    Serial.println("LED_BLUE_1 turned OFF.");
  }
}
