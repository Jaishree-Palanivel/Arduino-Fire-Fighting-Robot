#include <Servo.h>

// --- Pin Definitions ---
// L298N Motor Driver Pins
const int motorLeftFwd = 2;   // IN1
const int motorLeftRev = 3;   // IN2
const int motorRightFwd = 4;  // IN3
const int motorRightRev = 5;  // IN4

// Water Pump Relay Pin
const int pumpRelayPin = 6; 

// Servo Motor SG90 Pin
const int servoPin = 11;

// Flame Sensor Analog Pins
const int flameSensorLeft = A0;
const int flameSensorCenter = A1;
const int flameSensorRight = A2;

// --- Threshold Values ---
// Lower analog values indicate higher fire intensity.
// Fire detected if sensor reading drops below this threshold.
const int fireThreshold = 400; 

// Safe stopping threshold (Very close/immediate proximity)
// Below this, the robot halts to avoid burning.
const int safeStopThreshold = 150; 

Servo nozzleServo;

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);

  // Configure Motor Pins as Outputs
  pinMode(motorLeftFwd, OUTPUT);
  pinMode(motorLeftRev, OUTPUT);
  pinMode(motorRightFwd, OUTPUT);
  pinMode(motorRightRev, OUTPUT);

  // Configure Relay Pin as Output
  pinMode(pumpRelayPin, OUTPUT);
  
  // Turn OFF pump initially (Relay active LOW or HIGH depends on your module)
  // Standard 5V relay module is active LOW, so HIGH keeps it OFF.
  digitalWrite(pumpRelayPin, HIGH); 

  // Initialize Servo
  nozzleServo.attach(servoPin);
  nozzleServo.write(90); // Center the nozzle pointing forward
}

void loop() {
  // Read analog values from flame sensors
  int valLeft = analogRead(flameSensorLeft);
  int valCenter = analogRead(flameSensorCenter);
  int valRight = analogRead(flameSensorRight);

  // Print values to Serial Monitor for adjustment
  Serial.print("L: "); Serial.print(valLeft);
  Serial.print(" | C: "); Serial.print(valCenter);
  Serial.print(" | R: "); Serial.println(valRight);

  // Check if center sensor is dangerously close to the fire
  if (valCenter < safeStopThreshold && valCenter > 0) {
    stopRobot();
    extinguishFire();
  }
  // No immediate threat close-by, check directions to navigate
  else if (valCenter < fireThreshold && valCenter < valLeft && valCenter < valRight) {
    // Fire is directly ahead but at a distance -> Move Forward
    moveForward();
  } 
  else if (valLeft < fireThreshold && valLeft < valRight) {
    // Fire detected on the left -> Turn Left
    turnLeft();
  } 
  else if (valRight < fireThreshold && valRight < valLeft) {
    // Fire detected on the right -> Turn Right
    turnRight();
  } 
  else {
    // No fire detected -> Standby / Stop
    stopRobot();
  }

  delay(100); // Small delay to prevent sensor fluctuations
}

// --- Navigation Functions ---
void moveForward() {
  digitalWrite(motorLeftFwd, HIGH);
  digitalWrite(motorLeftRev, LOW);
  digitalWrite(motorRightFwd, HIGH);
  digitalWrite(motorRightRev, LOW);
}

void turnLeft() {
  digitalWrite(motorLeftFwd, LOW);
  digitalWrite(motorLeftRev, HIGH);
  digitalWrite(motorRightFwd, HIGH);
  digitalWrite(motorRightRev, LOW);
}

void turnRight() {
  digitalWrite(motorLeftFwd, HIGH);
  digitalWrite(motorLeftRev, LOW);
  digitalWrite(motorRightFwd, LOW);
  digitalWrite(motorRightRev, HIGH);
}

void stopRobot() {
  digitalWrite(motorLeftFwd, LOW);
  digitalWrite(motorLeftRev, LOW);
  digitalWrite(motorRightFwd, LOW);
  digitalWrite(motorRightRev, LOW);
}

// --- Fire Suppression Function ---
void extinguishFire() {
  // Activate pump (Low triggers typical active-LOW relay modules)
  digitalWrite(pumpRelayPin, LOW); 
  delay(500); // Wait for water pressure to build up

  // Sweep the servo to spread the water over the fire area
  for (int pos = 50; pos <= 130; pos += 2) {
    nozzleServo.write(pos);
    delay(15);
  }
  for (int pos = 130; pos >= 50; pos -= 2) {
    nozzleServo.write(pos);
    delay(15);
  }

  // Return nozzle to center and turn off pump
  nozzleServo.write(90);
  digitalWrite(pumpRelayPin, HIGH); 
  delay(1000); // Wait a second before resuming loop
}
