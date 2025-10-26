void setup() {
  Serial.begin(9600);
}
void loop() { // 핀 설정
  int sensor_1 = analogRead(A3);
  int sensor_2 = analogRead(A4);
  int sensor_3 = analogRead(A5);
  
  Serial.print("1번 센서:");
  Serial.print(sensor_1);
  Serial.print("2번 센서:");
  Serial.print(sensor_2);
  Serial.print("3번 센서:");
  Serial.println(sensor_3);
  delay(100);
}
