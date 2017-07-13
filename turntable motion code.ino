// include stepper library
#include <AccelStepper.h>
#include <Stepper.h>

const int stepsPerRevolution = 400;                                               // Nema34 motor has 200 steps in 360 degrees rotation (HW34-506 motor)

                                                                                  // Define a stepper and the pins it will use
AccelStepper stepper(1, 8, 9);

int POS;                                                                          // position variable, holds most recent position reached
int newPOS;                                                                       // 2nd position variable, holds the new requested position
int flag;                                                                         // a flag used to cycle between 3 running states - "forward-moving", "backwards-moving", and "off"
int x;                                                                            // for counting (unused right now)

void setup() 

{
  stepper.setMaxSpeed(30);                                                        //self explanatory (in steps per second)
  stepper.setAcceleration(10);                                                    //self explanatory
  Serial.begin(9600);                                                             //activates serial interface
  pinMode(10, OUTPUT);                                                            //set pin 10 as output (pin 10 connects to ENABLE+ pin on driver)
  digitalWrite(10, HIGH);                                                         //set pin 10 to logic HIGH (to enable driver)
}

void forward()                                                                    // your function for moving forwards

{
  Serial.print(", flag set to 1, ");
  stepper.moveTo(POS);                                                            // move towards your position value
  while (stepper.currentPosition() != (POS-40)) // Full speed up to POS-40        // constant speed towards zero, until a certain number of steps from zero (here, 40)
  {
    stepper.run();                                                                // run until 40 steps away from zero
  }
  Serial.print("exit while loop");                                                
  stepper.stop();                                                                 // stop as fast as possible: sets new target         
  stepper.runToPosition();                                                        // theres an issue here, typically runs through entire function without doing anything, just prints at end
  Serial.print(", flag set to zero, ");
  flag = 0;                                                                       // set flag to 0, promtping our "off" function, which waits for a new input before moving
}

void backward()                                                                   // your function for moving backwards (this should run in every cycle except the initial one)

{ 
  if (POS != 0)                                                                   // if position isnt zero, go to zero
  {   
  stepper.moveTo(0);
  while (stepper.currentPosition() != (0+40))                                     // loop which moves turntable at constant speed until a distance away from zero
  {                                                                               //
    stepper.run();                                                                // run until 40 steps away, then leave loop.
  }
  stepper.stop();                                                                 // continue running towards zero, but now decelerating smoothly
  stepper.runToPosition();                                                        // reach zero position
  }
  flag = 1;                                                                       // set flag to 1, prompting our forwards-moving function
}

void nothing()                                                                    // your function for waiting for inputs

//until weather data acquisition is accomplished, serial inputs used to enter discrete positions from 0 to 400 (in steps, where 400 steps = 1 revolution)

{  
  Serial.print(" enter newPOS, ");
                                                                                  // prompt for new position
  while(Serial.available()==0) {}                                                 // wait until value is entered                          
  newPOS=Serial.parseInt();                                                       // save value as newPOS, our newest requested position
  while(Serial.available() > 0) {Serial.read();};                                 //
  flag = 2;                                                                       // set flag to 2, prompting our backwards-moving function, for returning to our zero position.
  Serial.print("flag set to 2");                                                  //
}


void loop()   

// simple loop, references earlier functions
// contains your 3 flags for determining if youre moving to a new position, reverting to your zero position, or waiting for inputs.

  {
  if (flag == 2)  
  {
    backward();
  }
  if (flag == 1) 
  {
    POS = newPOS;                                                                  // now that the turntable is moving forward, replace your old position value (just used by the backwards-moving function) with the newest requested position value. 
    forward();
  }
  if (flag == 0)
  {
    nothing();
  }
}

