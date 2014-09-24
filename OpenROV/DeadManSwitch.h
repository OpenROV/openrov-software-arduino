
#ifndef __DEADMANSWITCH_H_
#define __DEADMANSWITCH_H_
#include <Arduino.h>
#include "Device.h"
#include "Pin.h"
#include "AConfig.h"

#define DELAYTOARM 180000 //3 minutes


class DeadmanSwitch : public Device {
  public:
    DeadmanSwitch():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif
