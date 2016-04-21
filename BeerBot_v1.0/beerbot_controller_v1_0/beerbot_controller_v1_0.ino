#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define kLeftJoystickY A0
#define kLeftJoystickX A1
#define kRightJoystickY A2
#define kRightJoystickX A3

typedef enum {
  kMotorForward,
  kMotorReverse
} MotorDirection;

struct controlData  // Data from remote control
{
  uint16_t leftSpeed;
  MotorDirection leftDirection;
  
  uint16_t rightSpeed;
  MotorDirection rightDirection;
};
controlData roverControlData;

struct telemetryData  // Data from rover
{
  uint16_t dummy;
};
telemetryData roverTelemetry;

bool roverConnected = false;

void setup() {
  
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Make It So!");
  lcd.setCursor(0, 1);
  lcd.print("BeerBot 1.0");

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = 16;
  Mirf.config();

  delay(2025);
}

void loop() {
  unsigned long time = millis();
  
  roverControlData.leftSpeed = abs(int(analogRead(kLeftJoystickX)) - 511) / 2;
  roverControlData.leftDirection = (analogRead(kLeftJoystickX) < 511) ? kMotorReverse : kMotorForward;
  roverControlData.rightSpeed = abs(int(analogRead(kRightJoystickX)) - 511) / 2;
  roverControlData.rightDirection = (analogRead(kRightJoystickX) < 511) ? kMotorReverse : kMotorForward;
  
  Mirf.setTADDR((byte *)"serv1");
  Mirf.send((byte *)&roverControlData);
  
  while (Mirf.isSending()) {
  }

  while (!Mirf.dataReady()) {
    if ( ( millis() - time ) > 2025) {
      roverConnected = false;
      lcd.setCursor(0,0);
      lcd.print("   Waiting...   ");
      lcd.setCursor(0,1);
      lcd.print("                ");
      return;
      }
  }

  if(!roverConnected) {
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("                ");
  }

  roverConnected = true;
  
  Mirf.getData((byte *) &roverTelemetry);
  
  lcd.setCursor(0, 0);
  if(kMotorForward == roverControlData.leftDirection) {
    lcd.print(int(roverControlData.leftSpeed));
    lcd.print("    ");
  }
  else if(kMotorReverse == roverControlData.leftDirection) {
    lcd.print(-int(roverControlData.leftSpeed));
    lcd.print("    ");
  }
  else {
    lcd.print(int(roverControlData.leftSpeed));
    lcd.print(" !");
    lcd.print("    ");
  }
  
  lcd.setCursor(0, 1);
  if(kMotorForward == roverControlData.rightDirection) {
    lcd.print(int(roverControlData.rightSpeed));
    lcd.print("    ");
  }
  else if(kMotorReverse == roverControlData.rightDirection) {
    lcd.print(-int(roverControlData.rightSpeed));
    lcd.print("    ");
  }
  else {
    lcd.print(int(roverControlData.rightSpeed));
    lcd.print(" !");
    lcd.print("    ");
  }
}
