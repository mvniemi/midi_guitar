//Midi_Guitar, Guitorgan Style
//Michael Niemi, https://github.com/mvniemi/midi_guitar
#define NUM_STRINGS 4
#define NUM_FRETS 8
#define VELOCITY 100
#define noteON 145
#define noteOFF 129

//This is where you pick what note you want your lowest string to start at
#define BASENOTE 28

//Set up for standard tuning of fourths
int stringOffsets[] = {0,5,10,15,19,25};
//This is for my bass, add more pins for a 6 string
int stringPins[] = {4,5,6,7};
int fretPins[] = {22,23,24,25,26,27,28,29};
int currentNote[NUM_STRINGS];

//
void setup() {
   //Serial to send midi commands over
  Serial.begin(115200);

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
      MIDImessage(noteOFF, (currentNote[i]+stringOffsets[i]+BASENOTE), VELOCITY);

    if (newNote != 0){
        MIDImessage(noteON, (newNote+stringOffsets[i]+BASENOTE), VELOCITY);
    }
    currentNote[i] = newNote;
   }
 // delay();
  }
}

void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command 
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}



