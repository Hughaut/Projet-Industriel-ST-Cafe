/*!
 * @file DFRobot_HX711_I2C.cpp
 * @brief Port STM32 HAL de la librairie DFRobot_HX711_I2C (origine Arduino)
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @https://github.com/DFRobot/DFRobot_HX711_I2C
 */
#include "DFRobot_HX711_I2C.h"
#include <cstring>

#define HX711_I2C_TIMEOUT_MS  100

DFRobot_HX711_I2C::DFRobot_HX711_I2C(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
  // Le constructeur ne fait que stocker : aucun appel HAL ici
  // (il est execute avant main(), donc avant l'init de l'I2C).
  _hi2c = hi2c;
  _address = (uint16_t)(addr << 1);
}

int DFRobot_HX711_I2C::begin(void)
{
  // Equivalent de Wire.beginTransmission + write(0x70) + write(0x65) + endTransmission
  uint8_t buf[2] = { REG_DATA_INIT_SENSOR, REG_CLEAR_REG_STATE };

  if (HAL_I2C_Master_Transmit(_hi2c, _address, buf, 2, HX711_I2C_TIMEOUT_MS) == HAL_OK) {
    _offset = average(10);
    return 1;
  }
  return 0;
}

float DFRobot_HX711_I2C::readWeight(uint8_t times)
{
  long value = average(times);
  uint8_t ppFlag = peelFlag();

  if (ppFlag == 1) {
    _offset = average(times);
  } else if (ppFlag == 2) {
    _calibration = getCalibration();
  }
  return (((float)value - _offset) / _calibration);
}

bool DFRobot_HX711_I2C::getCalFlag()
{
  return (peelFlag() == 2);
}

void DFRobot_HX711_I2C::enableCal()
{
  uint8_t data = 0;
  writeReg(REG_CLICK_CAL, &data, 1);
}

void DFRobot_HX711_I2C::peel()
{
  _offset = average();
  uint8_t data = 0;
  writeReg(REG_CLICK_RST, &data, 1);
}

long DFRobot_HX711_I2C::average(uint8_t times)
{
  if (times == 0) return 0;
  long sum = 0;
  for (uint8_t i = 0; i < times; i++) {
    sum += getValue();
  }
  return sum / times;
}

float DFRobot_HX711_I2C::getCalibration()
{
  uint8_t data[4] = {0};
  readReg(REG_DATA_GET_CALIBRATION, data, 4);

  uint32_t value = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                   ((uint32_t)data[2] << 8)  |  (uint32_t)data[3];

  float cal;
  memcpy(&cal, &value, sizeof(cal));   // reinterpretation sure des 32 bits en float
  return cal;
}

void DFRobot_HX711_I2C::setCalibration(float value)
{
  _calibration = value;
}

uint8_t DFRobot_HX711_I2C::peelFlag()
{
  uint8_t data[1] = {0};
  readReg(REG_DATA_GET_PEEL_FLAG, data, 1);

  if (data[0] == 1) return 1;
  if (data[0] == 2) return 2;
  return 0;
}

long DFRobot_HX711_I2C::getValue()
{
  uint8_t data[4] = {0};
  long value = 0;
  readReg(REG_DATA_GET_RAM_DATA, data, 4);

  if (data[0] == 0x12) {
    value = (long)data[1];
    value = (long)((value << 8) | data[2]);
    value = (long)((value << 8) | data[3]);
  }
  return (value ^ 0x800000);
}

void DFRobot_HX711_I2C::setThreshold(uint16_t threshold)
{
  uint8_t txData[2];
  txData[0] = threshold >> 8;
  txData[1] = threshold & 0xFF;
  writeReg(REG_SET_CAL_THRESHOLD, txData, 2);
  HAL_Delay(50);
}

void DFRobot_HX711_I2C::setCalWeight(uint16_t triWeight)
{
  uint8_t txData[2];
  txData[0] = triWeight >> 8;
  txData[1] = triWeight & 0xFF;
  writeReg(REG_SET_TRIGGER_WEIGHT, txData, 2);
  HAL_Delay(50);
}

int DFRobot_HX711_I2C::readReg(uint8_t reg, uint8_t *data, size_t size)
{
  // 1) ecriture du numero de registre (avec STOP)
  if (HAL_I2C_Master_Transmit(_hi2c, _address, &reg, 1, HX711_I2C_TIMEOUT_MS) != HAL_OK) {
    return 0;
  }
  // 2) le capteur a besoin de temps pour preparer la donnee
  HAL_Delay(22);
  // 3) lecture
  if (HAL_I2C_Master_Receive(_hi2c, _address, data, (uint16_t)size, HX711_I2C_TIMEOUT_MS) != HAL_OK) {
    return 0;
  }
  return 1;
}

uint8_t DFRobot_HX711_I2C::writeReg(uint8_t reg, const void *data, size_t size)
{
  if (data == NULL) return 0;

  // Une seule transaction : adresse, reg, donnees (comme l'original avec Wire)
  return (HAL_I2C_Mem_Write(_hi2c, _address, reg, I2C_MEMADD_SIZE_8BIT,
                            (uint8_t *)data, (uint16_t)size,
                            HX711_I2C_TIMEOUT_MS) == HAL_OK) ? 1 : 0;
}
