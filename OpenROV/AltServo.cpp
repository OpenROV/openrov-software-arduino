#include "AConfig.h"
#if(HAS_ALT_SERVO)
#include "Device.h"
#include "Pin.h"
#include "AltServo.h"
#include "Settings.h"
#include <Arduino.h>
#include "openrov_servo.h"
Servo _altservo;
int alts_val = ALTS_MIDPOINT;
int new_alts = ALTS_MIDPOINT;
int altsrate = 1;

int smoothAdjustedServo(int target, int current){
  double x = target - current;
  int sign = (x>0) - (x<0);
  int adjustedVal = current + sign * (min(abs(target - current), altsrate));
  return (adjustedVal);
}

void AltServo::device_setup(){
    _altservo.attach(ALTSERVO_PIN);
    _altservo.writeMicroseconds(ALTS_MIDPOINT);
    Settings::capability_bitarray |= (1 << ALT_SERVO_CAPABLE);

}

void AltServo::device_loop(Command command){
    if (command.cmp("asrt")) {
      int ms = map(command.args[1],-100,100,ALTS_MINPOINT,ALTS_MAXPOINT);
      if ((ms >= ALTS_MINPOINT) && (ms <= ALTS_MAXPOINT)){
        alts_val = ms;
        Serial.print("asr.t:");Serial.print(alts_val);Serial.println(";");
      }
    }
    if (alts_val != new_alts){
      new_alts = smoothAdjustedServo(alts_val,new_alts);
      _altservo.writeMicroseconds(new_alts);
      Serial.print("asr.v:");Serial.print(new_alts);Serial.println(";");
    }


}

#endif
