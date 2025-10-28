\
/*
 * Obstacle-Avoiding RC Car
 * - Debug prints on STOP/LEFT/RIGHT with distance (cm-ish ticks)
 *
 * Notes:
 *  - Ultrasonic returns a rough distance unit based on pulseIn.
 *  - Servo scan compares left(30°) vs right(150°) and turns to the side with more space.
 */

#include <Servo.h>

// ---------------- Servo ----------------
Servo SV;
const int SV_PIN = 2;
const int ANG_MID = 90;
const int ANG_LFT = 30;
const int ANG_RGT = 150;

// ---------------- Ultrasonic -------------
const int TRIG = 13;          // original trigPin
const int ECHO = 12;          // original echoPin

// ---------------- Motor pins -------------
const int R_EN  = 5;          // Right Enable (PWM)
const int L_EN  = 6;          // Left  Enable (PWM)
const int R_IN1 = 8;          // Right IN1
const int R_IN2 = 9;          // Right IN2
const int L_IN3 = 10;         // Left  IN3
const int L_IN4 = 11;         // Left  IN4

// ---------------- Speeds -----------------
int L_SPD = 153;
int R_SPD = 153;

// ---------------- State ------------------
int D = 0;                    // latest distance reading
int VAL = 0;                  // scan decision: 1=left, 0=right

// --------------- Motor helpers ----------------------------
void motorDrive(int in1, int in2, int en, bool fwd, int spd) {
  digitalWrite(in1, fwd ? HIGH : LOW);
  digitalWrite(in2, fwd ? LOW  : HIGH);
  analogWrite(en, constrain(spd, 0, 255));
}

void motorStop(int in1, int in2, int en) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(en, 0);
}

void motors(bool rFwd, bool lFwd, int rSpd, int lSpd) {
  motorDrive(R_IN1, R_IN2, R_EN, rFwd, rSpd);
  motorDrive(L_IN3, L_IN4, L_EN, lFwd, lSpd);
}

// --------------- Motions ----------------------------------
void fwd()  { motors(true,  true,  R_SPD, L_SPD); }
void back() { motors(false, false, R_SPD, L_SPD); }

void left() {
  // pivot left: stop left, run right forward
  motorStop(L_IN3, L_IN4, L_EN);
  motorDrive(R_IN1, R_IN2, R_EN, true, R_SPD);
  Serial.print("LEFT D="); Serial.println(D);
}

void right() {
  // pivot right: stop right, run left forward
  motorStop(R_IN1, R_IN2, R_EN);
  motorDrive(L_IN3, L_IN4, L_EN, true, L_SPD);
  Serial.print("RIGHT D="); Serial.println(D);
}

void halt() {
  motorStop(R_IN1, R_IN2, R_EN);
  motorStop(L_IN3, L_IN4, L_EN);
  Serial.print("STOP D="); Serial.println(D);
}

// --------------- Ultrasonic read --------------------------
int uRead() {
  long dur, dist;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  dur = pulseIn(ECHO, HIGH, 30000UL); // timeout 30ms
  if (dur == 0) return 999;
  dist = dur / 58;
  return (int)dist;
}

// --------------- Servo scan decision ----------------------
// Returns 1 for LEFT, 0 for RIGHT, based on which side is farther.
int scanDir() {
  SV.write(ANG_LFT);
  delay(300);
  int dL = uRead();
  delay(700);

  SV.write(ANG_RGT);
  delay(300);
  int dR = uRead();
  delay(700);

  // center back
  SV.write(ANG_MID);

  // Choose the side with more space
  if (dL > dR) {
    VAL = 1; // LEFT
  } else {
    VAL = 0; // RIGHT
  }

  // Debug: print both sides
  Serial.print("SCAN L="); Serial.print(dL);
  Serial.print(" R="); Serial.println(dR);

  return VAL;
}

// ---------------- Setup -----------------------------------
void setup() {
  SV.attach(SV_PIN);

  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);

  pinMode(R_EN, OUTPUT); pinMode(L_EN, OUTPUT);
  pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
  pinMode(L_IN3, OUTPUT); pinMode(L_IN4, OUTPUT);

  Serial.begin(9600);
  SV.write(ANG_MID);
  halt(); // ensure stop on boot
}

// ---------------- Loop ------------------------------------
void loop() {
  // Read distance and start moving forward
  D = uRead();
  fwd();

  // Obstacle thresholds
  if (D < 250) {
    if (D < 150) {
      // Too close: back off, short stop
      back();
      delay(1000);
      halt();
      delay(200);
    } else {
      // Close but not critical: stop, then decide left/right via scan
      halt();
      delay(200);

      // If distance is still small, perform a short back-off first
      if (D < 200) {
        back();
        delay(500);
        halt();
        delay(200);
      }

      int dir = scanDir(); // 1=LEFT, 0=RIGHT

      if (dir == 1) {
        // LEFT path: brief back, then pivot left
        back();
        delay(500);
        left();
        delay(800);
      } else {
        // RIGHT path: brief back, then pivot right
        back();
        delay(500);
        right();
        delay(800);
      }
    }
  }
}

// ---------------- Back-compat motor_role -------------------
void motor_role(int R_motor, int L_motor){
  digitalWrite(R_IN1, R_motor);
  digitalWrite(R_IN2, !R_motor);
  digitalWrite(L_IN3, L_motor);
  digitalWrite(L_IN4, !L_motor);
  analogWrite(R_EN, R_SPD);
  analogWrite(L_EN, L_SPD);
}
