#include "Servo.h"

/////////////////////////
// Launchers & Targets //
/////////////////////////

// TODO ammunition aware?

typedef struct {
  byte pin; // control pin
  byte dly; // delay between high and low signals
} Launcher;

Launcher launcher36V = {
  pin: 2,
  dly: 30 // TODO tweak delay
};

Launcher launcher24V = {
  pin: 3,
  dly: 30 // TODO tweak delay
};

Servo servo;
const unsigned int per_target_delay = 100; // TODO tweak delay
const byte angle_between_targets = 13; // TODO verify
const byte pivot = 3; // TODO tweak pivot based on requirements
const byte launcher24V_offset = 2; // 3?
byte last_target = pivot; // position of solenoid36V

// [0] [1] [2] [3] [4] [5] [6] [7]

// both solenoids must always be aiming at a target on completion
// all rotations are based on the position of the 36V launcher
void aim(byte target) {
  //Serial.println(target);
  byte pos36V = last_target;
  byte pos24V = last_target + launcher24V_offset;
  if (target == pos36V) { launch(launcher36V); }
  else if (target == pos24V) { launch(launcher24V); } 
  else {
    if (target < pos36V) { aim_and_launch36V(target); }
    else if (target > pos24V) { aim_and_launch24V(target); }
    else {
      // between the launchers (favor movements toward pivot)
      if (pos36V >= pivot) { aim_and_launch24V(target); }
      else { aim_and_launch36V(target); }      
    }
  }
}

void aim_and_launch36V(byte i) {
  rotate_to(i);
  launch(launcher36V);
}

void aim_and_launch24V(byte i) {
  
  rotate_to(i - launcher24V_offset);
  launch(launcher24V);
}

// rotate launcher36V to target at given index
void rotate_to(byte i) {
  /*Serial.print(i);
  Serial.print("\t");
  Serial.print(i * angle_between_targets);
  Serial.println();*/
  servo.write(i * angle_between_targets);
  delay(per_target_delay * abs(i - last_target));
  last_target = i;
}

void launch(Launcher l) {
  digitalWrite(l.pin, HIGH);
  delay(l.dly);
  digitalWrite(l.pin, LOW);
}

///////////////////
// Arduino Stuff //
///////////////////

void setup() {
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleNoteOff(handleNoteOff);

  pinMode(LED_BUILTIN, OUTPUT);
  // init launchers
  pinMode(launcher36V.pin, OUTPUT);
  pinMode(launcher24V.pin, OUTPUT);
  // init targets
  servo.attach(4);
  rotate_to(last_target);

  //Serial.begin(9600);

  blink(3);
}

void loop() { usbMIDI.read(); } // Do not modify!

void handleNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BUILTIN, HIGH);
  // TODO channel
  // velocity is ignored
  switch (note) {
    case 24: aim(0); break; // C0
    case 26: aim(1); break; // D0
    case 28: aim(2); break; // E0
    case 29: aim(3); break; // F0
    case 31: aim(4); break; // G0
    case 33: aim(5); break; // A0
    case 35: aim(6); break; // B0
    case 36: aim(7); break; // C1
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BUILTIN, LOW);
  // Nothing done
}

//////////
// Util //
//////////

void blink(byte n) {
  for (byte i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}

