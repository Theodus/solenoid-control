#include <Servo.h>

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
  dly: 30
};

Launcher launcher24V = {
  pin: 3,
  dly: 30
};

Servo servo;
const unsigned int per_target_delay = 120;
const byte angle_between_targets = 22;
const byte pivot = 3;
const byte launcher24V_offset = 2;
byte last_target = 2; // position of solenoid36V

// both solenoids must always be aiming at a target on completion
// all rotations are based on the position of the 36V launcher
void aim_and_launch(byte target) {
  byte pos36V = last_target;
  byte pos24V = last_target + launcher24V_offset;
  if (target == pos36V) { launch(launcher36V); }
  else if (target == pos24V) { launch(launcher24V); } 
  else if (target < pos36V) { aim_and_launch36V(target); }
  else if (target > pos24V) { aim_and_launch24V(target); }
  else {
    // target between the launchers (favor movements toward pivot)
    if (pos36V >= pivot) { aim_and_launch24V(target); }
    else { aim_and_launch36V(target); }      
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

// rotate launcher36V to target at given index and apply delay
void rotate_to(byte i) {
  servo.write(i * angle_between_targets);
  byte dist = abs(i - last_target);
  delay(per_target_delay * dist);
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
  //launch(launcher36V);
  //launch(launcher24V);

  blink(3);
}

void loop() { usbMIDI.read(6); } // Do not modify!

void handleNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BUILTIN, HIGH);
  // velocity is ignored
  switch (note) {
    case 47:  // B3
      aim_and_launch(7);
      break;
    case 49: // C#4
      aim_and_launch(6);
      break;
    case 50: // D4
      aim_and_launch(5);
      break;
    case 51: // D#4
      aim_and_launch(4);
      break;
    case 52: // E4
      aim_and_launch(3);
      break;
    case 55: // G4
      aim_and_launch(2);
      break;
    case 57: // A4
      aim_and_launch(1);
      break;
    case 59: // B4
      aim_and_launch(0);
      break;
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
 
