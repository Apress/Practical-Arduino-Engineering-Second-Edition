// initialize the led and motor pins
int LEDPin = 13;
int motorPin = 5;

// Keep track of Prev States of the LED and Motor
int prevLEDState = 0;
int prevMotorState = 0;

// Keep track of the current states of the LED and Motor
int currLEDState = 0;
int currMotorState = 0;

// store led and motor previous time value
unsigned long initLEDTime = 0;
unsigned long initMotorTime = 0;

// store led and motor previous time value
unsigned long currLEDTime = 0;
unsigned long currMotorTime = 0;

// interval declaration can be changed if needed
const long LEDInterval = 5000;
const long motorInterval = 1000;

void setup() {
  
  // Setup Serial port
  Serial.begin(9600);
  
  // setup motoe and led pins to be outputs
  pinMode(LEDPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  
}

void loop() {
  // Run State Machines
  LED_State_Machine();
  MOT_State_Machine();

}

void LED_State_Machine(){
  prevLEDState = currLEDState;

  // current state will always be preserved, so when in currLEDState = 2 the
  // switch statment will always go back to that state until set to a new state
  // in this case state 3.
  switch(currLEDState){
    case 0: // Init State
      currLEDState = 1;
    break;
    case 1: // Set State 1
      // Set initial time and Set the LED to High
      initLEDTime = millis();
      digitalWrite(LEDPin, HIGH);
      currLEDState = 2;
    break;
    case 2: // Update State 1
      // get current time and compare time to interval
      currLEDTime = millis();
      if (currLEDTime - initLEDTime > LEDInterval)
      {
        currLEDState = 3;
      }
    break;
    case 3: // Set State 2
      // Set initial time and Set the LED to Low
      initLEDTime = millis();
      digitalWrite(LEDPin, LOW);
      currLEDState = 4;
    break;
    case 4: // Update State 2
      // get current time and compare time to interval
      currLEDTime = millis();
      if (currLEDTime - initLEDTime > LEDInterval)
      {
        currLEDState = 0;
      }
    break;
    
  }
}

void MOT_State_Machine(){
  prevMotorState = currMotorState;

  // current state will always be preserved, so when in currMotorState = 2 the
  // switch statment will always go back to that state until set to a new state
  // in this case state 3.
  switch(currMotorState){
    case 0: // Init State
      currMotorState = 1;
    break;
    case 1: // Set State 1
      // Set initial time and Set the Motor to High
      initMotorTime = millis();
      digitalWrite(motorPin, HIGH);
      currMotorState = 2;
    break;
    case 2: // Update State 1
      // get current time and compare time to interval
      currMotorTime = millis();
      if (currMotorTime - initMotorTime > motorInterval)
      {
        currMotorState = 3;
      }
    break;
    case 3: // Set State 2
      // Set initial time and Set the Motor to Low
      initMotorTime = millis();
      digitalWrite(motorPin, LOW);
      currMotorState = 4;
    break;
    case 4: // Update State 2
      // get current time and compare time to interval
      currMotorTime = millis();
      if (currMotorTime - initMotorTime > motorInterval)
      {
        currMotorState = 0;
      }
    break;
    
  }
}
