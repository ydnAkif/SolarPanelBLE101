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
int text_Pos = 0;        // Variable to track text position

// BLE peripheral and service
BLEPeripheral blePeripheral;
BLEService lcdService("19B10000-E8F2-537E-4F6C-D104768A1214");  // Custom UUID for LCD service

// BLE characteristic to hold the text
BLEStringCharacteristic textCharacteristic("19B10000-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 255);

void setup() 
{
  // Serial begin()
  Serial.begin(9600);
  // Initialize LCD with Wire.h
  Wire.begin();  // Initialize I2C communication
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  // Set BLE parameters
  blePeripheral.setLocalName("BLE101");
  blePeripheral.setAdvertisedServiceUuid(lcdService.uuid());

  // Add service and characteristic
  blePeripheral.addAttribute(lcdService);
  blePeripheral.addAttribute(textCharacteristic);

  // Begin advertising
  blePeripheral.begin();
}

void loop() 
{
  // Listen for BLE connections
  BLECentral central = blePeripheral.central();

  if (central) 
  {
    while (central.connected()) 
    {
      // Check if new text is received
      if (textCharacteristic.written()) 
      {
        String receivedText = textCharacteristic.value();
        text_Len = receivedText.length();

        // Display text with animation
        while (true) 
        {
          for (int i = 0; i < lcd_Rows; i++) 
          {
            lcd.clear();
            lcd.setCursor(text_Pos, i);
            lcd.print(receivedText.substring(text_Pos, text_Pos + lcd_Columns));
            delay(speed_Adjust);

            text_Pos++;

            // Check if text has reached the end
            if (text_Pos >= text_Len) 
            {
              text_Pos = 0;
              delay(rest_Time);
            }
          }
        }
      }
    }
  }
}
