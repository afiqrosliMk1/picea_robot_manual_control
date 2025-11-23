// -----------------------------
// HALL SENSOR SETUP
// -----------------------------
const int hallPin = 2;          // Hall sensor input pin (interrupt pin on UNO)
const int pulsesPerRev = 4;     // Number of pulses per revolution

volatile int pulseCount = 0;
void count() {
  pulseCount++;                 // ISR increments pulse count
}

// -----------------------------
// MOTOR PWM SETUP
// -----------------------------
const int pwmPin = 9;           // must be a PWM pin on Arduino

// -----------------------------
// PID VARIABLES
// -----------------------------
float setpoint = 300;           // target RPM

float Kp = 1.2;
float Ki = 0.8;
float Kd = 0.05;

float integral = 0;
float lastError = 0;
unsigned long lastPID = 0;

// -----------------------------
// RPM MEASUREMENT
// -----------------------------
unsigned long lastRPMTime = 0;
float rpm = 0;

void setup() {
  Serial.begin(115200);

  pinMode(hallPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallPin), count, FALLING);

  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0);       // motor OFF initially
}

void loop() {

  // ----------------------------------
  // 1) CALCULATE RPM every 100 ms
  // ----------------------------------
  if (millis() - lastRPMTime >= 100) {
    int countCopy = pulseCount;
    pulseCount = 0;

    float revs = (float)countCopy / pulsesPerRev;   // revolutions per 0.1s
    rpm = (revs * 60.0) / 0.1;                      // convert to RPM

    lastRPMTime = millis();
  }

  // ----------------------------------
  // 2) PID CONTROL every 20 ms
  // ----------------------------------
  if (millis() - lastPID >= 20) {

    float error = setpoint - rpm;

    // PID terms
    float P = Kp * error;

    integral += error * 0.02;           // dt = 20 ms
    float I = Ki * integral;

    float derivative = (error - lastError) / 0.02;
    float D = Kd * derivative;

    float output = P + I + D;

    // Clamp output to valid PWM (0–255 on Arduino)
    output = constrain(output, 0, 255);

    analogWrite(pwmPin, output);

    lastError = error;
    lastPID = millis();

    // For Serial Plotter
    Serial.print("rpm:");
    Serial.print(rpm);
    Serial.print(", output:");
    Serial.println(output);
  }
}
