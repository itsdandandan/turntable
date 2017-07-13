// include stepper library
#include <AccelStepper.h>
#include <Stepper.h>

const int stepsPerRevolution = 400;  // Nema17 motor has 200 steps in 360 degrees rotation

// Define a stepper and the pins it will use
AccelStepper stepper(1, 8, 9);

int POS;
int flag;
int newPOS;
int x;

void setup() 

{
  stepper.setMaxSpeed(30);
  stepper.setAcceleration(10);
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
}

void forward() // your function for moving forwards

{

  // call for position here - you revert to zero before going forwards, backwards dir will have no position initially
  Serial.print(", flag set to 1, ");
  stepper.moveTo(POS);
  while (stepper.currentPosition() != (POS-40)) // Full speed up to POS-40
  {
    stepper.run();
  }
  Serial.print("exit while loop");
  stepper.stop(); // Stop as fast as possible: sets new target
  stepper.runToPosition();                                          // theres an issue here, typically runs through entire function without doing anything, just prints at end
  Serial.print(", flag set to zero, ");
  flag = 0;    
}

void backward() // your function for moving backwards

{ 
  if (POS != 0)
  {   
  stepper.moveTo(0);
  while (stepper.currentPosition() != (0+40)) // Full speed back to 0
  {
    stepper.run();
  }
  stepper.stop(); // Stop as fast as possible: sets new target
  stepper.runToPosition(); 
  }
  flag = 1;  
}

void nothing()

{  
  Serial.print(" enter newPOS, ");
                                                                                  //i used serial inputs for position requests for the motor
  while(Serial.available()==0) {} // wait for input data                          //replace this section later, with the inputs from anemometer instead of serial inputs
  newPOS=Serial.parseInt();
  while(Serial.available() > 0) {Serial.read();};
  flag = 2;
  Serial.print("flag set to 2");
}


void loop()
  {
  if (flag == 2)  // your 3 flags for determining if youre moving to a new position, reverting to your zero position, or waiting for inputs.
  {
    backward();
  }
  if (flag == 1) 
  {
    POS = newPOS;
    forward();
  }
  if (flag == 0)
  {
    nothing();
  }
}

