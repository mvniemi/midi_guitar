#define NUM_STRINGS 4
#define NUM_FRETS 8
#define VELOCITY 100
#define noteON 145
#define noteOFF 129 
#define BASENOTE 28
char *notes[]  = {"F","F#","G","G#","A","A#","B","C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
int stringOffsets[] = {0,5,10,15,19,25};
int stringPins[] = {4,5,6,7};
int fretPins[] = {22,23,24,25,26,27,28,29};
int currentNote[NUM_STRINGS];

//LED BACKPACK

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();



//Interrupts/
static int pinA = 18; // Our first hardware interrupt pin is digital pin 2
static int pinB = 19; // Our second hardware interrupt pin is digital pin 3

volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile int encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile int oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent

//
void setup() {
   pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  attachInterrupt(digitalPinToInterrupt(pinA),PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(digitalPinToInterrupt(pinB),PinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  // put your setup code here, to run once:
  Serial.begin(115200);

  alpha4.begin(0x70);
  alpha4.clear();
  alpha4.writeDisplay();
  
//Initialize read pins
  for (int i=0; i< NUM_FRETS; i++){
    pinMode(fretPins[i], INPUT);
  }
  
// Initialize write pins?
  for (int i=0; i< NUM_STRINGS; i++){
    pinMode(stringPins[i], OUTPUT);
    digitalWrite(stringPins[i], LOW);
    currentNote[i]=0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  //Scan Strings: We will scan down the neck string my string and set
  // the note as the highest detected fret
  // Currently setting the note after each string instead of after all strings
  for (int i=0; i < NUM_STRINGS; i++){
    
    //Turn String High
    digitalWrite(stringPins[i], HIGH);
    //Set current note to zero
    int newNote=0;
    
    for (int k=0; k < NUM_FRETS; k++){
      if (digitalRead(fretPins[k]) == HIGH){
        newNote=k+1;
        }
    }

   //Turn String Low
   digitalWrite(stringPins[i], LOW);
   if (newNote != currentNote[i]){
      MIDImessage(noteOFF, (currentNote[i]+stringOffsets[i]+encoderPos), VELOCITY);
      MIDImessage(noteOFF, (currentNote[i]+stringOffsets[i]+encoderPos+4), VELOCITY);
      MIDImessage(noteOFF, (currentNote[i]+stringOffsets[i]+encoderPos+7), VELOCITY);
    if (newNote != 0){
        MIDImessage(noteON, (newNote+stringOffsets[i]+encoderPos), VELOCITY);
        MIDImessage(noteON, (newNote+stringOffsets[i]+encoderPos+4), VELOCITY);
        MIDImessage(noteON, (newNote+stringOffsets[i]+encoderPos+7), VELOCITY);
      }
      currentNote[i] = newNote;
   }
 // delay();
  }
  dispInt(encoderPos);
}


void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command 
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}




//INTERRUPT CALLS

void PinA(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos --; //decrement the encoder's position count
//    dispInt(encoderPos);
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinB(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}


void dispInt(int input){
    String pos = String(input);
    alpha4.writeDigitAscii(0, pos[0]);
    alpha4.writeDigitAscii(1, pos[1]);
    alpha4.writeDigitAscii(2, pos[2]);
    
    alpha4.writeDisplay();

}

