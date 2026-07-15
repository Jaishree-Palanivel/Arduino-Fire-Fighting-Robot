// --- Macro Definitions (Standard C Practice) ---
#define MOTOR_LEFT_FWD 2      // IN1
#define MOTOR_LEFT_REV 3      // IN2
#define MOTOR_RIGHT_FWD 4     // IN3
#define MOTOR_RIGHT_REV 5     // IN4

#define PUMP_RELAY_PIN 6 
#define SERVO_PIN 11

#define FLAME_SENSOR_LEFT A0
#define FLAME_SENSOR_CENTER A1
#define FLAME_SENSOR_RIGHT A2

#define FIRE_THRESHOLD 400 
#define SAFE_STOP_THRESHOLD 150 

// --- Function Prototypes (Strict C Standard) ---
void moveForward(void);
void turnLeft(void);
void turnRight(void);
void stopRobot(void);
void setServoAngle(int angle);
void extinguishFire(void);

void setup() {
  Serial.begin(9600);

  // Set Pin Modes
  pinMode(MOTOR_LEFT_FWD, OUTPUT);
  pinMode(MOTOR_LEFT_REV, OUTPUT);
  pinMode(MOTOR_RIGHT_FWD, OUTPUT);
  pinMode(MOTOR_RIGHT_REV, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);

  // Ensure pump is initially OFF (Active-LOW relay)
  digitalWrite(PUMP_RELAY_PIN, HIGH); 

  // Point nozzle forward (90 degrees)
  setServoAngle(90); 
}

void loop() {
  // Read analog inputs
  int valLeft = analogRead(FLAME_SENSOR_LEFT);
  int valCenter = analogRead(FLAME_SENSOR_CENTER);
  int valRight = analogRead(FLAME_SENSOR_RIGHT);

  // Monitor values
  Serial.print("L: "); Serial.print(valLeft);
  Serial.print(" | C: "); Serial.print(valCenter);
  Serial.print(" | R: "); Serial.println(valRight);

  // Decision control logic
  if (valCenter < SAFE_STOP_THRESHOLD && valCenter > 0) {
    stopRobot();
    extinguishFire();
  }
  else if (valCenter < FIRE_THRESHOLD && valCenter < valLeft && valCenter < valRight) {
    moveForward();
  } 
  else if (valLeft < FIRE_THRESHOLD && valLeft < valRight) {
    turnLeft();
  } 
  else if (valRight < FIRE_THRESHOLD && valRight < valLeft) {
    turnRight();
  } 
  else {
    stopRobot();
  }

  delay(100); 
}

// --- Navigation Actuation ---
void moveForward(void) {
  digitalWrite(MOTOR_LEFT_FWD, HIGH);
  digitalWrite(MOTOR_LEFT_REV, LOW);
  digitalWrite(MOTOR_RIGHT_FWD, HIGH);
  digitalWrite(MOTOR_RIGHT_REV, LOW);
}

void turnLeft(void) {
  digitalWrite(MOTOR_LEFT_FWD, LOW);
  digitalWrite(MOTOR_LEFT_REV, HIGH);
  digitalWrite(MOTOR_RIGHT_FWD, HIGH);
  digitalWrite(MOTOR_RIGHT_REV, LOW);
}

void turnRight(void) {
  digitalWrite(MOTOR_LEFT_FWD, HIGH);
  digitalWrite(MOTOR_LEFT_REV, LOW);
  digitalWrite(MOTOR_RIGHT_FWD, LOW);
  digitalWrite(MOTOR_RIGHT_REV, HIGH);
}

void stopRobot(void) {
  digitalWrite(MOTOR_LEFT_FWD, LOW);
  digitalWrite(MOTOR_LEFT_REV, LOW);
  digitalWrite(MOTOR_RIGHT_FWD, LOW);
  digitalWrite(MOTOR_RIGHT_REV, LOW);
}

/* 
 * Pure C implementation of Servo Motor Control
 * Servos operate on a 50Hz frequency (20ms period). 
 * Pulse width of 1ms (1000us) represents 0 degrees, and 2ms (2000us) represents 180 degrees.
 */
void setServoAngle(int angle) {
  // Map angle (0 to 180) to pulse width (1000 to 2000 microseconds)
  long pulseWidth = 1000 + ((long)angle * 1000 / 180); 
  
  // Send 10 continuous PWM pulses to give the physical servo motor time to reach the angle
  for (int i = 0; i < 10; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    
    // Complete the remainder of the 20ms period
    delayMicroseconds(20000 - pulseWidth); 
  }
}

// --- Suppression System ---
void extinguishFire(void) {
  // Turn on pump (Active-LOW)
  digitalWrite(PUMP_RELAY_PIN, LOW); 
  delay(500); // Allow pressure to build

  // Manual C Sweep: Left to Right
  for (int pos = 50; pos <= 130; pos += 5) {
    setServoAngle(pos);
    delay(20);
  }
  
  // Manual C Sweep: Right to Left
  for (int pos = 130; pos >= 50; pos -= 5) {
    setServoAngle(pos);
    delay(20);
  }

  // Reset nozzle and turn OFF pump
  setServoAngle(90);
  digitalWrite(PUMP_RELAY_PIN, HIGH); 
  delay(1000); 
}
