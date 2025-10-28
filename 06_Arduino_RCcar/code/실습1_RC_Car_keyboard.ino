\
/*
 * RC Car (Serial Control)
 * - Serial commands (common set): F/B/L/R/S, +/- to tune speed
 * - Debug prints ONLY on STOP / LEFT / RIGHT
 */

// -------- Optional: enable sensor reads for debug prints --------
#define HAVE_SENSORS 0
#if HAVE_SENSORS
const int S_L = A5;  // Left sensor  (LOW on line)
const int S_C = A4;  // Center sensor (LOW on line)
const int S_R = A3;  // Right sensor  (LOW on line)
#endif

// ---------------- Pins ----------------
const int R_EN  = 5;   // Right Enable (PWM)
const int L_EN  = 6;   // Left  Enable (PWM)
const int R_IN1 = 8;   // Right IN1
const int R_IN2 = 9;   // Right IN2
const int L_IN3 = 10;  // Left  IN3
const int L_IN4 = 11;  // Left  IN4

// ---------------- Speeds (same defaults) ------------------
int L_SPD = 153;
int R_SPD = 153;

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
#if HAVE_SENSORS
  dbg("LEFT");
#endif
}

void right() {
  motorStop(R_IN1, R_IN2, R_EN);
  motorDrive(L_IN3, L_IN4, L_EN, true, L_SPD);
#if HAVE_SENSORS
  dbg("RIGHT");
#endif
}

void halt() {
  motorStop(R_IN1, R_IN2, R_EN);
  motorStop(L_IN3, L_IN4, L_EN);
#if HAVE_SENSORS
  dbg("STOP");
#endif
}

// --------------- Debug print helper -----------------------
#if HAVE_SENSORS
void dbg(const char* tag) {
  int L = digitalRead(S_L);
  int C = digitalRead(S_C);
  int R = digitalRead(S_R);
  Serial.print(tag);
  Serial.print(" L="); Serial.print(L);
  Serial.print(" C="); Serial.print(C);
  Serial.print(" R="); Serial.println(R);
}
#endif

// ---------------- Setup -----------------------------------
void setup() {
  pinMode(R_EN, OUTPUT); pinMode(L_EN, OUTPUT);
  pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
  pinMode(L_IN3, OUTPUT); pinMode(L_IN4, OUTPUT);

#if HAVE_SENSORS
  pinMode(S_L, INPUT); pinMode(S_C, INPUT); pinMode(S_R, INPUT);
#endif

  Serial.begin(9600);
  halt(); // ensure stop on boot
  Serial.println("READY"); // brief one-time banner
}

// ---------------- Loop ------------------------------------
void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    switch (c) {
      case 'F': case 'f': fwd();  break;
      case 'B': case 'b': back(); break;
      case 'L': case 'l': left(); break;   // prints L,C,R if sensors enabled
      case 'R': case 'r': right(); break;  // prints L,C,R if sensors enabled
      case 'S': case 's': halt();  break;  // prints L,C,R if sensors enabled

      case '+': // faster
        L_SPD = min(255, L_SPD + 10);
        R_SPD = min(255, R_SPD + 10);
        break;

      case '-': // slower
        L_SPD = max(0, L_SPD - 10);
        R_SPD = max(0, R_SPD - 10);
        break;

      default:
        Serial.println("ERR"); // unknown command
        break;
    }
  }
}
