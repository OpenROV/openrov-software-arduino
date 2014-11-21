#line 2 "OpenROV_UnitTests"
#include <ArduinoUnit.h>
#include "../../OpenROV/Command.h"
#include "../../OpenROV/Command.cpp"

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

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Test::run();
}
