
#include "AConfig.h"
#if(HAS_STD_PILOT)
#include "Device.h"
#include "Pin.h"
#include "Pilot.h"
#include "Timer.h"

Timer pilotTimer;

bool _headingHoldEnabled = false;
int  _headingHoldTarget = 0;
int hdg = 0;
int hdg_Error;
int raw_Left, raw_Right;
int left, right;  // motor outputs in microseconds, +/-500
float heading_loop_Gain = 1.0;
float depth_hold_loop_gain = 0.6;
int integral_Divisor = 100;
long hdg_Error_Integral = 0;
int tgt_Hdg = 0;
bool _depthHoldEnabled = false;
int _depthHoldTarget = 0;
int depth = 0;
int depth_Error = 0;
int raw_lift =0;
int lift = 0;
int target_depth;
int raw_yaw, yaw;

int depth_deadband = 4; // +/- cm
int heading_deadband = 4;  // +/i degrees



void Pilot::device_setup(){
  pilotTimer.reset();

  Serial.println(F("log:pilot setup complete;"));
}



void Pilot::device_loop(Command command){
//intended to respond to fly by wire commands: MaintainHeading(); TurnTo(compassheading); DiveTo(depth);



    if( command.cmp("headloff")){
      _headingHoldEnabled = false;
      raw_Left = 0;
      raw_Right = 0;
      hdg_Error_Integral = 0;  // Reset error integrator
      tgt_Hdg = -500;  // -500 = system not in hdg hold

      int argsToSend[] = {1,00}; //include number of parms as fist parm
      command.pushCommand("yaw",argsToSend);
      Serial.println(F("log:heading_hold_disabled;"));
      Serial.print(F("targetHeading:"));Serial.print(DISABLED);Serial.println(';');

    }

    if( command.cmp("headlon")){
      _headingHoldEnabled = true;
      if(command.args[0]==0){
        _headingHoldTarget = navdata::HDGD;
      } else {
        _headingHoldTarget = command.args[1];
      }
      tgt_Hdg = _headingHoldTarget;
      Serial.print(F("log:heading_hold_enabled on="));Serial.print(tgt_Hdg);Serial.println(';');
      Serial.print(F("targetHeading:"));Serial.print(tgt_Hdg);Serial.println(';');
    }

    //Backwards compatibility for a release or two (2.5.1 release)
    if( command.cmp("holdHeading_toggle")){
      if (_headingHoldEnabled) {
        int argsToSend[] = {0}; //include number of parms as fist parm
        command.pushCommand("headloff",argsToSend);
      } else {
        int argsToSend[] = {1,command.args[0]}; //include number of parms as fist parm
        command.pushCommand("headloff",argsToSend);
      }
    }

    if( command.cmp("deptloff")){
      _depthHoldEnabled = false;
      raw_lift = 0;
      target_depth = 0;

      int argsToSend[] = {1,0}; //include number of parms as fist parm
      command.pushCommand("lift",argsToSend);
      Serial.println(F("log:depth_hold_disabled;"));
      Serial.print(F("targetDepth:"));Serial.print(DISABLED);Serial.println(';');

    }

    if( command.cmp("deptlon")){
      _depthHoldEnabled = true;
      if(command.args[0]==0){
        _depthHoldTarget = navdata::DEAP*100;  //casting to cm
      } else {
        _depthHoldTarget = command.args[1];
      }
      target_depth = _depthHoldTarget;
      Serial.print(F("log:depth_hold_enabled on="));
      Serial.print(target_depth);
      Serial.println(';');
      Serial.print(F("targetDepth:"));  Serial.print(target_depth);Serial.println(';');
    }


    if( command.cmp("holdDepth_toggle")){
      if (_depthHoldEnabled) {
        int argsToSend[] = {0}; //include number of parms as fist parm
        command.pushCommand("deptloff",argsToSend);
      } else {
        int argsToSend[] = {1,command.args[0]}; //include number of parms as fist parm
        command.pushCommand("deptloff",argsToSend);
      }
    }


    if (pilotTimer.elapsed (50)) {

      // Autopilot Test #3 6 Jan 2014
      // Hold vehicle at arbitrary heading
      // Integer math; proportional control plus basic integrator
      // No hysteresis around 180 degree error

      // Check whether hold mode is on

      if (_depthHoldEnabled)
      {
        depth = navdata::DEAP*100;
        depth_Error = target_depth-depth;  //positive error = positive lift = go deaper.

        raw_lift = (float)depth_Error * depth_hold_loop_gain;
        lift = constrain(raw_lift, -100, 100);

        Serial.println(F("log:dhold pushing command;"));
        Serial.print(F("dp_er:"));
        Serial.print(depth_Error);
        Serial.println(';');
        if (abs(depth_Error)>depth_deadband){
          int argsToSend[] = {1,lift}; //include number of parms as fist parm
          command.pushCommand("lift",argsToSend);
        } else {
          int argsToSend[] = {1,0}; //include number of parms as fist parm
          command.pushCommand("lift",argsToSend);
        }

      }

      if (_headingHoldEnabled)
      {

        // Code for hold mode here
        hdg = navdata::HDGD;

        // Calculate heading error

        hdg_Error = hdg - tgt_Hdg;

        if (hdg_Error > 180)
        {
        hdg_Error = hdg_Error - 360;
        }

        if (hdg_Error < -179)
        {
        hdg_Error = hdg_Error + 360;
        }

        // Run error accumulator (integrator)
        hdg_Error_Integral = hdg_Error_Integral + hdg_Error;

        // Calculator motor outputs
        raw_yaw = -1 * hdg_Error * heading_loop_Gain;

        // raw_Left = raw_Left - (hdg_Error_Integral / integral_Divisor);
        // raw_Right = raw_Right + (hdg_Error_Integral / integral_Divisor);

        // Constrain and output to motors

        yaw = constrain(raw_yaw, -50, 50);
        Serial.println(F("log:hold pushing command;"));
        Serial.print(F("p_er:"));
        Serial.print(hdg_Error);
        Serial.println(';');

        if (abs(hdg_Error) > heading_deadband){
          //start the motor with least power
          if (hdg_Error > 0) {
            hdg_Error -=heading_deadband;
          } else {
            hdg_Error +=heading_deadband;
          }

          int argsToSend[] = {1,yaw}; //include number of parms as fist parm
          command.pushCommand("yaw",argsToSend);
        } else {
          int argsToSend[] = {1,0}; //include number of parms as fist parm
          command.pushCommand("yaw",argsToSend);
        }
      }


    }
}
#endif
