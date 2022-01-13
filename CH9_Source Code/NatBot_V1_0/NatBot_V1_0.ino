#include <SPI.h>
#include <SD.h>
#include <ADXL362.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <TinyGPS.h>

// Adafruit Display
#define sclk 52
#define mosi 51
#define cs   31
#define rst  29
#define dc   28

// Color definitions
#define  BLACK           0x0000

int numCount = 0;

// Motor IO
const int fields = 8; // how many fields are there? right now 8
int motorPins[] = {42,44,40,46,25,12,26,13}; // Motor Pins
int index = 0;        // the current field being received
int values[fields];   // array holding values for all the fields

// Object Creation
TinyGPS gps;
ADXL362 accel;
Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, mosi, sclk, rst);

// SD Card Chip Select
int SDCS = 30;

// Ultrasonic Sensor Pin
int pingPin = 22;

// Accelerometer variables
int16_t XValue, YValue, ZValue, Temperature;

// GPS data
char LatData[50];     // data buffer for Latitude
char LongData[50];

//Latching Variables
char prevState; 
int prevAutoState;

// Data buffer for saving drive data
char driveData [50];

// TMP36 Variables
int tempPin = A0;
int sensorValue = 0;

void setup() {
  
  // Serial of Arduino, Serial of bluetooth, Fianlly Serial of GPS
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(4800);

  // CS Pins of SPI devices
  pinMode(SDCS, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(27, OUTPUT);

  // set Motor pinMode to output
  for(int i; i <= 7; i++)        
  {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW); 
  }
  
  // Check for Card availability
  if (!SD.begin(SDCS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  // Turn off SD Chip Select
  digitalWrite(SDCS, HIGH);
  
  Serial.println("card initialized.");

  Serial.println("The Format is: MotoADir,MotoASpe,MotorBDir,MotoBSpe\n");
}

void loop() {
  if( Serial1.available())
  {
    char ch = Serial1.read();

    if (ch == 'y' || prevState == 'y') // if Serial reads y
    {
      accel.begin(10);                   // Setup SPI protocol, issue device soft reset
      accel.beginMeasure();              // Switch ADXL362 to measure mode

      // read all three axis in burst to ensure all measurements correspond to same sample time
      accel.readXYZTData(XValue, YValue, ZValue, Temperature);  
      Serial.print("XVALUE=");
      Serial.print(XValue);   
      Serial.print("\tYVALUE=");
      Serial.print(YValue);  
      Serial.print("\tZVALUE=");
      Serial.print(ZValue);  
      Serial.print("\tTEMPERATURE=");
      Serial.println(Temperature);   
      delay(100);                // Arbitrary delay to make serial monitor easier to observe

      // Stop communication with accelerometer
      digitalWrite(27, HIGH);

      // Store data into SD card
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      SD.begin(SDCS);
      File accelFile = SD.open("Accel.txt", FILE_WRITE);

      // if the file is available, write to it:
      if (accelFile) {
         accelFile.print(XValue);
         accelFile.print(" , ");
         accelFile.print(YValue);
         accelFile.print(" , ");
         accelFile.println(ZValue);
         accelFile.close();
         digitalWrite(SDCS, HIGH);
         }
       // if the file isn't open, pop up an error:
       else {
         Serial.println("error opening datalog.txt");
       }
      
       prevState = ch;
    }
    else if (ch == 'a') // if Serial reads a
    {
      if (prevAutoState == 0)
      {
        prevAutoState = 1;
        // put your main code here, to run repeatedly:
        float duration, inches;

        pinMode(pingPin, OUTPUT);
        digitalWrite(pingPin, LOW);
        delayMicroseconds(2);
        digitalWrite(pingPin, HIGH);
        delayMicroseconds(2);
        digitalWrite(pingPin, LOW);

        pinMode(pingPin, HIGH);
        inches = duration / 74 / 2;

        if (inches >= 2)
        {
          Serial1.println("Object Near!!");
        }
      }
      else if (prevAutoState == 1) 
      {
        prevAutoState = 0;
      }
    }
    else if (ch == 'g' || prevState == 'g') // if Serial reads g
    {     
        if (Serial2.available() > 0) // now gps device is active
        {
          int c = Serial2.read();
          if(gps.encode(c))     // New valid sentence?
          {

            // Initialize Longitude and Latitude to floating point numbers
            float latitude, longitude;

            // Get longitude and latitude
            gps.f_get_position(&latitude,&longitude);

            Serial.print("Lat:   ");
            // Prints latitude with 5 decimal places to the Serle ial Monitor
            Serial.println(latitude,7);

            Serial.print("long: ");
            // Prints longitude with 5 decimal places to the Serial Monitor
            Serial.println(longitude,7);

            // Store data into SD card
            // open the file. note that only one file can be open at a time,
            // so you have to close this one before opening another.
            SD.begin(SDCS);
            File GPSFile = SD.open("GPS.txt", FILE_WRITE);

            // if the file is available, write to it:
            if (GPSFile) {
                GPSFile.print(latitude, 7);
                GPSFile.print(" , ");
                GPSFile.println(longitude,7);
                GPSFile.close();
                digitalWrite(SDCS, HIGH);
              }
             // if the file isn't open, pop up an error:
             else {
                Serial.println("error opening datalog.txt");
             }
        }
      }
      prevState = ch;
    }
    else if(ch >= '0' && ch <= '9') // If the value is a number 0 to 9
    {
      // add to the value array
      values[index] = (values[index] * 10) + (ch - '0');
    }
    else if (ch == ',') // if it is a comma
    {
      if(index < fields -1) // If index is less than 4 - 1...
        index++; // increment index
    }
    else
    {

      for(int i=0; i <= index; i++)
      {

        if (i == 0 && numCount == 0)
        {
          Serial.println("Motor A");
          Serial.println(values[i]); 
        }
        else if (i == 1)
        {
          Serial.println(values[i]);
        }
        if (i == 2)
        {
          Serial.println("Motor B");
          Serial.println(values[i]); 
        }
        else if (i == 3)
        {
          Serial.println(values[i]);
        }
        if (i == 4)
        {
          Serial.println("Motor C");
          Serial.println(values[i]); 
        }
        else if (i == 5)
        {
          Serial.println(values[i]);
        }
        if (i == 6)
        {
          Serial.println("Motor D");
          Serial.println(values[i]); 
        }
        else if (i == 7)
        {
          Serial.println(values[i]);
        }

        if (i == 0 || i == 2 || i == 4 || i == 6)  // If the index is equal to 0 or 2
        {
          digitalWrite(motorPins[i], values[i]); // Write to the digital pin 1 or 0 
          // depending on what is sent to the arduino.
        }


        if (i == 1 || i == 3 || i == 5 || i == 7) // If the index is equal to 1 or 3 
        {
          analogWrite(motorPins[i], values[i]);  // Write to the PWM pins a number between 
          // 0 and 255 or what the person has entered 
          // in the serial monitor. 
        }

        values[i] = 0; // set values equal to 0

        sprintf(driveData, "Motor A = ", values[0], "Motor B = ", values[2], "Motor C = ", values[4], "Motor D = ", values[6]);

        // Send temperature to the LCD
        sensorValue = analogRead(tempPin);

        display.begin();
        display.fillScreen(BLACK);
        display.setCursor(0,0);
        display.print(sensorValue);
        digitalWrite(cs, HIGH);
        
      }

      index = 0; 
      numCount = 0;
    }
    if (ch == 'x' || prevState == 'x') // if Serial reads x
    {
            SD.begin(SDCS);
            File driveFile = SD.open("Drive.txt", FILE_WRITE);

            // if the file is available, write to it:
            if (driveFile) {
                driveFile.println(driveData);
                driveFile.close();
                digitalWrite(SDCS, HIGH);
              }
             // if the file isn't open, pop up an error:
             else {
                Serial.println("error opening datalog.txt");
             }
             prevState = ch;
    }
  }

}
