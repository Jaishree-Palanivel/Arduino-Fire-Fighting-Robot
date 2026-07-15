// L298N Driver Pin Config
#define MOTOR_LEFT_FWD 2      // IN1
#define MOTOR_LEFT_REV 3      // IN2
#define MOTOR_RIGHT_FWD 4     // IN3
#define MOTOR_RIGHT_REV 5     // IN4

// Pump & Servo Control
#define PUMP_RELAY_PIN 6  
#define SERVO_PIN 11

// Analog Sensor Mapping
#define FLAME_SENSOR_LEFT A0
#define FLAME_SENSOR_CENTER A1
#define FLAME_SENSOR_RIGHT A2

// Calibrated limits (lower value = stronger flame)
#define FIRE_THRESHOLD 400      // Start tracking fire below this reading
#define SAFE_STOP_THRESHOLD 150 // Halt and spray when fire is ~10cm close

void moveForward(void);
void turnLeft(void);
void turnRight(void);
void stopRobot(void);
void setServoAngle(int angle);
void extinguishFire(void);

void setup() {
  Serial.begin(9600);

  // Set motor pins as outputs
  pinMode(MOTOR_LEFT_FWD, OUTPUT);
  pinMode(MOTOR_LEFT_REV, OUTPUT);
  pinMode(MOTOR_RIGHT_FWD, OUTPUT);
  pinMode(MOTOR_RIGHT_REV, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);

  // CRITICAL: Pull relay high BEFORE setting to output 
  // This prevents the pump from clicking/spraying during boot-up
  digitalWrite(PUMP_RELAY_PIN, HIGH); 
  pinMode(PUMP_RELAY_PIN, OUTPUT);

  // Start with nozzle centered
  setServoAngle(90); 
}

void loop() {
  // Pull analog raw values from flame sensors
  int valLeft = analogRead(FLAME_SENSOR_LEFT);
  int valCenter = analogRead(FLAME_SENSOR_CENTER);
  int valRight = analogRead(FLAME_SENSOR_RIGHT);

  // Debugging output for serial plotter/monitor
  Serial.print("L: "); Serial.print(valLeft);
  Serial.print(" | C: "); Serial.print(valCenter);
  Serial.print(" | R: "); Serial.println(valRight);

  // Main navigation & fire control state machine
  if (valCenter < SAFE_STOP_THRESHOLD) {
    // Fire is directly in front and close enough to put out
    stopRobot();
    extinguishFire();
  }
  else if (valCenter < FIRE_THRESHOLD && valCenter < valLeft && valCenter < valRight) {
    // Center sees the strongest heat signature
    moveForward();
  } 
  else if (valLeft < FIRE_THRESHOLD && valLeft < valRight) {
    // Fire is to the left
    turnLeft();
  } 
  else if (valRight < FIRE_THRESHOLD && valRight < valLeft) {
    // Fire is to the right
    turnRight();
  } 
  else {
    // No fire detected, stay put
    stopRobot();
  }

  delay(100); // Main loop sample rate
}

// H-Bridge Control Logic (Ensures no illegal high/high states)
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

// Manual PWM Generator (Replaces heavy C++ Servo library)
void setServoAngle(int angle) {
  // Protection: Clip out-of-bound values to prevent servo motor grinding
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  // Standard servo math: 50Hz frequency (20ms total period)
  // Maps 0-180 degrees to 1000-2000 microsecond pulse widths
  long pulseWidth = 1000 + ((long)angle * 1000 / 180); 
  
  // Pulse train of 10 cycles (~200ms) gives the physical motor enough time to react
  for (int i = 0; i < 10; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    
    // Remaining period time (20ms - pulse high time)
    delayMicroseconds(20000 - pulseWidth); 
  }
}

// Extinguishing sequence (Relay activation & nozzle sweeping)
void extinguishFire(void) {
  // Safety delay before switching states to protect power rails
  stopRobot();
  delay(200);

  // Turn on pump (Active-LOW relay)
  digitalWrite(PUMP_RELAY_PIN, LOW); 
  delay(500); // Wait for water line to pressurize

  // Slow horizontal sweeps to evenly spread water
  for (int pos = 50; pos <= 130; pos += 5) {
    setServoAngle(pos);
    delay(20);
  }
  
  for (int pos = 130; pos >= 50; pos -= 5) {
    setServoAngle(pos);
    delay(20);
  }

  // Centering nozzle and turning pump back off
  setServoAngle(90);
  digitalWrite(PUMP_RELAY_PIN, HIGH); 
  delay(1000); // Brief cooldown delay
}
