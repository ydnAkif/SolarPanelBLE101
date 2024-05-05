#include <Wire.h>
#include <LiquidCrystalTr_I2C.h>
#include <CurieBLE.h>


// Define LCD parameters
int lcd_Columns = 16;  // Number of columns in display
int lcd_Rows = 2;      // Number of Rows in display

LiquidCrystal_I2C lcd(0x3f, lcd_Columns, lcd_Rows);  // I2C address, rows, columns

// Animation parameters
int speed_Adjust = 300;  // Speed of moving Text
int rest_Time = 400;     // Resting time of Text animation at the edges of display
int text_Len;            // Variable to save text length

// BLE peripheral and service
BLEPeripheral blePeripheral;
BLEService lcdService("19B10000-E8F2-537E-4F6C-D104768A1214");  // Custom UUID for LCD service

// BLE characteristic to hold the text
BLEStringCharacteristic textCharacteristic("19B10000-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 255);

void setup() {
  // Serial begin()
  Serial.begin(9600);
  // Initialize LCD with Wire.h
  Wire.begin();  // Initialize I2C communication
  lcd.begin();
  lcd.backlight();

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
        text_Len = receivedText.length();

        for (int j = 0; j < lcd_Rows; j++) {
          for (int i = 0; i < (lcd_Columns - text_Len + 1); i++) {
            lcd.clear();
            lcd.setCursor(i, j);
            lcd.print(receivedText);
            delay(speed_Adjust);
          }

          delay(rest_Time);

          for (int i = (lcd_Columns - text_Len); i > -1; i--) {
            lcd.clear();
            lcd.setCursor(i, j);
            lcd.print(receivedText);
            delay(speed_Adjust);
          }

          delay(rest_Time);
        }

        for (int j = lcd_Rows - 1; j >= 0; j--) {
          lcd.clear();
          lcd.setCursor(0, j);
          lcd.print(receivedText);
          delay(speed_Adjust);
        }
        
        delay(rest_Time);
      }
    }
  }
}
