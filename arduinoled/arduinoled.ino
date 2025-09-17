#include <LiquidCrystal.h>
#include <DHT.h>
#include <MQ135.h>
#include <Servo.h>

// Initialize the LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Pin Definitions
#define LDR_PIN A1           // LDR connected to Analog Pin A1
#define SOIL_PIN A2          // Soil Moisture Sensor connected to Analog Pin A2
#define DHT_PIN 2            // DHT11 connected to Digital Pin 2
#define MQ135_PIN A0         // MQ135 Gas Sensor connected to Analog Pin A0
#define LED_PIN 3            // LED connected to Digital Pin 3
#define MOTOR_TEMP_PIN 4     // DC Motor 1 (Temperature-based) connected to Digital Pin 4
#define MOTOR_SOIL_PIN 5     // DC Motor 2 (Soil Moisture-based) connected to Digital Pin 5
#define SERVO_PIN 6          // Servo Motor connected to Digital Pin 6

// Thresholds
int ldrThreshold = 500;      // LDR Threshold for LED
int soilThreshold = 600;     // Soil Moisture Threshold for motor
float tempThreshold = 24.0;  // Temperature Threshold for motor
float co2Threshold = 1.0;    // CO2 Threshold for Servo Motor activation (1 ppm)

// DHT11 Setup
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// MQ135 Setup
MQ135 gasSensor(MQ135_PIN);

// Servo Motor Setup
Servo servoMotor;

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  // Pin Setup
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_TEMP_PIN, OUTPUT); // DC Motor for Temperature
  pinMode(MOTOR_SOIL_PIN, OUTPUT); // DC Motor for Soil Moisture
  Serial.begin(9600);

  // Initialize DHT11
  dht.begin();

  // Attach Servo Motor
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0); // Default position: 0 degrees
}

void loop() {
  // LDR Reading
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  lcd.setCursor(0, 0); // Display LDR
  lcd.print("Light: ");
  lcd.print(ldrValue);

  if (ldrValue < ldrThreshold) {
    digitalWrite(LED_PIN, HIGH); // Turn on LED
    Serial.println("LED ON: Low light detected.");
    lcd.print(" (Dark)");
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off LED
    Serial.println("LED OFF: Sufficient light.");
  }

  delay(500);

  // Soil Moisture Reading
  int soilValue = analogRead(SOIL_PIN);
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilValue);

  lcd.setCursor(0, 1); // Display Soil Moisture value and motor state
  lcd.print("Soil: ");
  lcd.print(soilValue);

  // Check Soil Moisture Threshold
  if (soilValue > soilThreshold) {
    digitalWrite(MOTOR_SOIL_PIN, HIGH); // Turn on Soil Moisture Motor
    Serial.println("Soil Motor ON: High Moisture Detected");
    lcd.print(" M:ON");
  } else {
    digitalWrite(MOTOR_SOIL_PIN, LOW); // Turn off Soil Moisture Motor
    Serial.println("Soil Motor OFF: Moisture Normal");
    lcd.print(" M:OFF");
  }

  delay(500);

  // DHT11 Readings
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT11 sensor!");
    lcd.setCursor(0, 0);
    lcd.print("DHT Error!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    lcd.clear(); // Update LCD for DHT11 readings
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print(" %");

    // Check Temperature Threshold
    if (temp > tempThreshold) {
      digitalWrite(MOTOR_TEMP_PIN, HIGH); // Turn on Temperature Motor
      Serial.println("Temp Motor ON: High Temp Detected");
      lcd.setCursor(0, 1);
      lcd.print(" T-Motor:ON");
    } else {
      digitalWrite(MOTOR_TEMP_PIN, LOW); // Turn off Temperature Motor
      Serial.println("Temp Motor OFF: Temp Normal");
      lcd.setCursor(0, 1);
      lcd.print(" T-Motor:OFF");
    }
  }

  delay(1000);

  // MQ135 Gas Sensor Reading
  float gasPPM = gasSensor.getPPM();
  Serial.print("Air Quality (CO2 Concentration): ");
  Serial.print(gasPPM);
  Serial.println(" ppm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  lcd.print(gasPPM);
  lcd.print(" ppm");

  // Check CO2 Threshold for Servo Motor Activation
  if (gasPPM > co2Threshold) {
    servoMotor.write(90); // Move Servo to 90 degrees
    Serial.println("Servo Motor Rotated to 90°: High CO2 Detected");
    lcd.setCursor(0, 1);
    lcd.print("Servo: ON ");
  } else {
    servoMotor.write(0); // Move Servo back to 0 degrees
    Serial.println("Servo Motor Rotated to 0°: CO2 Normal");
    lcd.setCursor(0, 1);
    lcd.print("Servo: OFF");
  }

  delay(2000); // Delay before the next loop iteration
}
