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

#include "mgos_ads1x1x_internal.h"

static char *mgos_ads1x1x_type_str(struct mgos_ads1x1x *dev) {
  if (!dev) {
    return "VOID";
  }
  switch (dev->type) {
  case ADC_NONE: return "NONE";

  case ADC_ADS1013: return "ADS1013";

  case ADC_ADS1014: return "ADS1014";

  case ADC_ADS1015: return "ADS1015";

  case ADC_ADS1113: return "ADS1113";

  case ADC_ADS1114: return "ADS1114";

  case ADC_ADS1115: return "ADS1115";

  default: return "UNKNOWN";
  }
}

static bool mgos_ads1x1x_reset(struct mgos_ads1x1x *dev) {
  if (!dev) {
    return false;
  }

  return mgos_i2c_write_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF, 0x8583);
}

struct mgos_ads1x1x *mgos_ads1x1x_create(struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_ads1x1x_type type) {
  struct mgos_ads1x1x *dev = NULL;

  if (!i2c || type == ADC_NONE || type > ADC_ADS1115) {
    return NULL;
  }

  dev = calloc(1, sizeof(struct mgos_ads1x1x));
  if (!dev) {
    return NULL;
  }

  memset(dev, 0, sizeof(struct mgos_ads1x1x));
  dev->i2caddr = i2caddr;
  dev->i2c     = i2c;
  dev->type    = type;
  switch (type) {
  case ADC_ADS1015:
  case ADC_ADS1115:
    dev->channels = 4;
    break;

  default:
    dev->channels = 1;
  }

  if (!mgos_ads1x1x_reset(dev)) {
    LOG(LL_INFO, ("Could not reset %s at I2C 0x%02x", mgos_ads1x1x_type_str(dev), dev->i2caddr));
    free(dev);
    return NULL;
  }
  LOG(LL_INFO, ("%s initialized at I2C 0x%02x", mgos_ads1x1x_type_str(dev), dev->i2caddr));
  return dev;
}

bool mgos_ads1x1x_destroy(struct mgos_ads1x1x **dev) {
  if (!*dev) {
    return false;
  }

  free(*dev);
  *dev = NULL;
  return true;
}

bool mgos_ads1x1x_set_fsr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_fsr fsr) {
  uint16_t val;

  if (!dev) {
    return false;
  }

  switch (fsr) {
  case MGOS_ADS1X1X_FSR_MAX:
  case MGOS_ADS1X1X_FSR_6144:
    val = 0;
    break;

  case MGOS_ADS1X1X_FSR_4096:
    val = 1;
    break;

  case MGOS_ADS1X1X_FSR_1024:
    val = 3;
    break;

  case MGOS_ADS1X1X_FSR_512:
    val = 4;
    break;

  case MGOS_ADS1X1X_FSR_MIN:
  case MGOS_ADS1X1X_FSR_256:
    val = 5;
    break;

  default:   // 2.048V
    val = 2;
  }

  // ADS1x13 only supports 2.048V
  if ((dev->type == ADC_ADS1013 || dev->type == ADC_ADS1113) && val != 2) {
    return false;
  }

  return mgos_i2c_setbits_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF, 9, 3, val);
}

bool mgos_ads1x1x_get_fsr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_fsr *fsr) {
  uint16_t val;

  if (!dev || !fsr) {
    return false;
  }
  if (!mgos_i2c_getbits_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF, 9, 3, &val)) {
    return false;
  }
  switch (val) {
  case 0: *fsr = MGOS_ADS1X1X_FSR_6144; break;

  case 1: *fsr = MGOS_ADS1X1X_FSR_4096; break;

  case 2: *fsr = MGOS_ADS1X1X_FSR_2048; break;

  case 3: *fsr = MGOS_ADS1X1X_FSR_1024; break;

  case 4: *fsr = MGOS_ADS1X1X_FSR_512; break;

  default: *fsr = MGOS_ADS1X1X_FSR_256; break;
  }
  return true;
}

bool mgos_ads1x1x_set_dr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_dr dr) {
  uint16_t val;

  if (!dev) {
    return false;
  }
  switch (dev->type) {
  case ADC_ADS1113:
  case ADC_ADS1114:
  case ADC_ADS1115:
    switch (dr) {
    case MGOS_ADS1X1X_SPS_MIN:
    case MGOS_ADS1X1X_SPS_8:
      val = 0;
      break;

    case MGOS_ADS1X1X_SPS_16:
      val = 1;
      break;

    case MGOS_ADS1X1X_SPS_32:
      val = 2;
      break;

    case MGOS_ADS1X1X_SPS_64:
      val = 3;
      break;

    case MGOS_ADS1X1X_SPS_DEFAULT:
    case MGOS_ADS1X1X_SPS_128:
      val = 4;
      break;

    case MGOS_ADS1X1X_SPS_250:
      val = 5;
      break;

    case MGOS_ADS1X1X_SPS_475:
      val = 6;
      break;

    case MGOS_ADS1X1X_SPS_MAX:
    case MGOS_ADS1X1X_SPS_860:
      val = 7;
      break;

    default: return false;
    }
    break;

  case ADC_ADS1013:
  case ADC_ADS1014:
  case ADC_ADS1015:
    switch (dr) {
    case MGOS_ADS1X1X_SPS_MIN:
    case MGOS_ADS1X1X_SPS_128:
      val = 0;
      break;

    case MGOS_ADS1X1X_SPS_250:
      val = 1;
      break;

    case MGOS_ADS1X1X_SPS_490:
      val = 2;
      break;

    case MGOS_ADS1X1X_SPS_920:
      val = 3;
      break;

    case MGOS_ADS1X1X_SPS_DEFAULT:
    case MGOS_ADS1X1X_SPS_1600:
      val = 4;
      break;

    case MGOS_ADS1X1X_SPS_2400:
      val = 5;
      break;

    case MGOS_ADS1X1X_SPS_MAX:
    case MGOS_ADS1X1X_SPS_3300:
      val = 6;
      break;

    default: return false;
    }
    break;

  default: return false;
  }

  return mgos_i2c_setbits_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF, 5, 3, val);
}

bool mgos_ads1x1x_get_dr(struct mgos_ads1x1x *dev, enum mgos_ads1x1x_dr *dr) {
  uint16_t val;

  if (!dev || !dr) {
    return false;
  }
  if (!mgos_i2c_getbits_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF, 5, 3, &val)) {
    return false;
  }
  switch (dev->type) {
  case ADC_ADS1013:
  case ADC_ADS1014:
  case ADC_ADS1015:
    switch (val) {
    case 0: *dr = MGOS_ADS1X1X_SPS_128; break;

    case 1: *dr = MGOS_ADS1X1X_SPS_250; break;

    case 2: *dr = MGOS_ADS1X1X_SPS_490; break;

    case 3: *dr = MGOS_ADS1X1X_SPS_920; break;

    case 4: *dr = MGOS_ADS1X1X_SPS_1600; break;

    case 5: *dr = MGOS_ADS1X1X_SPS_2400; break;

    default: *dr = MGOS_ADS1X1X_SPS_3300; break;
    }
    break;

  case ADC_ADS1113:
  case ADC_ADS1114:
  case ADC_ADS1115:
    switch (val) {
    case 0: *dr = MGOS_ADS1X1X_SPS_8; break;

    case 1: *dr = MGOS_ADS1X1X_SPS_16; break;

    case 2: *dr = MGOS_ADS1X1X_SPS_32; break;

    case 3: *dr = MGOS_ADS1X1X_SPS_64; break;

    case 4: *dr = MGOS_ADS1X1X_SPS_128; break;

    case 5: *dr = MGOS_ADS1X1X_SPS_250; break;

    case 6: *dr = MGOS_ADS1X1X_SPS_475; break;

    default: *dr = MGOS_ADS1X1X_SPS_860; break;
    }
    break;

  default: return false;
  }
  return true;
}

bool mgos_ads1x1x_read(struct mgos_ads1x1x *dev, uint8_t chan, int16_t *result) {
  return mgos_ads1x1x_read_diff(dev, chan, 0xff, result);
}

// if chanN is 0xff, perform a single ended read (with chanP against GND)
bool mgos_ads1x1x_read_diff(struct mgos_ads1x1x *dev, uint8_t chanP, uint8_t chanN, int16_t *result) {
  uint16_t conf_val;
  int16_t  result_val;
  uint16_t mux;

  if (!dev) {
    return false;
  }
  if (chanN == 0xff) {
    // Single ended read
    if (chanP >= dev->channels) {
      return false;
    }
    switch (chanP) {
    case 0: mux = 4; break;

    case 1: mux = 5; break;

    case 2: mux = 6; break;

    default: mux = 7; break;
    }
  } else {
    if (dev->type != ADC_ADS1015 && dev->type != ADC_ADS1115) {
      if (chanN != 0 || chanP != 1) {
        return false;
      }
      mux = 0;
    } else {
      // ADS1X15: Differential read, only 4 allowed combinations
      if (chanN == 0 && chanP == 1) {
        mux = 0;
      } else if (chanN == 0 && chanP == 3) {
        mux = 1;
      } else if (chanN == 1 && chanP == 3) {
        mux = 2;
      } else if (chanN == 2 && chanP == 3) {
        mux = 3;
      } else {
        return false;
      }
    }
  }
  conf_val = mgos_i2c_read_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF);

  // Clear and then set MUX bits
  conf_val |= (7 << 12);
  conf_val &= ~(7 << 12);
  conf_val |= (mux << 12);

  conf_val |= (1 << 15); // OS - start conversion
  conf_val |= (1 << 8);  // MODE - singleshot

  mgos_i2c_write_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONF, conf_val);

  // ADS101X has 1ms conversion delay, ADS111X has 8ms.
  if (dev->type >= ADC_ADS1113) {
    mgos_usleep(8000);
  } else{
    mgos_usleep(1000);
  }

  result_val = mgos_i2c_read_reg_w(dev->i2c, dev->i2caddr, MGOS_ADS1X1X_REG_POINTER_CONV);

  // ADS101X has 12 bit conversion
  if (dev->type < ADC_ADS1113) {
    result_val /= 16;
  }

  *result = result_val;
  return true;
}

// Mongoose OS library initialization
bool mgos_ads1x1x_i2c_init(void) {
  return true;
}
