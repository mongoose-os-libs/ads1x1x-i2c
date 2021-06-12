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
#include "mgos_ads1x1x.h"

#ifdef __cplusplus
extern "C" {
#endif

// ADS1X1X I2C address, note default for ADS1119 with A0 & A1 tied to GND is 0x40
#define MGOS_ADS1X1X_I2C_ADDR            (0x48)

// Registers
#define MGOS_ADS1X1X_REG_POINTER_MASK    (0x03)
#define MGOS_ADS1X1X_REG_POINTER_CONV    (0x00)
#define MGOS_ADS1119_REG_POINTER_COMMAND (0x00)
#define MGOS_ADS1X1X_REG_POINTER_CONF    (0x01)
#define MGOS_ADS1X1X_REG_POINTER_LO_T    (0x02)
#define MGOS_ADS1X1X_REG_POINTER_HI_T    (0x03)

struct mgos_ads1x1x {
  struct mgos_i2c *      i2c;
  uint8_t                i2caddr;
  enum mgos_ads1x1x_type type;
  uint8_t                channels;
  uint8_t                configuration[2]; // allow for 16 bit configuration with an array. ADS1119 only uses configuration[0]
  uint8_t                mux;
  uint8_t                gain;
  uint8_t                dataRate;
  uint8_t                conversionMode;
  uint8_t                vRef;
};

/* Mongoose OS initializer */
bool mgos_ads1x1x_i2c_init(void);

#ifdef __cplusplus
}
#endif
