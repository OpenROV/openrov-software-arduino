#include "AConfig.h"
#if(HAS_STD_LIGHTS)
#include "Device.h"
#include "Pin.h"
#include "DeadManSwitch.h"
#include "Settings.h"
#include <Arduino.h>
#include "Timer.h"

bool _deadmanSwitchEnabled = false;
bool blinkstate = false;
bool _deadmanSwitchArmed = false;
Timer deadmanSwitchTimer;
Timer blinklightTimer;
void DeadmanSwitch::device_setup(){
  Settings::capability_bitarray |= (1 << LIGHTS_CAPABLE);
  deadmanSwitchTimer.reset();
  blinklightTimer.reset();
}



void DeadmanSwitch::device_loop(Command command){

  //Auto arm the deadman switch after a reasonable boot time
  if(!_deadmanSwitchArmed && (millis() > DELAYTOARM)){
    _deadmanSwitchArmed = true;
  }

  if( command.cmp("ping")){
    deadmanSwitchTimer.reset();
    if (_deadmanSwitchEnabled){
      int argsToSend[] = {0};
      command.pushCommand("start",argsToSend);
      _deadmanSwitchEnabled = false;
    }
    Serial.print(F("pong:")); Serial.print(command.args[0]); Serial.print(","); Serial.print(millis()); Serial.print(";");
  }

  if ((deadmanSwitchTimer.elapsed (2000)) && _deadmanSwitchArmed && (_deadmanSwitchEnabled == false)) {
    int argsToSend[] = {0}; //include number of parms as fist parm
    command.pushCommand("deptloff",argsToSend);
    command.pushCommand("headloff",argsToSend);
    command.pushCommand("stop",argsToSend);
    _deadmanSwitchEnabled = true;

  }

  if (_deadmanSwitchEnabled && blinklightTimer.elapsed(500)){
    int argsToSend[] = {1,50};
    if (blinkstate){
      argsToSend[1] = 0;
    }
    command.pushCommand("ligt",argsToSend);
    blinkstate = !blinkstate;
  }

  if( command.cmp("dms")){
    if(command.args[0]==0){
      _deadmanSwitchArmed = false;
    } else {
      _deadmanSwitchArmed = true;
    }
  }

}
#endif
