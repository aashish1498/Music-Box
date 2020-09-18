#include <MIDI.h>  // Add Midi Library
#include <SoftwareSerial.h>
#include "LedControl.h"
#include <Servo.h>
Servo myservo; // Create servo object to control a servo

#define LED 13    // Arduino Board LED is on Pin 13
int buzzer = 8;
//SoftwareSerial mySerial(2, 3);
byte midiByte;
int actual;
int multiplier;
double pos;

byte latestNote;

const int numberOfNotesToOpen = 8;
const int numberOfNotesToClose = 4;
const int numberOfNotesToSwitch = 6;

double requiredNotesOpen[numberOfNotesToOpen] = {67, 66, 63, 57, 56, 64, 68, 72}; //Required notes to open
double requiredNotesClose[numberOfNotesToClose] = {72, 67, 64, 60}; //Required notes to close
double switchStateNotes[numberOfNotesToSwitch] = {84, 83, 84, 83, 84, 83}; // Notes to switch state from open to closed (in case the initial state is incorrect)

int state = 0; //Assume initially open
int count = 0;
int switchCount = 0;
int countclose = 0;
int servoPos = 180; //Variable to store servo position

//Create an instance of the library with default name, serial port and settings
MIDI_CREATE_DEFAULT_INSTANCE();

double freq[12] = {65.406, 69.296, 73.416, 77.82, 82.407, 87.307, 92.499, 97.999, 103.83, 110, 116.54, 123.47};

void setup() {
  pinMode (LED, OUTPUT); // Set Arduino board pin 13 to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This is important!! This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This command tells the Midi Library
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.
  delay(100);
  pinMode(buzzer, OUTPUT);
  //myservo.attach(5);
}

void loop() { // Main loop
  MIDI.read(); // Continuously check if Midi data has been received.
}

void MyHandleNoteOn(byte channel, byte pitch, byte velocity) {
  int pitchint = pitch;
  int velocityint = velocity;
  latestNote = pitchint;
  int letter = pitchint % 12;
  soundBuzzer(pitchint, letter);
  delay(10);

  if (pitchint == switchStateNotes[switchCount])
  {
    digitalWrite(LED, HIGH); //Turn LED on
    switchCount = switchCount + 1;
    if (switchCount == numberOfNotesToSwitch)
    {
      if (state == 1)
      {
        state = 0;
      }
      else if (state == 0)
      {
        state = 1;
      }
    }
  }
  else if (state == 1) {
    switchCount = 0;
    if (pitchint == requiredNotesOpen[count])
    {
      digitalWrite(LED, HIGH); //Turn LED on
      count = count + 1;
      if (count == numberOfNotesToOpen)
      {
        myservo.attach(5);
        for (servoPos = 180; servoPos >= 90; servoPos -= 1) // goes from 180 degrees to 90 degrees (Anti Clockwise)
        {
          myservo.write(servoPos);// tell servo to go to position in variable 'pos'
          delay(15); // waits 15ms for the servo to reach the position
        }
        noTone(buzzer);
        myservo.detach();
        delay(1000);
        state = 0;
        count = 0;
      }
    }
    else
    {
      count = 0;
    }
  }
  else {
    switchCount = 0;
    if (pitchint == requiredNotesClose[countclose])
    {
      digitalWrite(LED, HIGH); //Turn LED on
      countclose = countclose + 1;
      if (countclose == numberOfNotesToClose) {
        myservo.attach(5);
        for (servoPos = 90; servoPos <= 180; servoPos += 1) // goes from 90 degrees to 180 degrees (Clockwise)
        {
          myservo.write(servoPos); // tell servo to go to position in variable 'pos'
          delay(15);// waits 15ms for the servo to reach the position

        }
        noTone(buzzer);
        delay(1000);
        myservo.detach();
        state = 1;
        countclose = 0;
      }
    }
    else {
      countclose = 0;
    }
  }
}

void MyHandleNoteOff(byte channel, byte pitch, byte velocity) {
  int pitchint = pitch;
  if (latestNote == pitchint) {
    noTone(buzzer);
  }
  digitalWrite(LED, LOW);
}

void soundBuzzer(int pitchint, int letter) {
  int multiplier = (pitchint / 12) - 3;
  int actual = bit(multiplier);
  int pos =  freq[letter] * actual;
  tone(buzzer, pos);
  delay(10);
}
