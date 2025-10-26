#include <IRremote.h>

int Ir_pin = A0; // IR 리모컨 센서 핀
IRrecv irrecv(Ir_pin);
decode_results ir_results;
 
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); 
}
 
void loop() {
  if (irrecv.decode(&ir_results)) {
    Serial.println(ir_results.value, HEX);
    irrecv.resume(); 
    }
}
