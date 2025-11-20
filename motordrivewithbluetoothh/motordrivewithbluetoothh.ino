#include <SoftwareSerial.h>
/*Due to limited pins, shift register is used
direction for motor signal is packaged in an 8bit payload
For payload structure, refer document in repository*/

// HC-05 Bluetooth module on pins 10 (RX) and 11 (TX)
// RX must be a supported pin on Leonardo (10, 11, etc.)
SoftwareSerial BT(10, 11); // RX, TX  (HC-05 TX → 10, RX → 11)


// L293D Motor Driver enable pins (PWM)
const int traction_left_enablePin = 9; 
const int traction_right_enablePin = 6; 
const int roll_enablePin = 5; 
const int hydration_enablePin = 3;

// 74HC595 Shift Register Pin
const int dataPin = 2;
const int latchPin = 4;
const int clockPin = 7;

// State variables
bool rollerOn = false;

// payload
uint8_t payload = 0b00000000;

// bit mask for each motor
#define TRACTION_LEFT_MASK  0b00000011
#define TRACTION_RIGHT_MASK 0b00001100
#define ROLLER_MASK         0b00110000
#define HYDRATION_MASK      0b11000000

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(traction_left_enablePin, OUTPUT);
  pinMode(traction_right_enablePin, OUTPUT);
  pinMode(roll_enablePin, OUTPUT);
  pinMode(hydration_enablePin, OUTPUT);

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
    updateShiftRegister();
  }

  // (Optional) — also allow control via USB Serial
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    Serial.print("USB Received: ");
    Serial.println(cmd);
    handleCommand(cmd);
    updateShiftRegister();
    for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(payload, i));
    }
    Serial.println();
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

void updateShiftRegister(){
  //set latch pin low before shifting for smooth update
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, payload);
  //LOW->HIGH move payload from shift register to storage register
  digitalWrite(latchPin, HIGH);
}

void updateRollerState(){
  if (rollerOn == true){
    rollerOn = false;
  }else{
    rollerOn = true;
  }
}

void startRoller(int speed){
  //if roller state is ON, turn on roller
  if (rollerOn == true){
    //isolate and clear the roller bits
    payload &= ~ROLLER_MASK;

    //roller rotate forward;
    payload |= 0b00010000;

    analogWrite(roll_enablePin, speed);
    
    // turn on hydration pump
    // isolate and clear hydration pump bits
    payload &= ~HYDRATION_MASK;

    // hydration pump rotate forward
    payload |= 0b01000000;

    analogWrite(hydration_enablePin, speed);
  }else{
    //isolate and clear the roller bits
    payload &= ~ROLLER_MASK;

    //disable roller
    payload |= 0b00000000;
    analogWrite(roll_enablePin, 0);

    // isolate and clear hydration pump bits
    payload &= ~HYDRATION_MASK;

    //disable hydration pump
    payload |= 0b00000000;
    analogWrite(hydration_enablePin, 0);
  }
}

void forward(int speed) {
  //isolate the traction motor bits and clear it while keeping other bits untouched
  payload &= ~TRACTION_LEFT_MASK;
  payload &= ~TRACTION_RIGHT_MASK;

  //left traction motor move forward
  payload |= 0b00000001;
  //right traction motor move forward
  payload |= 0b00000100;
  analogWrite(traction_left_enablePin, speed);
  analogWrite(traction_right_enablePin, speed);
  Serial.println("Motor forward");
}

void reverse(int speed) {
  //isolate the traction motor bits and clear it while keeping other bits untouched
  payload &= ~TRACTION_LEFT_MASK;
  payload &= ~TRACTION_RIGHT_MASK;

  //left traction motor move backward
  payload |= 0b00000010;
  //right traction motor move backward
  payload |= 0b00001000;

  analogWrite(traction_left_enablePin, speed);
  analogWrite(traction_right_enablePin, speed);
  Serial.println("Motor reverse");
}

void stopMotor() {
  //clear bits for left and right traction motor while leaving others bits untouched
  payload &= ~TRACTION_LEFT_MASK;
  payload &= ~TRACTION_RIGHT_MASK;
  //stop left traction motor
  payload |= 0b00000000;
  //stop right traction motor
  payload |= 0b00000000;
  
  analogWrite(traction_left_enablePin, 0);
  analogWrite(traction_right_enablePin, 0);
  Serial.println("Motor stopped");
}
