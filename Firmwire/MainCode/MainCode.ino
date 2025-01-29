#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Keypad.h>

#include <SD.h>
#include <SPI.h>



#define DHT11_PIN 48         // DHT11 sensor pin
#define DHTTYPE DHT11        // DHT type
#define anemometerPin A14    // Pin for the anemometer
#define buzzerPin 49         // Pin for the buzzer
#define keypadRows 4
#define keypadCols 4
#define SD_PIN 53             // CS pin for SD card module

DHT dht11(DHT11_PIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD address (may vary)

const float maxAnalogValue = 1023.0;
const float maxWindSpeed = 32.4;     // Example maximum wind speed (m/s)
const float windSpeedThreshold = 15.0;
int timeInterval = 5;             // Default interval in milliseconds
int Duration = 100;
int Timepassed = 0;    


// Set up the Keypad
const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns
char keys[ROWS][COLS] = {
  {'D', '#', '0', '*'},
  {'C', '9', '8', '7'},
  {'B', '6', '5', '4'},
  {'A', '3', '2', '1'}
};
byte rowPins[ROWS] = {33, 35, 37, 23};   // Connect to the row pinouts of the keypad
byte colPins[COLS] = {25, 27, 31, 29};     // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


void setup() {
  Serial.begin(9600);
  dht11.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing!");
  // Initialize the SD card
  if (!SD.begin(SD_PIN)) {
    Serial.println("SD card initialization failed!");
    lcd.print("SD init failed!");
    while (1);  // Halt execution if SD fails
  }
  Serial.println("SD card ready!");
  
  
  delay(2000);
  lcd.clear();
  lcd.print("Welcome!");

  // Set up buzzer and LCD
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);

}

void loop() {
  lcd.clear();
  lcd.print("Select interval:");
  lcd.setCursor(0, 1);
  lcd.print("1:H 2:M  3:S");

  int intervalType = waitForKeypress();
  lcd.clear();

  if (intervalType == '1') {
    lcd.print("Set hours:");
    timeInterval = getIntervalValue() * 3600;

  } else if (intervalType == '2') {
    lcd.print("Set minutes:");
    timeInterval = getIntervalValue() * 60;
  } else if (intervalType == '3') {
    lcd.print("Set seconds:");
    timeInterval = getIntervalValue() * 1;

  }


  lcd.clear();
  lcd.print("Select duration:");
  lcd.setCursor(0, 1);
  lcd.print("1:H  2:M  3:S");

  intervalType = waitForKeypress();
  lcd.clear();

  if (intervalType == '1') {
    lcd.print("Set hours:");
    Duration = getIntervalValue() * 3600;

  } else if (intervalType == '2') {
    lcd.print("Set minutes:");
    Duration = getIntervalValue() * 60;
  } else if (intervalType == '3') {
    lcd.print("Set seconds:");
    Duration = getIntervalValue() * 1;

  }

  lcd.clear();
  lcd.print("Enter filename:");
  String filename = "data_" + getFileName();
Serial.println(filename);
  // Open file on SD card
  File dataFile = SD.open(filename, FILE_WRITE);


  if (!dataFile) {
    Serial.println("Error opening file!");
    lcd.print("File error!");
    return;
  }

  dataFile.print("Wind Speed (m/s)");
  dataFile.print(", Temperature (°C)");
  dataFile.print(", Humidity (%)");
  dataFile.flush();

  lcd.clear();
  lcd.print("Reading Sensors");
  // Main data logging loop
  while (Timepassed<Duration) {

    Timepassed = Timepassed + timeInterval;

    int sensorValue = analogRead(anemometerPin);
    float windSpeed = (sensorValue / maxAnalogValue) * maxWindSpeed;

    int humidity = dht11.readHumidity() + 50;
    int temperature = dht11.readTemperature() + 25;

    // Check if any DHT readings failed
    if (isnan(humidity) || isnan(temperature)) {
      lcd.clear();
      Serial.println("Failed to read from DHT sensor!");
      lcd.print("DHT error");
    } else {
      // Log and display data
      logData(dataFile, windSpeed, temperature, humidity);
      displayData(windSpeed, temperature, humidity);
    }

    if (windSpeed > windSpeedThreshold) {
      digitalWrite(buzzerPin, HIGH);
    } else {
      digitalWrite(buzzerPin, LOW);
    }

    delay(timeInterval*1000);
  }
}


// Helper function to wait for a key press
int waitForKeypress() {
  char key;
  while (true) {
    key = keypad.getKey();
    if (key) {  // If a key is pressed
      Serial.print("Key Pressed: ");
      Serial.println(key);
      return key;  // Return the key pressed
    }
  }
}

// Helper function to get interval value from keypadint getIntervalValue() {
  int getIntervalValue() {
  int value = 0;         // This will store the final integer value
  char key;
  // Serial.print("Enter value: ");
  // lcd.clear();
  // lcd.print("Enter value: ");
  lcd.setCursor(0, 1);
  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {  // Check if the key is a digit
        value = value * 10 + (key - '0'); // Convert char to integer and update value
        Serial.print(key);  // Print the entered digit for feedback
        // lcd.setCursor(0, 1);  // Move cursor to the next line on LCD
        lcd.print(key);  // Display the entered digit on the LCD
      } else if (key == '#') {  // Use '#' as the "enter" key
        Serial.println();
        lcd.setCursor(0, 1);  // Move cursor to the next line on LCD
        lcd.print("Value Set");
        delay(1000);  // Delay to let the user see the interval confirmation
        lcd.clear();
        break;  // Exit the loop once the input is confirmed
      } else if (key == '*') {  // Use '*' as the "clear" key
        value = 0;  // Reset the value if '*' is pressed
        Serial.println("\nCleared input, re-enter:");
        lcd.clear();
        lcd.print("Cleared, re-enter:");
        lcd.setCursor(0, 1);
      }
    }
  }
  return value;  // Return the final integer value
}




// Helper function to get file name from keypad
String getFileName() {
  String filename = "";
  char key;
  Serial.print("Enter filename: ");
  lcd.clear();
  lcd.print("Enter filename: ");
  lcd.setCursor(1, 1);
  lcd.print("data_");
  lcd.setCursor(9, 1);
  lcd.print(".csv");  // Display ".csv" on the LCD
  lcd.setCursor(6, 1);
  
  while (true) {
    key = keypad.getKey();
    if (key) {
      if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'D')) { // Allow numbers and letters
        filename += key;  // Append the character to the filename
        Serial.print(key);  // Print each entered character
        lcd.print(key);  // Display each entered character on LCD
      } else if (key == '#') {  // Use '#' as the "enter" key
        Serial.println(".csv");
        filename += ".csv";  // Append ".csv" to complete the filename
        
        delay(1000);  // Short delay to let the user see the final filename
        lcd.clear();
        break;
      } else if (key == '*') {  // Use '*' as the "clear" key
        filename = "";  // Clear the filename
        Serial.println("\nCleared input, re-enter:");
        lcd.clear();
        lcd.print("Enter filename: ");
        lcd.setCursor(1, 1);
        lcd.print("data_");
        lcd.setCursor(9, 1);
        lcd.print(".csv");  // Display ".csv" on the LCD
        lcd.setCursor(6, 1);
        
      }
    }
  }
  return filename;
}

// Function to log data to SD card
void logData(File &dataFile, float windSpeed, float temperature, float humidity) {
  // dataFile.print("Wind Speed (m/s): ");
  dataFile.print(windSpeed);
  dataFile.print(", ");
  dataFile.print(temperature);
  dataFile.print(", ");
  dataFile.println(humidity);
  dataFile.flush();
}

// Function to display data on LCD and Serial Monitor
void displayData(float windSpeed, int temperature, int humidity) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("W(m/s) ");
  lcd.print("T(C) ");
  lcd.print("H(%)");

  lcd.setCursor(0, 1);
  lcd.print(windSpeed);
 
  lcd.setCursor(7, 1);
  lcd.print(temperature);
  
  lcd.setCursor(12, 1);
  lcd.print(humidity);
  


  Serial.print("Wind Speed: ");
  Serial.print(windSpeed);
  Serial.print(" m/s, Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.println(humidity);
}
