// Motor + Ultrasonic example for ESP32 + L298N
// Pins (change if you like)
#define TRIG_PIN 4
#define ECHO_PIN 2

#define IN1 27
#define IN2 26
#define ENA 14   // PWM pin (must be attached with ledcAttachPin)

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 2000;    // 2 kHz PWM
const int PWM_RESOLUTION = 8; // 8-bit resolution -> 0..255

const float SOUND_SPEED_CM_PER_US = 0.034 / 2.0; // in cm per microsecond (duration/2 * 0.034)
const int DETECT_DISTANCE_CM = 30;   // detection threshold in cm
const int MOTOR_SPEED = 200;         // 0..255

void setup() {
  Serial.begin(115200);

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Setup PWM on ESP32
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL);

  stopMotor();
  delay(200);
  Serial.println("Ready.");
}

float getDistanceCM() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo (with timeout to avoid blocking forever)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL); // 30ms timeout (~5m)
  if (duration == 0) return -1.0; // no echo / out of range
  float distance = duration * SOUND_SPEED_CM_PER_US; // cm
  return distance;
}

void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(PWM_CHANNEL, constrain(speed, 0, 255));
}

void moveBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(PWM_CHANNEL, constrain(speed, 0, 255));
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  ledcWrite(PWM_CHANNEL, 0);
}

unsigned long lastPrint = 0;

void loop() {
  float dist = getDistanceCM();
  if (millis() - lastPrint > 250) { // print occasionally
    Serial.print("Distance: ");
    if (dist < 0) Serial.println("Out of range");
    else Serial.print(dist, 1), Serial.println(" cm");
    lastPrint = millis();
  }

  if (dist > 0 && dist < DETECT_DISTANCE_CM) {
    // object detected
    moveForward(MOTOR_SPEED);
  } else {
    stopMotor();
  }

  delay(80); // small loop delay
}
