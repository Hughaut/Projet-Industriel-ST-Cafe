/*!
 * @file DFRobot_HX711_I2C.h
 * @brief Port STM32 HAL de la librairie DFRobot_HX711_I2C (origine Arduino)
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @https://github.com/DFRobot/DFRobot_HX711_I2C
 */

#ifndef DFROBOT_HX711_I2C_H
#define DFROBOT_HX711_I2C_H

#include "main.h"
#include <cstddef>
#include <cstdint>

#ifndef HX711_I2C_ADDR
#define HX711_I2C_ADDR  (0x64)  ///< adresse I2C 7 bits du capteur
#endif

class DFRobot_HX711_I2C
{
public:
  #define REG_CLEAR_REG_STATE         0x65
  #define REG_DATA_GET_RAM_DATA       0x66
  #define REG_DATA_GET_CALIBRATION    0x67
  #define REG_DATA_GET_PEEL_FLAG      0x69
  #define REG_DATA_INIT_SENSOR        0x70
  #define REG_SET_CAL_THRESHOLD       0x71
  #define REG_SET_TRIGGER_WEIGHT      0x72
  #define REG_CLICK_RST               0x73
  #define REG_CLICK_CAL               0x74

  /**
   * @param hi2c  handle HAL de l'I2C (ex: &hi2c1)
   * @param addr  adresse I2C 7 bits (0x64 par defaut), le decalage est fait en interne
   */
  DFRobot_HX711_I2C(I2C_HandleTypeDef *hi2c, uint8_t addr = HX711_I2C_ADDR);

  /** @return 1 si le capteur repond, 0 sinon */
  int begin(void);

  /** @return poids en g, moyenne sur 'times' mesures */
  float readWeight(uint8_t times = 12);

  float getCalibration();
  void  setCalibration(float value);
  void  setThreshold(uint16_t threshold);
  void  setCalWeight(uint16_t triWeight);
  void  enableCal();
  void  peel();
  bool  getCalFlag();

private:
  long    getValue();
  uint8_t peelFlag();
  long    average(uint8_t times = 10);

  int     readReg(uint8_t reg, uint8_t *data, size_t size);
  uint8_t writeReg(uint8_t reg, const void *data, size_t size);

  I2C_HandleTypeDef *_hi2c;
  uint16_t _address;          // adresse deja decalee (<< 1) pour le HAL
  long     _offset = 0;
  float    _calibration = 2236.0f;
};

#endif
