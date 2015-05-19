
#include "AConfig.h"
#if(HAS_BNO055)
#ifndef __ASBNO055_H_
#define __ASBNO055_H_
#include <Arduino.h>
#include "Device.h"

class AndreSharmonBNO055 : public Device {
  public:
    AndreSharmonBNO055():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif
#endif
