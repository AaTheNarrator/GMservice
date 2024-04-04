//
//    FILE: MS5611_SPI.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.2
// PURPOSE: MS5611 (SPI) Temperature & Pressure library for Arduino
//     URL: https://github.com/RobTillaart/MS5611_SPI
//
//  HISTORY: see changelog.md


#include "MS5611_SPI.h"

// datasheet page 10
#define MS5611_CMD_READ_ADC       0x00
#define MS5611_CMD_READ_PROM      0xA0
#define MS5611_CMD_RESET          0x1E
#define MS5611_CMD_CONVERT_D1     0x40
#define MS5611_CMD_CONVERT_D2     0x50

/////////////////////////////////////////////////////
//
//  PUBLIC
//
MS5611_SPI::MS5611_SPI(uint8_t select, uint8_t dataOut, uint8_t dataIn, uint8_t clock)
{
  //  _address           = deviceAddress;  // TODO
  _samplingRate      = OSR_ULTRA_LOW;
  _temperature       = MS5611_NOT_READ;
  _pressure          = MS5611_NOT_READ;
  _result            = MS5611_NOT_READ;
  _lastRead          = 0;
  _deviceID          = 0;
  _pressureOffset    = 0;
  _temperatureOffset = 0;
  _compensation      = true;

  //  SPI
  _select   = 25;
  _dataIn   = dataIn;
  _dataOut  = dataOut;
  _clock    = clock;
  _hwSPI    = 1;//(dataIn == 255) && (dataOut == 255) && (clock == 255);
}


bool MS5611_SPI::begin()
{
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);

  mySPI = new SPIClass(HSPI);
  mySPI->end();
  mySPI->begin(16, 4, 17, 25);
    
  return reset();
}


bool MS5611_SPI::isConnected()
{
  int rv = read();
  return (rv == MS5611_READ_OK);
}

bool MS5611_SPI::reset(uint8_t mathMode)
{
  command(MS5611_CMD_RESET);
  uint32_t start = micros();
  while (micros() - start < 3000)     //  increased as first ROM values were missed.
  {
    yield();
    delayMicroseconds(10);
  }

  //  initialize the C[] array
  initConstants(mathMode);

  // read factory calibrations from EEPROM.
  bool ROM_OK = true;
  for (uint8_t reg = 0; reg < 7; reg++)
  {
    //  used indices match datasheet.
    //  C[0] == manufacturer - read but not used;
    //  C[7] == CRC - skipped.
    uint16_t tmp = readProm(reg);
    Serial.println(tmp);
    C[reg] *= tmp;
    //  _deviceID is a simple SHIFT XOR merge of PROM data
    _deviceID <<= 4;
    _deviceID ^= tmp;
    //  Serial.println(readProm(reg));
    if (reg > 0)
    {
      ROM_OK = ROM_OK && (tmp != 0);
    }
  }
  return ROM_OK;
}


int MS5611_SPI::read(uint8_t bits)
{
  //  VARIABLES NAMES BASED ON DATASHEET
  //  ALL MAGIC NUMBERS ARE FROM DATASHEET
  //  NOTE: D1 and D2 seem reserved in MBED (NANO BLE)
  command(MS5611_CMD_CONVERT_D1);
  delay(1);
  uint32_t _D1 = readADC();
  command(MS5611_CMD_CONVERT_D2);
  delay(1);
  uint32_t _D2 = readADC();
  //  TEMP & PRESS MATH - PAGE 7/20
  float dT = _D2 - C[5];
  _temperature = 2000 + dT * C[6];
  float offset =  C[2] + dT * C[4];
  float sens = C[1] + dT * C[3];
  if (_compensation)
  {
    if (_temperature < 2000)
    {
      float T2 = dT * dT * 4.6566128731E-10;
      float t = (_temperature - 2000) * (_temperature - 2000);
      float offset2 = 2.5 * t;
      float sens2 = 1.25 * t;
      //  COMMENT OUT < -1500 CORRECTION IF NOT NEEDED
      if (_temperature < -1500)
	{
	  t = (_temperature + 1500) * (_temperature + 1500);
	  offset2 += 7 * t;
	  sens2 += 5.5 * t;
	}
      _temperature -= T2;
      offset -= offset2;
      sens -= sens2;
    }
    //  END SECOND ORDER COMPENSATION
  }
  _pressure = (_D1 * sens * 4.76837158205E-7 - offset) * 3.051757813E-5;
  _lastRead = millis();
  return MS5611_READ_OK;
}


void MS5611_SPI::setOversampling(osr_t samplingRate)
{
  _samplingRate = (uint8_t) samplingRate;
}


osr_t MS5611_SPI::getOversampling() const
{
  return (osr_t) _samplingRate;
};


float MS5611_SPI::getTemperature() const
{
  if (_temperatureOffset == 0) return _temperature * 0.01;
  return _temperature * 0.01 + _temperatureOffset;
};


float MS5611_SPI::getPressure() const
{
  // давление в мм рт. ст.
  if (_pressureOffset == 0) return _pressure * 0.01 * 0.75;
  return _pressure * 0.01 + _pressureOffset * 0.75;
};


void MS5611_SPI::setPressureOffset(float offset)
{
  _pressureOffset = offset;
};


float MS5611_SPI::getPressureOffset()
{
  return _pressureOffset;
};


void MS5611_SPI::setTemperatureOffset(float offset)
{
  _temperatureOffset = offset;
};


float MS5611_SPI::getTemperatureOffset()
{
  return _temperatureOffset;
};

uint32_t MS5611_SPI::getDeviceID() const
{
  return _deviceID;
};


void MS5611_SPI::setCompensation(bool flag)
{
  _compensation = flag;
};


bool MS5611_SPI::getCompensation()
{
  return _compensation;
};


//       EXPERIMENTAL
uint16_t MS5611_SPI::getManufacturer()
{
  return readProm(0);
}

//       EXPERIMENTAL
uint16_t MS5611_SPI::getSerialCode()
{
  return readProm(7) >> 4;
}

uint16_t MS5611_SPI::readProm(uint8_t reg)
{
  //  last EEPROM register is CRC - Page 13 datasheet.
  uint16_t value = 0;
  digitalWrite(_select, LOW);
  mySPI->beginTransaction(_spi_settings);
  mySPI->transfer(0xA0 + reg * 2);
  value = mySPI->transfer(0x00);
  value <<= 8;
  value += mySPI->transfer(0x00);
  mySPI->endTransaction();
  digitalWrite(_select, HIGH);
  return value;
}


uint32_t MS5611_SPI::readADC()
{
  //  command(MS5611_CMD_READ_ADC);
  uint32_t value = 0;
  digitalWrite(_select, LOW);
  mySPI->beginTransaction(_spi_settings);
  mySPI->transfer(0x00);
  value += mySPI->transfer(0x00);
  value <<= 8;
  value += mySPI->transfer(0x00);
  value <<= 8;
  value += mySPI->transfer(0x00);
  mySPI->endTransaction();
  digitalWrite(_select, HIGH);
  //Serial.println(value, HEX);
  return value;
}


int MS5611_SPI::command(const uint8_t command)
{
  yield();
  digitalWrite(_select, LOW);
  mySPI->beginTransaction(_spi_settings);
  mySPI->transfer(command);
  mySPI->endTransaction();
  digitalWrite(_select, HIGH);
  return 0;
}

void MS5611_SPI::initConstants(uint8_t mathMode)
{
  //  constants that were multiplied in read() - datasheet page 8
  //  do this once and you save CPU cycles
  //
  //                               datasheet ms5611     |    appNote
  //                                mode = 0;           |    mode = 1
  C[0] = 1;
  C[1] = 32768L;          //  SENSt1   = C[1] * 2^15    |    * 2^16
  C[2] = 65536L;          //  OFFt1    = C[2] * 2^16    |    * 2^17
  C[3] = 3.90625E-3;      //  TCS      = C[3] / 2^8     |    / 2^7
  C[4] = 7.8125E-3;       //  TCO      = C[4] / 2^7     |    / 2^6
  C[5] = 256;             //  Tref     = C[5] * 2^8     |    * 2^8
  C[6] = 1.1920928955E-7; //  TEMPSENS = C[6] / 2^23    |    / 2^23

  if (mathMode == 1)  //  Appnote version for pressure.
  {
    C[1] = 65536L;          //  SENSt1
    C[2] = 131072L;         //  OFFt1
    C[3] = 7.8125E-3;       //  TCS
    C[4] = 1.5625e-2;       //  TCO
  }
}
