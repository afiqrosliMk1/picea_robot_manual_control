#include <SoftwareSerial.h>

// HC-05 Bluetooth module on pins 10 (RX) and 11 (TX)
// RX must be a supported pin on Leonardo (10, 11, etc.)
SoftwareSerial BT(10, 11); // RX, TX  (HC-05 TX → 10, RX → 11)


// L293D Motor Driver pins
const int enablePin = 9;  // Enable pin (PWM)
const int in1Pin = 8;     // Direction pin 1
const int in2Pin = 7;     // Direction pin 2
const int roll_enablePin = 6; 
const int roll_dir1Pin = 5;
const int roll_dir2Pin = 4;
const int hydration_enablePin = 3;
const int hydration_dir1Pin = 2;
const int hydration_dir2Pin = 12;

// State variables
bool rollerOn = false;

void setup() {
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

  pinMode(roll_enablePin, OUTPUT);
  pinMode(roll_dir1Pin, OUTPUT);
  pinMode(roll_dir2Pin, OUTPUT);

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
    case 'B':
      reverse(200);
      break;
    case 'S':
      stopMotor();
      break;
    case 'R':
      updateRollerState();
      startRoller(200);
      break;
    default:
      stopMotor();
      break;
  }
}

void updateRollerState(){
  if (rollerOn == true){
    rollerOn = false;
  }else{
    rollerOn = true;
  }
}

void startRoller(int speed){
  if (rollerOn == true){
    //if roller state is ON, turn on roller
    digitalWrite(roll_dir1Pin, HIGH);
    digitalWrite(roll_dir2Pin, LOW);
    analogWrite(roll_enablePin, speed);
    // turn on hydration pump
    digitalWrite(hydration_dir1Pin, HIGH);
    digitalWrite(hydration_dir2Pin, LOW);
    analogWrite(hydration_enablePin, speed);
  }else{
    //disable roller
    digitalWrite(roll_dir1Pin, LOW);
    digitalWrite(roll_dir2Pin, LOW);
    analogWrite(roll_enablePin, 0);
    //disable hydration pump
    digitalWrite(hydration_dir1Pin, LOW);
    digitalWrite(hydration_dir2Pin, LOW);
    analogWrite(hydration_enablePin, 0);
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
