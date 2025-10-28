\
/*
 * RC Line-Following Car
 * - Debug prints ONLY on STOP / LEFT / RIGHT (with sensor values L,C,R)
 */

// ---------------- Pins ----------------
const int R_EN  = 5;   // Right Enable (PWM)
const int L_EN  = 6;   // Left  Enable (PWM)
const int R_IN1 = 8;   // Right IN1
const int R_IN2 = 9;   // Right IN2
const int L_IN3 = 10;  // Left  IN3
const int L_IN4 = 11;  // Left  IN4

const int S_L = A5;    // Left  sensor (LOW on line)
const int S_C = A4;    // Center sensor (LOW on line)
const int S_R = A3;    // Right sensor (LOW on line)

// ---------------- Speeds ------------------
int L_SPD = 153;
int R_SPD = 153;

// Previous sensor states
int pL = HIGH, pC = HIGH, pR = HIGH;

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
  motorStop(L_IN3, L_IN4, L_EN);
  motorDrive(R_IN1, R_IN2, R_EN, true, R_SPD);
}

void right() {
  motorStop(R_IN1, R_IN2, R_EN);
  motorDrive(L_IN3, L_IN4, L_EN, true, L_SPD);
}

void halt() {
  motorStop(R_IN1, R_IN2, R_EN);
  motorStop(L_IN3, L_IN4, L_EN);
}

// --------------- Debug print helper -----------------------
void dbg(const char* tag, int L, int C, int R) {
  Serial.print(tag);
  Serial.print(" L="); Serial.print(L);
  Serial.print(" C="); Serial.print(C);
  Serial.print(" R="); Serial.println(R);
}

// ---------------- Setup -----------------------------------
void setup() {
  pinMode(R_EN, OUTPUT); pinMode(L_EN, OUTPUT);
  pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
  pinMode(L_IN3, OUTPUT); pinMode(L_IN4, OUTPUT);

  pinMode(S_L, INPUT); pinMode(S_C, INPUT); pinMode(S_R, INPUT);
  Serial.begin(9600);
  halt(); // ensure stop on boot
}

// ---------------- Loop ------------------------------------
void loop() {
  // LOW = line, HIGH = background
  int L = digitalRead(S_L);
  int C = digitalRead(S_C);
  int R = digitalRead(S_R);

  // Decision
  if (L == LOW && R == LOW) {         // 0 0 0, 0 1 0
    fwd();
  }
  else if (L == LOW && R == HIGH) {   // 0 0 1, 0 1 1
    right();
    dbg("RIGHT", L, C, R);
  }
  else if (L == HIGH && R == LOW) {   // 1 0 0, 1 1 0
    left();
    dbg("LEFT", L, C, R);
  }
  else if (L == HIGH && R == HIGH) {  // 1 1 1, 1 0 1
    halt();
    dbg("STOP", L, C, R);
  }

  // Save previous states
  pL = L; pC = C; pR = R;
}
