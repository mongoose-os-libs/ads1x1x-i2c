/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "mgos.h"
#include "mgos_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_ads1x1x;

enum mgos_ads1x1x_type {
  ADC_NONE = 0,
  ADC_ADS1013,
  ADC_ADS1014,
  ADC_ADS1015,
  ADC_ADS1113,
  ADC_ADS1114,
  ADC_ADS1115,
  ADC_ADS1119
};

enum mgos_ads1x1x_fsr {
  MGOS_ADS1X1X_FSR_MIN = 0, // 6.144V
  MGOS_ADS1X1X_FSR_6144,    // 6.144V
  MGOS_ADS1X1X_FSR_4096,    // 4.096V
  MGOS_ADS1X1X_FSR_2048,    // 2.048V
  MGOS_ADS1X1X_FSR_1024,    // 1.024V
  MGOS_ADS1X1X_FSR_512,     // 0.512V
  MGOS_ADS1X1X_FSR_256,     // 0.256V
  MGOS_ADS1X1X_FSR_DEFAULT, // 2.048V
  MGOS_ADS1X1X_FSR_MAX,     // 0.256V
};

enum mgos_ads1x1x_dr {
  MGOS_ADS1X1X_SPS_MIN = 0, // 8SPS for ADS111X, 128SPS for ADS101X
  MGOS_ADS1X1X_SPS_8,       // 8SPS, ADS111X only
  MGOS_ADS1X1X_SPS_16,      // 16SPS, ADS111X only
  MGOS_ADS1X1X_SPS_20,      // 20SPS, ADS1119 only (default)
  MGOS_ADS1X1X_SPS_32,      // 32SPS, ADS111X only
  MGOS_ADS1X1X_SPS_64,      // 64SPS, ADS111X only
  MGOS_ADS1X1X_SPS_90,      // 90SPS, ADS1119 only
  MGOS_ADS1X1X_SPS_128,     // 128SPS, both ADS111X and ADS101X
  MGOS_ADS1X1X_SPS_250,     // 250SPS, both ADS111X and ADS101X
  MGOS_ADS1X1X_SPS_330,     // 330SPS, ADS1119 only
  MGOS_ADS1X1X_SPS_475,     // 475SPS, ADS111X only
  MGOS_ADS1X1X_SPS_490,     // 490SPS, ADS101X only
  MGOS_ADS1X1X_SPS_860,     // 860SPS, ADS111X only
  MGOS_ADS1X1X_SPS_920,     // 920SPS, ADS101X only
  MGOS_ADS1X1X_SPS_1000,    // 1000SPS, ADS1119 only
  MGOS_ADS1X1X_SPS_1600,    // 1600SPS, ADS101X only
  MGOS_ADS1X1X_SPS_2400,    // 2400SPS, ADS101X only
  MGOS_ADS1X1X_SPS_3300,    // 3300SPS, ADS101X only
  MGOS_ADS1X1X_SPS_DEFAULT, // 128SPS for ADS111X, 1600SPS for ADS101X, 20SPS for ADS1119
  MGOS_ADS1X1X_SPS_MAX,     // 860SPS for ADS111X, 3300SPS for ADS101X, 1000SPS for ADS1119
};

enum mgos_ads1119_gain
{
    MGOS_ADS1119_GAIN_1 = 0,
    MGOS_ADS1119_GAIN_4
};

enum mgos_ads1119_conversion_mode
{
    MGOS_ADS1119_CM_SS = 0,
    MGOS_ADS1119_CM_CONT
};

enum mgos_ads1119_vref
{
    MGOS_ADS1119_VREF_INT = 0, // Internal 2.048v
    MGOS_ADS1119_VREF_EXT
};

/*
 * Initialize a ADS1X1X on the I2C bus `i2c` at address specified in `i2caddr`
 * parameter (default ADS1X1X is on address 0x48). The device will be polled for
 * validity, upon success a new `struct mgos_ads1x1x` is allocated and
 * returned. If the device could not be found, NULL is returned.
 */
struct mgos_ads1x1x*
mgos_ads1x1x_create(struct mgos_i2c* i2c, uint8_t i2caddr, enum mgos_ads1x1x_type type);

/*
 * Initialize a ADS1119 on the I2C bus `i2c` at address specified in `i2caddr`
 * parameter (default ADS1X1X is on address 0x48). The device will be polled for
 * validity, upon success a new `struct mgos_ads1x1x` is allocated and
 * returned. If the device could not be found, NULL is returned.
 * Refer to TI product PDF, Configuration Register, for information on what to use for mux/gain/DR/CM/VREF.
 * Note, use the decimal value for each, ie AinP = AIN2, AinN=AGND is decimal 5 (101 binary).
 * Mux is provided at read time
 */
//struct mgos_ads1x1x* mgos_ads1119_create(struct mgos_i2c* i2c, uint8_t i2caddr, enum mgos_ads1x1x_type type, enum mgos_ads1119_gain gain = MGOS_ADS1119_GAIN_1, enum mgos_ads1x1x_dr dataRate = MGOS_ADS1X1X_SPS_20, enum mgos_ads1119_conversion_mode conversionMode = MGOS_ADS1119_CM_SS, enum mgos_ads1119_vref vRef = MGOS_ADS1119_VREF_INT);
struct mgos_ads1x1x* mgos_ads1119_create(struct mgos_i2c* i2c, uint8_t i2caddr, enum mgos_ads1x1x_type type, enum mgos_ads1119_gain gain, enum mgos_ads1x1x_dr dataRate, enum mgos_ads1119_conversion_mode conversionMode, enum mgos_ads1119_vref vRef);

/*
 * Destroy the data structure associated with a ADS1X1X device. The reference
 * to the pointer of the `struct mgos_ads1x1x` has to be provided, and upon
 * successful destruction, its associated memory will be freed and the pointer
 * set to NULL and true will be returned.
 */
bool mgos_ads1x1x_destroy(struct mgos_ads1x1x **dev);

/* Get or Set the Full Scale Range (FSR). All chips in the ADS1X1X family support
 * the same settings. By default, 2.048V is used.
 *
 * Note: ADS1x13 does not support this, and always has an FSR of 2.048V.
 *
 * Returns true on success, false otherwise.
 */
bool mgos_ads1x1x_set_fsr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_fsr fsr);
bool mgos_ads1x1x_get_fsr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_fsr *fsr);

/* Get or Set the Data Rate (in Samples per Second). If the supplied `dr`
 * argument cannot be set on the chip, false is returned. Otherwise, the
 * supplied `dr` is set. By default, ADS101X sets 1600SPS, ADS111X sets 128SPS.
 *
 * Returns true on success, false otherwise.
 */
bool mgos_ads1x1x_set_dr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_dr dr);
bool mgos_ads1x1x_get_dr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_dr *dr);

/* Read a channel from the ADC and return the read value in `result`. If the
 * channel was invalid, or an error occurred, false is returned and the result
 * cannot be relied upon.
 *
 * Returns true on success, false otherwise.
 */
bool mgos_ads1x1x_read(struct mgos_ads1x1x *dev, uint8_t chan, int16_t *result);

bool mgos_ads1119_read(struct mgos_ads1x1x* dev, uint8_t mux, int16_t* result);

/* Read a 2-channel differential from the ADC and return the read value in `result`.
 * If the channel pair invalid, or an error occurred, false is returned and the
 * result cannot be relied upon. Upon success, true is returned.
 *
 * Note: This is only available on ADS1X15 chips.
 *       Valid chanP/chanN pairs are : 0/1, 0/3, 1/3, 2/3.
 *
 * Returns true on success, false otherwise.
 */
bool mgos_ads1x1x_read_diff(struct mgos_ads1x1x *dev, uint8_t chanP, uint8_t chanN, int16_t *result);

/* Centralised helper function to write configuration to an ADS1119
* Sends the command for WREG first
*/
bool mgos_ads1119_write_conf(struct mgos_ads1x1x * dev, uint8_t value);

#ifdef __cplusplus
}
#endif
