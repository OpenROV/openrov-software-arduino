
#ifndef __ALTSERVO_H_
#define __ALTSERVO_H_
#include "AConfig.h"
#if(HAS_ALT_SERVO)
#include <Arduino.h>
#include "Device.h"
#include "Pin.h"

#ifndef ALTS_MIDPOINT
#define ALTS_MIDPOINT 1500
#endif
#ifndef ALTS_MINPOINT
#define ALTS_MINPOINT 1000
#endif
#ifndef ALTS_MAXPOINT
#define ALTS_MAXPOINT 2000
#endif
#if(HAS_STD_CAPE)
  #include "Cape.h"
#endif

#if(HAS_OROV_CONTROLLERBOARD_25)
  #include "controllerboard25.h"
#endif

class AltServo : public Device {
  public:
    AltServo():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif
#endif
