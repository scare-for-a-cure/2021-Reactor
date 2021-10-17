//Reactor Code 2021 season, made by Christopher James, 


//LIBRARIES
#include <RBD_Timer.h>  // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Button.h> // https://github.com/alextaujenis/RBD_Button

int DebugMode = 0;// 1= serial enabled, must be disabled for actual use otherwise outputs 1&2 will be stuck on.

//variables
//inputs
RBD::Button trigger(2); //a button to trigger end sequence 
//We are only gonna use one button apparently...
//RBD::Button reset = D3; //a button to reset 

//outputs
int fogMachine = 10;
int alertAudio = 9;       
int successAudio = 8;      
int exitDoor = 7;     
int alarmLight = 6;

//other
bool triggerSuccess; //since we are using one button, use this to flip states.

//Timers
RBD::Timer fogTimer(5000); //5s delay for Fog Machine
RBD::Timer doorTimer(500); //how long to engage the doorlocks (so we don't overheat it)
RBD::Timer waitTimer(2000); //delay time we need to wait before unlocking door.
RBD::Timer inputDelay(1000); //delay to limit how quickly an input can be read.

//Audio Timers
RBD::Timer successTimer(100);
RBD::Timer alertTimer(100);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////
///V- Sub Functions -V ///
//////////////////////////

//reset sequence
void resetSequence() { //reset the reactor into panic mode
  Serial.println("Resetting sequence...");
  digitalWrite(alertAudio, HIGH);
  alertTimer.restart();
  digitalWrite(alarmLight, HIGH);
  Serial.println("Reset complete.");
}//end resetSequence

//success sequence
void successSequence() { //reset the reactor into panic mode
  Serial.println("Stabilizing reactor...");
  digitalWrite(fogMachine,HIGH);
  fogTimer.restart();
  digitalWrite(successAudio, HIGH);
  successTimer.restart();
  digitalWrite(alarmLight, LOW);
  waitTimer.restart();
  Serial.println("Reactor stabilized. Meltdown averted.");
}//end resetSequence




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////
///V-Main Functions-V///
////////////////////////

void setup() {
  // put your setup code here, to run once:
  //init serial coms
  if(DebugMode){
    Serial.begin(9600); // serial print has to be disabled while running the system as outputs 1 and 2 are stuck as always on if serial print is enabled.
  }
  
  Serial.println("Begin Initialization");
  
  // init pin modes  
  pinMode(fogMachine, OUTPUT);
  pinMode(alertAudio, OUTPUT);
  pinMode(successAudio, OUTPUT);
  pinMode(exitDoor, OUTPUT);
  pinMode(alarmLight, OUTPUT);

  triggerSuccess = false;
  digitalWrite(fogMachine, LOW);
  digitalWrite(alertAudio, LOW);
  digitalWrite(successAudio, LOW);
  digitalWrite(exitDoor, LOW);
  digitalWrite(alarmLight, LOW);
  
  // init timers and stop them so they don't trigger yet
  fogTimer.onExpired();
  doorTimer.onExpired();
  waitTimer.onExpired();
  successTimer.onExpired();
  alertTimer.onExpired();

  Serial.println("Initialization Complete");
} //end setup


void loop() {
  // put your main code here, to run repeatedly:

  //runs the reset or success sequence when button is pressed.
  if (trigger.onPressed() && inputDelay.isExpired()) { 
    Serial.println("Trigger button pressed!");
    triggerSuccess = !triggerSuccess;
    if(triggerSuccess){
      successSequence();
    } else {
      resetSequence();
    }
    inputDelay.restart();
  }

/// INPUT BASED EVENTS GO ABOVE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TIMING BASED EVENTS GO BELOW

////////// all audio //////////
  if(fogTimer.onExpired()){
    Serial.println("Turning off fog machine.");
    digitalWrite(fogMachine, LOW);
  }//if end

  if(doorTimer.onExpired()){
    Serial.println("Dropping door lock voltage");
    digitalWrite(exitDoor, LOW);
  }

  if(waitTimer.onExpired()){
    Serial.println("Setting door lock high.");
    digitalWrite(exitDoor, HIGH);
    doorTimer.restart();
  }

  if(successTimer.onExpired()){
    Serial.println("Turning off success audio relay");
    digitalWrite(successAudio, LOW);
  }

  if(alertTimer.onExpired()){
    Serial.println("Turning off alarm audio relay");
    digitalWrite(alertAudio, LOW);
  }
} //end loop
