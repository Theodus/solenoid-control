#include "Servo.h"

/////////////////////////
// Launchers & Targets //
/////////////////////////

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

typedef struct {
  byte target = 0;
  unsigned int per_target_delay = 100; // TODO tweak delay
  Servo servo;
  byte rotations[2] = {20, 40};
} Targets;

Targets targets;

void aim(byte target) {
  if (targets.target != target) {
    targets.servo.write(targets.rotations[target]);
    delay(targets.per_target_delay * abs(target - targets.target));
    targets.target = target;
  }
}

void launch(Launcher l, byte target) {
  aim(target);
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
  targets.servo.attach(4);
  aim(0);

  blink(3);
}

void loop() { usbMIDI.read(); } // Do not modify!

void handleNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BUILTIN, HIGH);
  // TODO channel
  // velocity is ignored
  switch (note) {
    // C1
    case 24: launch(launcher24V, 0); break;
    // D1
    case 26: launch(launcher36V, 0); break;
    // E1
    case 28: launch(launcher24V, 1); break;
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

