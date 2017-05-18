#include "Servo.h"

/////////////////
// LauncherFix //
/////////////////

typedef struct {
  byte pin; // control pin
  byte dly; // delay between high and low signals
} LauncherFix;

LauncherFix launcherFix = {
  pin: 2,
  dly: 30 // TODO tweak delay
};

void launchFix() {
  digitalWrite(launcherFix.pin, HIGH);
  delay(launcherFix.dly);
  digitalWrite(launcherFix.pin, LOW);
}

/////////////////
// LauncherRot //
/////////////////

typedef struct {
  byte pin; // control pin
  byte dly; // delay between high and low signals
  byte positions[2]; // roational position array
} LauncherRot;

//Servo servo; // declare outside of struct because compiler can't handle it:
// "sorry, unimplemented: non-trivial designated initializers not supported"

LauncherRot launcherRot = {
  pin: 3,
  dly: 40, // TODO tweak delay
  positions: {40, 60} // TODO get real rotations
};

void launchRot(byte i) {
  //servo.write(launcherRot.positions[i]);
  // TODO delay if rotating?
  digitalWrite(launcherRot.pin, HIGH);
  delay(launcherRot.dly);
  digitalWrite(launcherRot.pin, LOW);
}

///////////////////
// Arduino stuff //
///////////////////

void setup() {
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleNoteOff(handleNoteOff);

  pinMode(LED_BUILTIN, OUTPUT);
  // init launchers
  pinMode(launcherFix.pin, OUTPUT);
  pinMode(launcherRot.pin, OUTPUT);
  //servo.attach(4);

  blink(3);
}

void loop() { usbMIDI.read(); } // Do not modify!

void handleNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BUILTIN, HIGH);
  // TODO channel
  // velocity is ignored
  switch (note) {
    // C1
    case 24: launchFix(); break;
    // D1
    case 26: launchRot(0); break;
    // E1
    case 28: launchRot(1); break;
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

