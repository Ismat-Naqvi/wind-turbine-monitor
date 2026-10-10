#include <DHT.h>

// DHT11 Sensor Pin
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// DC Motor (Voltage Generator) Pin
#define MOTOR_PIN A0

// Calibration constants
const float VOLTAGE_TO_WINDSPEED = 0.3;  // Adjust based on your motor characteristics
const float MIN_VOLTAGE = 0.2;           // Minimum voltage threshold
const float MAX_VOLTAGE = 5.0;           // Maximum voltage (5V for Arduino)

// Variables
float windSpeed = 0;
float temperature = 0;
float humidity = 0;
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000; // Read every 2 seconds

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize DHT sensor
  dht.begin();
  
  Serial.println("===================================");
  Serial.println("Wind Turbine Monitor System");
  Serial.println("Using: Arduino Uno + DHT11 + DC Motor");
  Serial.println("===================================");
  Serial.println("");
}

void loop() {
  // Read sensor data at specified interval
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();
    
    // Read voltage from DC motor (wind speed proxy)
    int rawVoltage = analogRead(MOTOR_PIN);
    float voltage = (rawVoltage / 1023.0) * 5.0; // Convert to 0-5V range
    
    // Calculate wind speed from voltage
    windSpeed = calculateWindSpeed(voltage);
    
    // Read temperature and humidity from DHT11
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    
    // Check for DHT read errors
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error: Failed to read from DHT11 sensor!");
      return;
    }
    
    // Display readings
    displayData(voltage, windSpeed, temperature, humidity);
  }
}

// Function to calculate wind speed from motor voltage
float calculateWindSpeed(float voltage) {
  // Filter out noise below minimum threshold
  if (voltage < MIN_VOLTAGE) {
    return 0.0;
  }
  
  // Convert voltage to wind speed
  // Assuming linear relationship: higher voltage = higher wind speed
  float speed = (voltage - MIN_VOLTAGE) * VOLTAGE_TO_WINDSPEED;
  
  // Cap at reasonable maximum
  if (speed > 100.0) {
    speed = 100.0;
  }
  
  return speed;
}

// Function to display all data on Serial Monitor
void displayData(float voltage, float windSpeed, float temperature, float humidity) {
  Serial.println("");
  Serial.println("===== WIND TURBINE MONITOR =====");
  Serial.print("Motor Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");
  
  Serial.print("Wind Speed: ");
  Serial.print(windSpeed, 2);
  Serial.println(" (m/s estimated)");
  
  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(humidity, 2);
  Serial.println(" %");
  
  // Status indicators
  Serial.print("Status: ");
  if (windSpeed < 2.0) {
    Serial.println("Low Wind - Standby");
  } else if (windSpeed < 8.0) {
    Serial.println("Normal Operation");
  } else if (windSpeed < 15.0) {
    Serial.println("High Wind - Caution");
  } else {
    Serial.println("Extreme Wind - Emergency!");
  }
  
  // Temperature warning
  if (temperature > 50.0) {
    Serial.println("⚠️  WARNING: Temperature too high!");
  }
  
  Serial.println("===============================");
}
