// initialize the led and motor pins
int LEDPin = 13;
int motorPin = 5;

// keep track of the current state of the led and motor
int LEDState = 0;
int motorState = 0;

// store led and motor previous time value
unsigned long prevLEDTime = 0;
unsigned long prevMotorTime = 0;

// interval declaration can be changed if needed
const long LEDInterval = 5000;
const long motorInterval = 500;

void setup() {

  // setup motoe and led pins to be outputs
  pinMode(LEDPin, OUTPUT);
  pinMode(motorPin, OUTPUT);

}

void loop() {

  // get current time for the led and motor pin
  unsigned long curLEDTime = millis();
  unsigned long curMotorTime = millis();

  // this is a delta of the current time and previous time for the led circuit
  // this will also handle overflow
  if (curLEDTime - prevLEDTime >= LEDInterval)
  {
    // set the previous time to the current time to keep track of the led's state
    prevLEDTime = curLEDTime;

    // set the led state to high or low depending on what it's previous state was.
    if(LEDState == LOW)
    {
      LEDState = HIGH;
    }
    else
    {
      LEDState = LOW;
    }

    // turn on or off the led
    digitalWrite(LEDPin, LEDState);
  }

  // this is a delta of the current time and previous time for the motor circuit
  // this will also handle overflow
  if (curMotorTime - prevMotorTime >= motorInterval)
  {
    // set the previous time to the current time to keep track of the motor's state
    prevMotorTime = curMotorTime;

    // set the motor state to high or low depending on what it's previous state was.
    if(motorState == LOW)
    {
      motorState = HIGH;
    }
    else
    {
      motorState = LOW;
    }

    // turn on or off the motor
    digitalWrite(motorPin, motorState);
  }

}
