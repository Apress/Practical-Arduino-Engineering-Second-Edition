// This code blinks an LED at 100ms

const int LEDdelay = 100;  // delay time

void setup()
{
     pinMode(13, OUTPUT);  // makes pin 13 an output
}

void loop()
{
     digitalWrite(13, HIGH);   // this writes a high bit to pin 13
     delay(LEDdelay);          // delay 100ms
     digitalWrite(13, LOW);    // this writes a low bit to pin 13
     delay(LEDdelay);          // delay 100ms

} 
