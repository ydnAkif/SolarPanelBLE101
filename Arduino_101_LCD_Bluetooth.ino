

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <CurieBLE.h>

// Define LCD parameters
LiquidCrystal_I2C lcd(0x3f, 16, 2);  // I2C address, rows, columns

// BLE peripheral and service
BLEPeripheral blePeripheral;
BLEService lcdService("19B10000-E8F2-537E-4F6C-D104768A1214");  // Custom UUID for LCD service

// BLE characteristic to hold the text
BLEStringCharacteristic textCharacteristic("19B10000-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite,16);

void setup() {
  // Serial begin()
  Serial.begin(9600);
  // Initialize LCD with Wire.h
  Wire.begin();  // Initialize I2C communication
  lcd.begin();
  lcd.backlight();

  // LED pin (optional for debugging, comment out if not used)
  // pinMode(ledPin, OUTPUT);

  // Set BLE parameters
  blePeripheral.setLocalName("BLE101");
  blePeripheral.setAdvertisedServiceUuid(lcdService.uuid());

  // Add service and characteristic
  blePeripheral.addAttribute(lcdService);
  blePeripheral.addAttribute(textCharacteristic);

  // Begin advertising
  blePeripheral.begin();
}

void loop() {
  // Listen for BLE connections
  BLECentral central = blePeripheral.central();

  if (central) {
    while (central.connected()) {
      // Check if new text is received
      if (textCharacteristic.written()) {
        String receivedText = textCharacteristic.value();

        // Clear screen and display received text (truncate if too long)
        lcd.clear();
        lcd.print(receivedText.substring(0, 16));
        Serial.println(receivedText);
      }
    }
  }
}
