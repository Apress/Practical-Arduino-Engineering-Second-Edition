#include <SdFat.h>
#include <SdFatUtil.h> 
#include <ctype.h>

#include <TinyGPS.h>

#include <NewSoftSerial.h>

TinyGPS gps;

NewSoftSerial nss(2,3);

void printFloat(double number, int digits); // function prototype for printFloat() function

int numCount = 0;

const int fields = 4; // how many fields are there? right now 4
int motorPins[] = {4,5,7,6}; // Motor Pins
int index = 0;        // the current field being received
int values[fields];   // array holding values for all the fields

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

char name[] = "GPSData.txt";     // holds the name of the new file
char LatData[50];                // data buffer for Latitude
char LongData[50]; 

void setup()
{
  Serial.begin(9600); // Initialize serial port to send and receive at 9600 baud
  nss.begin(4800);    // Begins software serial communication

  pinMode(10, OUTPUT);       // Pin 10 must be set as an output for the SD communication to 
                             // work.
  card.init();               // Initialize the SD card and configure the I/O pins.
  volume.init(card);         // Initialize a volume on the SD card.
  root.openRoot(volume);

  for (int i; i <= 3; i++)        // set LED pinMode to output
  {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW); 
  }

  Serial.println("The Format is: MotoADir,MotoASpe,MotorBDir,MotoBSpe\n");
}

void loop()
{

  if( Serial.available())
  {
    char ch = Serial.read();

    if (ch == 'G') // if Serial reads G
    {
      digitalWrite(motorPins[0], LOW);
      digitalWrite(motorPins[2], LOW);
      analogWrite(motorPins[1], 0);
      analogWrite(motorPins[3], 0);

      while (numCount == 0)
      {
        if (nss.available() > 0) // now gps device is active
        {
          int c = nss.read();
          if(gps.encode(c))     // New valid sentence?
          {

            // Initialize Longitude and Latitude to floating point numbers
            float latitude, longitude;

            // Get longitude and latitude
            gps.f_get_position(&latitude,&longitude);

            Serial.print("Lat:   ");
            // Prints latitude with 5 decimal places to the Serial Monitor
            Serial.println(latitude,7);

            Serial.print("long: ");
            // Prints longitude with 5 decimal places to the Serial Monitor
            Serial.println(longitude,7);

            file.open(root, name, O_CREAT | O_APPEND | O_WRITE);    // Open or create the file 
                                                                    // 'name'
                                                                    // in 'root' for writing 
                                                                    // to the
                                                                    // end of the file.
            file.print("Latitude: ");
            printFloat(latitude, 6);    
            file.println("");
            file.print("Longitude: ");
            printFloat(longitude, 6);
            file.println("");
            file.close();    // Close the file.
            delay(1000);     // Wait 1 second 

            numCount++;
          } 
        }
      }
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

        if (i == 0 || i == 2)  // If the index is equal to 0 or 2
        {
          digitalWrite(motorPins[i], values[i]); // Write to the digital pin 1 or 0 
          // depending on what is sent to the arduino.
        }


        if (i == 1 || i == 3) // If the index is equal to 1 or 3 
        {
          analogWrite(motorPins[i], values[i]);  // Write to the PWM pins a number between 
          // 0 and 255 or what the person has entered 
          // in the serial monitor. 
        }

        values[i] = 0; // set values equal to 0

      }

      index = 0; 
      numCount = 0;
    }

  }

}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
     file.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  file.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    file.print(".");

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    file.print(toPrint);
    remainder -= toPrint;
  }
}
