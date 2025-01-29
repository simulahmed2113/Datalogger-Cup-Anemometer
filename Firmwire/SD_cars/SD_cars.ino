#include <SD.h>
#include <SPI.h>

// Set CS pin for the SD card module
const int chipSelect = 53;

void setup() {
  // Start Serial Monitor for debugging
  Serial.begin(9600);

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Open or create a file
  File dataFile = SD.open("data.csv", FILE_WRITE);

  // Check if the file opened successfully
  if (dataFile) {
    // Write data in CSV format
    dataFile.println("Time, Temperature, Humidity");  // Header row
    dataFile.println("10:00, 25.3, 60.1");
    dataFile.println("10:05, 26.1, 58.4");

    // Close the file
    dataFile.close();
    Serial.println("Data written to data.csv");
  } else {
    Serial.println("Failed to open data.csv for writing.");
  }
}

void loop() {
  // Nothing to do here
}
