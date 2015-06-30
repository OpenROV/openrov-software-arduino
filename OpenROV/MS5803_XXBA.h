
#ifndef __MS5803_XXBA_H_
#define __MS5803_XXBA_H_
#include <Arduino.h>
#include "Device.h"

class MS5803_XXBA : public Device {
  public:
    MS5803_XXBA():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};

#endif
