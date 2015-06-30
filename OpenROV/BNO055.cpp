#include "AConfig.h"
#if(HAS_BNO055)

#include "BNO055.h"
#include "Timer.h"


/* BNO055 OpenROV AHRS Module
 By Brian Adams
 Adopted from code by Kris Winer
 */
#include <Wire.h>

// BNO055 Register Map
// http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_10_Release.pdf
//
// BNO055 Page 0
#define BNO055_CHIP_ID          0x00    // should be 0xA0
#define BNO055_ACC_ID           0x01    // should be 0xFB
#define BNO055_MAG_ID           0x02    // should be 0x32
#define BNO055_GYRO_ID          0x03    // should be 0x0F
#define BNO055_SW_REV_ID_LSB    0x04
#define BNO055_SW_REV_ID_MSB    0x05
#define BNO055_BL_REV_ID        0x06
#define BNO055_PAGE_ID          0x07
#define BNO055_ACC_DATA_X_LSB   0x08
#define BNO055_ACC_DATA_X_MSB   0x09
#define BNO055_ACC_DATA_Y_LSB   0x0A
#define BNO055_ACC_DATA_Y_MSB   0x0B
#define BNO055_ACC_DATA_Z_LSB   0x0C
#define BNO055_ACC_DATA_Z_MSB   0x0D
#define BNO055_MAG_DATA_X_LSB   0x0E
#define BNO055_MAG_DATA_X_MSB   0x0F
#define BNO055_MAG_DATA_Y_LSB   0x10
#define BNO055_MAG_DATA_Y_MSB   0x11
#define BNO055_MAG_DATA_Z_LSB   0x12
#define BNO055_MAG_DATA_Z_MSB   0x13
#define BNO055_GYR_DATA_X_LSB   0x14
#define BNO055_GYR_DATA_X_MSB   0x15
#define BNO055_GYR_DATA_Y_LSB   0x16
#define BNO055_GYR_DATA_Y_MSB   0x17
#define BNO055_GYR_DATA_Z_LSB   0x18
#define BNO055_GYR_DATA_Z_MSB   0x19
#define BNO055_EUL_HEADING_LSB  0x1A
#define BNO055_EUL_HEADING_MSB  0x1B
#define BNO055_EUL_ROLL_LSB     0x1C
#define BNO055_EUL_ROLL_MSB     0x1D
#define BNO055_EUL_PITCH_LSB    0x1E
#define BNO055_EUL_PITCH_MSB    0x1F
#define BNO055_QUA_DATA_W_LSB   0x20
#define BNO055_QUA_DATA_W_MSB   0x21
#define BNO055_QUA_DATA_X_LSB   0x22
#define BNO055_QUA_DATA_X_MSB   0x23
#define BNO055_QUA_DATA_Y_LSB   0x24
#define BNO055_QUA_DATA_Y_MSB   0x25
#define BNO055_QUA_DATA_Z_LSB   0x26
#define BNO055_QUA_DATA_Z_MSB   0x27
#define BNO055_LIA_DATA_X_LSB   0x28
#define BNO055_LIA_DATA_X_MSB   0x29
#define BNO055_LIA_DATA_Y_LSB   0x2A
#define BNO055_LIA_DATA_Y_MSB   0x2B
#define BNO055_LIA_DATA_Z_LSB   0x2C
#define BNO055_LIA_DATA_Z_MSB   0x2D
#define BNO055_GRV_DATA_X_LSB   0x2E
#define BNO055_GRV_DATA_X_MSB   0x2F
#define BNO055_GRV_DATA_Y_LSB   0x30
#define BNO055_GRV_DATA_Y_MSB   0x31
#define BNO055_GRV_DATA_Z_LSB   0x32
#define BNO055_GRV_DATA_Z_MSB   0x33
#define BNO055_TEMP             0x34
#define BNO055_CALIB_STAT       0x35
#define BNO055_ST_RESULT        0x36
#define BNO055_INT_STATUS       0x37
#define BNO055_SYS_CLK_STATUS   0x38
#define BNO055_SYS_STATUS       0x39
#define BNO055_SYS_ERR          0x3A
#define BNO055_UNIT_SEL         0x3B
#define BNO055_OPR_MODE         0x3D
#define BNO055_PWR_MODE         0x3E
#define BNO055_SYS_TRIGGER      0x3F
#define BNO055_TEMP_SOURCE      0x40
#define BNO055_AXIS_MAP_CONFIG  0x41
#define BNO055_AXIS_MAP_SIGN    0x42
#define BNO055_ACC_OFFSET_X_LSB 0x55
#define BNO055_ACC_OFFSET_X_MSB 0x56
#define BNO055_ACC_OFFSET_Y_LSB 0x57
#define BNO055_ACC_OFFSET_Y_MSB 0x58
#define BNO055_ACC_OFFSET_Z_LSB 0x59
#define BNO055_ACC_OFFSET_Z_MSB 0x5A
#define BNO055_MAG_OFFSET_X_LSB 0x5B
#define BNO055_MAG_OFFSET_X_MSB 0x5C
#define BNO055_MAG_OFFSET_Y_LSB 0x5D
#define BNO055_MAG_OFFSET_Y_MSB 0x5E
#define BNO055_MAG_OFFSET_Z_LSB 0x5F
#define BNO055_MAG_OFFSET_Z_MSB 0x60
#define BNO055_GYR_OFFSET_X_LSB 0x61
#define BNO055_GYR_OFFSET_X_MSB 0x62
#define BNO055_GYR_OFFSET_Y_LSB 0x63
#define BNO055_GYR_OFFSET_Y_MSB 0x64
#define BNO055_GYR_OFFSET_Z_LSB 0x65
#define BNO055_GYR_OFFSET_Z_MSB 0x66
#define BNO055_ACC_RADIUS_LSB   0x67
#define BNO055_ACC_RADIUS_MSB   0x68
#define BNO055_MAG_RADIUS_LSB   0x69
#define BNO055_MAG_RADIUS_MSB   0x6A
//
// BNO055 Page 1
#define BNO055_PAGE_ID          0x07
#define BNO055_ACC_CONFIG       0x08
#define BNO055_MAG_CONFIG       0x09
#define BNO055_GYRO_CONFIG_0    0x0A
#define BNO055_GYRO_CONFIG_1    0x0B
#define BNO055_ACC_SLEEP_CONFIG 0x0C
#define BNO055_GYR_SLEEP_CONFIG 0x0D
#define BNO055_INT_MSK          0x0F
#define BNO055_INT_EN           0x10
#define BNO055_ACC_AM_THRES     0x11
#define BNO055_ACC_INT_SETTINGS 0x12
#define BNO055_ACC_HG_DURATION  0x13
#define BNO055_ACC_HG_THRESH    0x14
#define BNO055_ACC_NM_THRESH    0x15
#define BNO055_ACC_NM_SET       0x16
#define BNO055_GYR_INT_SETTINGS 0x17
#define BNO055_GYR_HR_X_SET     0x18
#define BNO055_GYR_DUR_X        0x19
#define BNO055_GYR_HR_Y_SET     0x1A
#define BNO055_GYR_DUR_Y        0x1B
#define BNO055_GYR_HR_Z_SET     0x1C
#define BNO055_GYR_DUR_Z        0x1D
#define BNO055_GYR_AM_THRESH    0x1E
#define BNO055_GYR_AM_SET       0x1F


//#define BNO055_ADDRESS 0x29
#define BNO055_ADDRESS 0x28

// Set initial input parameters
enum Ascale {  // ACC Full Scale
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_18G
};

enum Abw { // ACC Bandwidth
  ABW_7_81Hz = 0,
  ABW_15_63Hz,
  ABW_31_25Hz,
  ABW_62_5Hz,
  ABW_125Hz,
  ABW_250Hz,
  ABW_500Hz,
  ABW_1000Hz,    //0x07
};

enum APwrMode { // ACC Pwr Mode
  NormalA = 0,
  SuspendA,
  LowPower1A,
  StandbyA,
  LowPower2A,
  DeepSuspendA
};

enum Gscale {  // gyro full scale
  GFS_2000DPS = 0,
  GFS_1000DPS,
  GFS_500DPS,
  GFS_250DPS,
  GFS_125DPS    // 0x04
};

enum GPwrMode { // GYR Pwr Mode
  NormalG = 0,
  FastPowerUpG,
  DeepSuspendedG,
  SuspendG,
  AdvancedPowerSaveG
};

enum Gbw { // gyro bandwidth
  GBW_523Hz = 0,
  GBW_230Hz,
  GBW_116Hz,
  GBW_47Hz,
  GBW_23Hz,
  GBW_12Hz,
  GBW_64Hz,
  GBW_32Hz
};

enum OPRMode {  // BNO-55 operation modes
  CONFIGMODE = 0x00,
// Sensor Mode
  ACCONLY,
  MAGONLY,
  GYROONLY,
  ACCMAG,
  ACCGYRO,
  MAGGYRO,
  AMG,            // 0x07
// Fusion Mode
  IMU,
  COMPASS,
  M4G,
  NDOF_FMC_OFF,
  NDOF            // 0x0C
};

enum PWRMode {
  Normalpwr = 0,
  Lowpower,
  Suspendpwr
};

enum Modr {         // magnetometer output data rate
  MODR_2Hz = 0,
  MODR_6Hz,
  MODR_8Hz,
  MODR_10Hz,
  MODR_15Hz,
  MODR_20Hz,
  MODR_25Hz,
  MODR_30Hz
};

enum MOpMode { // MAG Op Mode
  LowPower = 0,
  Regular,
  EnhancedRegular,
  HighAccuracy
};

enum MPwrMode { // MAG power mode
  Normal = 0,
  Sleep,
  Suspend,
  ForceMode
};

static uint8_t PWRMode = Normal ;    // Select BNO055 power mode
static uint8_t OPRMode = NDOF;        // specify operation mode for sensors

static uint8_t status;               // BNO055 data status register
static int16_t EulCount[3];    // Stores the 16-bit signed Euler angle output
static float bPitch, bYaw, bRoll;
static float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
static float quat[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
static float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method



// I2C read/write functions for the BNO055 sensor

void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	Wire.beginTransmission(address);  // Initialize the Tx buffer
	Wire.write(subAddress);           // Put slave register address in Tx buffer
	Wire.write(data);                 // Put data in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}

uint8_t readByte(uint8_t address, uint8_t subAddress)
{
	uint8_t data; // `data` will store the register data
	Wire.beginTransmission(address);         // Initialize the Tx buffer
	Wire.write(subAddress);	                 // Put slave register address in Tx buffer
//	Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
	Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
	Wire.requestFrom((int)address, 1);  // Read one byte from slave register address
	Wire.requestFrom((int)address, (size_t) 1);   // Read one byte from slave register address
	data = Wire.read();                      // Fill Rx buffer with result
	return data;                             // Return data read from slave register
}

void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{
	Wire.beginTransmission(address);   // Initialize the Tx buffer
	Wire.write(subAddress);            // Put slave register address in Tx buffer
//	Wire.endTransmission(I2C_NOSTOP);  // Send the Tx buffer, but send a restart to keep connection alive
	Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
	uint8_t i = 0;
        Wire.requestFrom(address, count);  // Read bytes from slave register address
        Wire.requestFrom(address, (size_t) count);  // Read bytes from slave register address
        unsigned long timeout = 0;
	while ((i<count) && (timeout<10000)){
          timeout++;
          if (Wire.available()) {
            dest[i++] = Wire.read();
           }         // Put read results in the Rx buffer
        }
        if (i<count)
          Serial.println("DIAG:Timeout reading I2c;");
}



//===================================================================================================================
//====== Set of useful function to access acceleration. gyroscope, magnetometer, and temperature data
//===================================================================================================================

void readAccelData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z accel register data stored here
  readBytes(BNO055_ADDRESS, BNO055_ACC_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;      // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
}


void readGyroData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes(BNO055_ADDRESS, BNO055_GYR_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
}

int8_t readGyroTempData()
{
  return readByte(BNO055_ADDRESS, BNO055_TEMP);  // Read the two raw data registers sequentially into data array
}

void readMagData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes(BNO055_ADDRESS, BNO055_MAG_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
}

void readQuatData(int16_t * destination)
{
  uint8_t rawData[8];  // x/y/z gyro register data stored here
  readBytes(BNO055_ADDRESS, BNO055_QUA_DATA_W_LSB, 8, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
  destination[3] = ((int16_t)rawData[7] << 8) | rawData[6] ;
}

void readEulData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes(BNO055_ADDRESS, BNO055_EUL_HEADING_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
}

void readLIAData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes(BNO055_ADDRESS, BNO055_LIA_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
}

void readGRVData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes(BNO055_ADDRESS, BNO055_GRV_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
  destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
}

void initBNO055() {
  byte mode = readByte(BNO055_ADDRESS, BNO055_OPR_MODE);
  Serial.print("BNO055.Initial_Mode:"); Serial.print(mode); Serial.println(';');


  while (mode != CONFIGMODE) {
    // Select BNO055 config mode
     writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE );
     delay(100);
     mode = readByte(BNO055_ADDRESS, BNO055_OPR_MODE);
   }

   // Select page 0 to read sensors
   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);

   //Select BNO055 orientation

//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x24 );//P7
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x05 );//P7

    /* landscape, wires starboard, sensors down  */
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x21 );//P6
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x07 );//P6

   /* wires port, sensors down */
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x21 );//P5
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x01 );//P5

   /* wires forward, sensors down */
   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x24 );//P4
   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x03 );//P4

//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x21 );//P3
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x02 );//P3
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x24 );//P2
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x06 );//P2

     /*  Portait, wires forward, sensors up  */
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x24 );//P1
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x00 );//P1
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_CONFIG, 0x21 );//P0
//   writeByte(BNO055_ADDRESS, BNO055_AXIS_MAP_SIGN, 0x04 );//P0

   // Select BNO055 gyro temperature source
//   writeByte(BNO055_ADDRESS, BNO055_TEMP_SOURCE, 0x01 );

   // Select BNO055 sensor units (temperature in degrees C, rate in dps, accel in mg)
   writeByte(BNO055_ADDRESS, BNO055_UNIT_SEL, 0x01 );

   // Select BNO055 system power mode
   writeByte(BNO055_ADDRESS, BNO055_PWR_MODE, PWRMode );

   // Select BNO055 system operation mode
  while (mode != OPRMode) {
    // Select BNO055 config mode
     writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, OPRMode );
     delay(100);
     mode = readByte(BNO055_ADDRESS, BNO055_OPR_MODE);
   }


}

static bool initalized = false;
static Timer bno055_sample_timer;
static Timer report_timer;
void BNO055::device_setup()
{
  Wire.begin();
  Wire.beginTransmission(BNO055_ADDRESS);
  if (Wire.endTransmission() != 0)
    return; //Cannot find I2c device, abort setup

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = readByte(BNO055_ADDRESS, BNO055_CHIP_ID);  // Read WHO_AM_I register for BNO055
  Serial.print("BNO055.Address:"); Serial.print(BNO055_ADDRESS); Serial.println(';');
  Serial.print("BNO055.WHO_AM_I:"); Serial.print(BNO055_CHIP_ID); Serial.println(';');
  Serial.print("BNO055.IAM_160:"); Serial.print(c); Serial.println(';');




  if (c == 0xA0) // BNO055 WHO_AM_I should always be 0xA0
  {
    Serial.println("BNO055.status:online;");

    // System Reset
    writeByte(BNO055_ADDRESS, BNO055_SYS_TRIGGER, 0x20);
    delay(1000);


    // Check software revision ID
    byte swlsb = readByte(BNO055_ADDRESS, BNO055_SW_REV_ID_LSB);
    byte swmsb = readByte(BNO055_ADDRESS, BNO055_SW_REV_ID_MSB);
    Serial.print("BNO055.SW_Revision_ID:"); Serial.print(swmsb, HEX); Serial.print("."); Serial.print(swlsb, HEX); Serial.println(";");

    // Check bootloader version
    byte blid = readByte(BNO055_ADDRESS, BNO055_BL_REV_ID);
    Serial.print("BNO055.bootloader:"); Serial.print(blid); Serial.println(";");

    // Check self-test results
    byte selftest = readByte(BNO055_ADDRESS, BNO055_ST_RESULT);

    Serial.print("BNO055.selftest.acc:");
    if(selftest & 0x01) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");
    Serial.print("BNO055.selftest.mag:");
    if(selftest & 0x02) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");
    Serial.print("BNO055.selftest.gyro:");
    if(selftest & 0x04) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");
    Serial.print("BNO055.selftest.mcu:");
    if(selftest & 0x08) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");

    // BILT Check self-test results
    writeByte(BNO055_ADDRESS, BNO055_SYS_TRIGGER, 0x01);
    delay(1000);
    selftest = readByte(BNO055_ADDRESS, BNO055_ST_RESULT);
    Serial.print("BNO055.selftest2.acc:");
    if(selftest & 0x01) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");
    Serial.print("BNO055.selftest2.mag:");
    if(selftest & 0x02) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");
    Serial.print("BNO055.selftest2.gyro:");
    if(selftest & 0x04) {
      Serial.println("passed");
    } else {
      Serial.println("failed");
    }
    Serial.println(";");


  initBNO055(); // Initialize the BNO055
  Serial.println("BNO055.status:initialized;");
  initalized=true;
  }
  else
  {
    Serial.println("BNO055.status:failed;");
  }
 bno055_sample_timer.reset();
 report_timer.reset();
}

void BNO055::device_loop(Command command)
{
  if (bno055_sample_timer.elapsed(1000/75)){
    if(!initalized){
      if(report_timer.elapsed(30000)){
        device_setup();
      }
      return;
    }

    if (report_timer.elapsed(500)){
    // Check self-test results
    byte magCal = readByte(BNO055_ADDRESS, BNO055_CALIB_STAT);
    Serial.print("BNO055.CALIB_MAG:"); Serial.print(magCal & 0x03); Serial.println(';');
    Serial.print("BNO055.CALIB_ACC:"); Serial.print(magCal>>2 & 0x03); Serial.println(';');
    Serial.print("BNO055.CALIB_GYR:"); Serial.print(magCal>>4 & 0x03); Serial.println(';');
    Serial.print("BNO055.CALIB_SYS:"); Serial.print(magCal>>6 & 0x03); Serial.println(';');

    byte mode = readByte(BNO055_ADDRESS, BNO055_OPR_MODE);
    Serial.print("BNO055.MODE:"); Serial.print(mode); Serial.println(';');

      if (mode != OPRMode){
    	initalized=false;
      }
    }

    readEulData(EulCount);  // Read the x/y/z adc values
    // Calculate the Euler angles values in degrees
    bYaw = (float)EulCount[0]/16.;
    bRoll = -(float)EulCount[1]/16.;
    bPitch = (float)EulCount[2]/16.;


      navdata::PITC = bPitch;
      navdata::ROLL = bRoll;
      navdata::YAW = bYaw;
      navdata::HDGD = bYaw;
  }
}
#endif
