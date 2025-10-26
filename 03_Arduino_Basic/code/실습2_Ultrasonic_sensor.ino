int trig = 13; // trig 핀 연결
int echo = 12; // echo 핀 연결

void setup() {
  Serial.begin(9600);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
}

void loop() {
  long duration, distance;
  digitalWrite(trig, HIGH);  // trig에서 초음파 발생(echoPin도 HIGH)
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH); // echo에서 입력받은 시간
  distance = ((float)(340 * duration) / 10000) / 2; // 거리 계산
  Serial.print("distance:"); // 거리 출력
  Serial.print(distance);
  Serial.println(" cm");
  delay(100);
}
