// include stepper library
#include <AccelStepper.h>
#include <Stepper.h>

const int stepsPerRevolution = 400;                                      // Nema34 motor has 200 steps in 360 degrees rotation (HW34-506 motor)

AccelStepper stepper(1, 8, 9);                                           // Define a stepper and the pins it will use

int POS;                                                                 // position variable, holds most recent position reached
int newPOS;                                                              // stores most recent input position value (so you have your two most recent ones)
int flag;                                                                // used to run functions in the same order each time (may be redundant after latest edits)
int timer;

int rainflag;                                                            // 0 = no rain, 1 = raining (if raining, add 180 degrees to position, see lines 37 and 51)
int x;                                                                   // for counting (unused right now)
int total;                                                               // variable for the running total (unused right now, may be used with data acquisition later)

int inputPin = 5;
int countdata;                                                           // number of data points counted
int average;                                                             // average of data so far
int halfrev;                                                             // steps in half a revolution (turntable)

void setup()

{
  stepper.setMaxSpeed(30);                                               //self explanatory (in steps per second)
  stepper.setAcceleration(10);                                           //self explanatory
  Serial.begin(9600);                                                    //activates serial interface
  pinMode(10, OUTPUT);                                                   //set pin 10 as output (pin 10 connects to ENABLE+ pin on stepper driver, necessary to operate motor)
  digitalWrite(10, HIGH);                                                //set pin 10 to logic HIGH (to ENABLE your stepper driver)
}

void forward()                                                           // your function for moving the turntable forwards
{

  //a function that checks your rainflag, then moves to a position (position depends on rainflag) 
  //q for dan: make this flag robust?
  //if it rains for 9 out of 10 minutes and you check during the dry minute, the flag will tell you it wasnt raining for 10 minutes

  if (rainflag == 0)
  {
    Serial.print(", forward: good conditions, ");
    stepper.moveTo(POS);                                                 // move towards your position value
    while (stepper.currentPosition() != (POS - 40))                      // constant speed towards zero, until a certain number of steps from zero (here, 40)
    {
      stepper.run();                                                     // run until 40 steps away from zero
    }
    Serial.print("exit while loop");
    stepper.stop();                                                      // stop as fast as possible: sets new target
    stepper.runToPosition();
    Serial.print(", flag set to zero, ");
    flag = 0;                                                            // set flag to 0, promtping our "off" function, which waits for a new input before moving
  }
  if (rainflag == 1)
  {

    Serial.print(", forward: precipitating");                            // identical to earlier IF statement, just offset.  (position + 180 degrees, to face away from the rain)
    POS = POS + halfrev;                                                 // sets your position to position + 180 degrees.
    stepper.moveTo(POS);                                                 // move towards your position value
    while (stepper.currentPosition() != (POS - 40))                      // constant speed towards zero, until a certain number of steps from zero (here, 40)
    {
      stepper.run();                                                     // run until 40 steps away from zero
    }
    Serial.print("exit while loop");
    stepper.stop();                                                      // stop as fast as possible: sets new target
    stepper.runToPosition();
    Serial.print(", flag set to zero, ");
    flag = 0;                                                            // set flag to 0, promtping our "off" function, which waits for a new input before moving
  }
}
void backward()                                                          // your function for moving backwards, back to your zero position (this should run in every cycle except the initial one)

{
  if (POS != 0)                                                          // if position isnt zero, go to zero
  {
    stepper.moveTo(0);
    while (stepper.currentPosition() != (0 + 40))                        // loop which moves turntable at constant speed until a distance away from zero
    { //
      stepper.run();                                                     // run until 40 steps away, then leave loop.
    }
    stepper.stop();                                                      // continue running towards zero, but now decelerating smoothly
    stepper.runToPosition();                                             // reach zero position
  }
  flag = 1;                                                              // set flag to 1, prompting our forwards-moving function
}

void nothing()                                                           // your function for waiting for inputs

//until weather data acquisition method is finalized, serial inputs used to enter discrete positions from 0 to 400 (in steps, where 400 steps = 1 revolution). 800steps = 2rev, 1200steps = 3rev, etc.

{

  /*

    Serial.print(" enter newPOS, ");
                                                                         // prompt for new position
    while(Serial.available()==0) {}                                      // wait until value is entered
    newPOS=Serial.parseInt();                                            // save value as newPOS, our newest requested position
    while(Serial.available() > 0) {Serial.read();};                      //
    flag = 2;                                                            // set flag to 2, prompting our backwards-moving function, for returning to our zero position.
    Serial.print("flag set to 2");

  */
  int i;
  for (i = 0; i < 11; i++)                                               // just a placeholder until data acquisition is figured out
  {
    int dir = digitalRead(inputPin);
    total = total + dir;
    timer = millis() + 60000;
    while (millis() < timer);
  }
  average = total / i;
  newPOS = average;
  timer = millis() + 60000;
  while (millis() < timer);
}



void loop()

// main function. simple loop, references the functions defined above
// contains your 3 flags for determining if youre moving to a new position, reverting to your zero position, or waiting for inputs.

{
  if (flag == 2)
  {
    backward();
  }
  if (flag == 1)
  {
    POS = newPOS;                                                         // now that the turntable is moving forward, replace your old position value (just used by the backwards-moving function to go back to zero) with the newest requested position value.
    forward();
  }
  if (flag == 0)
  {
    nothing();
  }
}
