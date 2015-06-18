#line 2 "OpenROV_UnitTests"
#include "../lib/ArduinoUnit/ArduinoUnit.h"
#include "../../OpenROV/Command.h"
#include "../../OpenROV/Command.cpp"
#define HAS_MS5803_XXBA (1)
#include "../../OpenROV/MS5803_XXBALib.h"
#include "../../OpenROV/MS5803_XXBALib.cpp"
#include <Arduino.h>

//https://github.com/OpenROV/openrov-software/issues/242
test(injected_command_is_the_next_command_read)
{
  Command cmd;
  cmd.reset();
  int argsToSend[] = {1,00}; //include number of parms as fist parm
  cmd.pushCommand("test1",argsToSend);
  cmd.get();
  assertEqual(true,cmd.cmp("test1"));
}

test(injected_command_cannot_be_read_twice)
{
  Command cmd;
  cmd.reset();
  int argsToSend[] = {1,00}; //include number of parms as fist parm
  cmd.pushCommand("test2",argsToSend);
  cmd.get();
  cmd.get();
  assertEqual(false,cmd.cmp("test2"));
}

test(injected_command_args_0_holds_number_of_arguments)
{
  Command cmd;
  cmd.reset();
  int argsToSend[] = {4,1,2,3,4}; //include number of parms as fist parm
  cmd.pushCommand("test4",argsToSend);
  cmd.get();
  assertEqual(true,cmd.cmp("test4"));
  assertEqual(4,cmd.args[0]);
}

test(injected_command_drops_next_command_to_be_executed_if_outofbuffer)
{
  Command cmd;
  cmd.reset();
  //since the tail represents last command executed, in the case of a buffer of 3
  //we can insert int to buffer [0], [1], but when we try inserting in the last
  //buffer location [2] it will give a buffer overrun because head cannot push in
  //to the same location as tail.  So effectively we hax MAX_COMMANDS - 1 bufferd
  //commands.
  for(int i=0;i<MAX_COMMANDS;i++){
    int argsToSend[] = {1,i}; //include number of parms as fist parm
    cmd.pushCommand("test3",argsToSend);
  }
  cmd.get();
  //zero command should have gotten dropped
  assertEqual(true,cmd.cmp("test3"));
  assertEqual(1,cmd.args[1]);
}

test(sending_up_to_max_commands_does_not_create_buffer_overrun){
  Command cmd;
  cmd.reset();
  //since the tail represents last command executed, in the case of a buffer of 3
  //we can insert int to buffer [0], [1], but when we try inserting in the last
  //buffer location [2] it will give a buffer overrun because head cannot push in
  //to the same location as tail.  So effectively we hax MAX_COMMANDS - 1 bufferd
  //commands.
  for(int i=0;i<MAX_COMMANDS-1;i++){
    int argsToSend[] = {1,i}; //include number of parms as fist parm
    cmd.pushCommand("test4",argsToSend);
  }
  cmd.get();
  //zero command should be unaffected
  assertEqual(true,cmd.cmp("test4"));
  assertEqual(0,cmd.args[1]);
}


test(increasing_raw_pressure_always_calculates_as_deeper_depth){
  unsigned int CalConstant[8];
  CalConstant[0]=45057;
  CalConstant[1]=33648;
  CalConstant[2]=31822;
  CalConstant[3]=20275;
  CalConstant[4]=19680;
  CalConstant[5]=27191;
  CalConstant[6]=26252;
  CalConstant[7]=26252;

  float temp=8669000; //D2
  float pressure = 4312000; //D1
  float adjustedPressure;
  float lastAdjustedPressure = -999999;
  long failures = 0;
  for(long D2=7805000;D2<8000000;D2++){
    adjustedPressure=TemperatureCorrectedPressure(pressure,D2,CalConstant);
    if (adjustedPressure>=lastAdjustedPressure){
      lastAdjustedPressure = adjustedPressure;
    } else {
      failures++;
    }
  }
  assertEqual(0,failures);
}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Test::run();
}
