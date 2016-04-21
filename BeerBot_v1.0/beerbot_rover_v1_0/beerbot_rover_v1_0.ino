#include <Wire.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

int TN1 = 23;
int TN2 = 22;
int ENA = 5;
int TN3 = 24;
int TN4 = 25;
int ENB = 4;
int HALL_1A = 27;
int HALL_2A = 26;
int HALL_1B = 29;
int HALL_2B = 28;

typedef enum {
  kMotorForward,
  kMotorReverse
} MotorDirection;

struct controlData  // Data from remote control
{
  uint16_t leftSpeed = 0;
  MotorDirection leftDirection;
  
  uint16_t rightSpeed = 0;
  MotorDirection rightDirection;
};
controlData roverControlData;

struct telemetryData  // Data from rover
{
  uint16_t dummy;
};
telemetryData roverTelemetry;

volatile uint16_t rotation_a = 0;
volatile uint16_t rotation_b = 0;

/*hall_a keeps track of hall effect sensor a's position*/
void hall_a() {
    if(digitalRead(HALL_2A) == HIGH)
        rotation_a++;
    else
        rotation_a--;
}

/*hall_b keeps track of hall effect sensor b's position*/
void hall_b() {
    if(digitalRead(HALL_2B) == HIGH)
        rotation_b++;
    else
        rotation_b--;
}

void setup() {
  // 24L01 initialization
  Mirf.cePin = 53;
  Mirf.csnPin = 48;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 16;
  Mirf.config();

  roverTelemetry.dummy = 1;

    pinMode(TN1, OUTPUT);
    pinMode(TN2, OUTPUT);
    pinMode(TN3, OUTPUT);
    pinMode(TN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    //pinMode(18, INPUT);
    //pinMode(2, INPUT);

    attachInterrupt(HALL_1A, hall_a, FALLING);
    attachInterrupt(HALL_1B, hall_b, FALLING);
}

void loop() {

    Receive();

    if(roverControlData.leftSpeed > 15 || roverControlData.rightSpeed > 15)
    {
      // NUCLEAR DRIVE ASSAULT MODULE GO! :)
      // left speed
      analogWrite(ENA, roverControlData.leftSpeed);
      //right speed
      analogWrite(ENB, roverControlData.rightSpeed);
      
      // left direction
      if(kMotorForward == roverControlData.leftDirection)
      {
        digitalWrite(TN1, HIGH);
        digitalWrite(TN2, LOW);
      }
      else
      {
        digitalWrite(TN1, LOW);
        digitalWrite(TN2, HIGH);
      }

      // right direction
      if(kMotorForward == roverControlData.rightDirection)
      {
        digitalWrite(TN3, HIGH);
        digitalWrite(TN4, LOW);
      }
      else
      {
        digitalWrite(TN3, LOW);
        digitalWrite(TN4, HIGH);
      }
      
    }
    else
    {
      // full stop
      digitalWrite(TN1, HIGH);
      digitalWrite(TN2, HIGH);
      digitalWrite(TN3, HIGH);
      digitalWrite(TN4, HIGH);
    }
}

void Receive()
{
  if (!Mirf.isSending() && Mirf.dataReady())
  {
    // Read data from the romote controller
    Mirf.getData((byte *) &roverControlData);

    // zzz move this out of the receive function?
    Mirf.setTADDR((byte *)"clie1");
    Mirf.send((byte *) &roverTelemetry);  // Send data back to the controller
  }
  
}
