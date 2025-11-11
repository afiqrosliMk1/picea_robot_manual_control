#include <SoftwareSerial.h>

// HC-05 Bluetooth module on pins 10 (RX) and 11 (TX)
// RX must be a supported pin on Leonardo (10, 11, etc.)
SoftwareSerial BT(10, 11); // RX, TX  (HC-05 TX → 10, RX → 11)


// L293D Motor Driver pins
const int enablePin = 9;  // Enable pin (PWM)
const int in1Pin = 8;     // Direction pin 1
const int in2Pin = 7;     // Direction pin 2

void setup() {
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

  Serial.begin(9600); // USB debug
  BT.begin(9600);     // HC-05 default baud rate

  stopMotor();
  Serial.println("Bluetooth motor control ready");
}

void loop() {
  // Read from Bluetooth
  if (BT.available() > 0) {
    char cmd = BT.read();
    Serial.print("BT Received: ");
    Serial.println(cmd);
    handleCommand(cmd);
  }

  // (Optional) — also allow control via USB Serial
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    Serial.print("USB Received: ");
    Serial.println(cmd);
    handleCommand(cmd);
  }
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 'F':
      forward(200);
      break;
    case 'R':
      reverse(200);
      break;
    case 'S':
      stopMotor();
      break;
    default:
      stopMotor();
      break;
  }
}

void forward(int speed) {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  analogWrite(enablePin, speed);
  Serial.println("Motor forward");
}

void reverse(int speed) {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  analogWrite(enablePin, speed);
  Serial.println("Motor reverse");
}

void stopMotor() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  analogWrite(enablePin, 0);
  Serial.println("Motor stopped");
}
