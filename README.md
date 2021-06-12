# ADS1X1X I2C Driver

A Mongoose library for various `I2C` speaking ADCs from Texas Instruments:

*   ADS1119 - 16bit, 1000 Samples/sec, 2 differential / 4 single-ended, programmable gain.
*   ADS1115 - 16bit, 860 Samples/sec, 2 differential / 4 single-ended, programmable gain
*   ADS1114 - 16bit, 860 Samples/sec, 1 differential / 1 single-ended, programmable gain
*   ADS1113 - 16bit, 860 Samples/sec, 1 differential / 1 single-ended, no gain
*   ADS1015 - 12bit, 3300 Samples/sec, 2 differential / 4 single-ended, programmable gain
*   ADS1014 - 12bit, 3300 Samples/sec, 1 differential / 1 single-ended, programmable gain
*   ADS1013 - 12bit, 3300 Samples/sec, 1 differential / 1 single-ended, no gain.

The most common are the **ADS1115** and **ADS1015** chips.

## Implementation details

The driver takes care of exposing the correct fuctionality based on which `type`
is created. Differential measurements can be taken on all devices, but only
`ADS1x15` has multiple options.

## ADS 1013-1115 API

First, create a device using `mgos_ads1x1x_create()` by specifying the type of
chip you're using. Take some measurements using `mgos_ads1x1x_read()`, and
clean up the driver by using `mgos_ads1x1x_destroy()`.

`mgos_ads1x1x_set_fsr()` is used to set the full scale range (FSR) of
the ADC. Each chip supports ranges from 6.144 Volts down to 0.256 Volts. You
can read the current FSR with `mgos_ads1x1x_get_fsr()`.

`mgos_ads1x1x_set_dr()` is used to set the data rate of continuous
measurements. The support differs between `ADS101X` (the 12-bit version,
which is faster), and `ADS111X` (the 16-bit version, which is slower). You
can read the current DR with `mgos_ads1x1x_get_dr()`.

`mgos_ads1x1x_read()` starts a singleshot measurement on the given
channel (which takes 1ms for `ADS101X` and 8ms for `ADS111X`), and
returns a 16 bit signed value. The datasheet mentions that with input
voltages around `GND`, a negative value might be returned (ie -2 rather
than 0).

`mgos_ads1x1x_read_diff()` starts a singleshot measurement of
the differential voltage between two channels, typically `Chan0` and
`Chan1`. Several channel pairs are allowed, see the include file for
details. Note, that this function is only available on `ADS1X15` chips.

#### Example application (!ADS1119)

```
#include "mgos.h"
#include "mgos_config.h"
#include "mgos_ads1x1x.h"

void timer_cb(void *data) {
  struct mgos_ads1x1x *d = (struct mgos_ads1x1x *)data;
  int16_t res[4];

  if (!d) return;

  for(int i=0; i<4; i++) {
    if (!mgos_ads1x1x_read(s_adc, i, &res[i])) {
      LOG(LL_ERROR, ("Could not read device"));
      return;
    }
  }
  LOG(LL_INFO, ("chan={%6d, %6d, %6d, %6d}", res[0], res[1], res[2], res[3]));
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_ads1x1x *d = NULL;

  if (!(d = mgos_ads1x1x_create(mgos_i2c_get_global(), 0x48, ADC_ADS1115))) {
    LOG(LL_ERROR, ("Could not create ADS1115"));
    return MGOS_APP_INIT_ERROR;
  }

  mgos_set_timer(100, true, timer_cb, d);

  return MGOS_APP_INIT_SUCCESS;
}

```

## ADS 1119 API 

TI has simplified the command API and configuration registers during the development of the ADS1119 so it works quite differently to prior chips. 

First, create a device using `mgos_ads1119_create()` by specifying the type of
chip you're using. Take some measurements using `mgos_ads1119_read()`, and
clean up the driver by using `mgos_ads1x1x_destroy()`.

#### Creation

During initiation you can specify the settings such as data rate, gain, single shot vs continuous and whether to use the internal 2.048v vREF or an external vREF.
 
For example: `mgos_ads1119_create(mgos_i2c_get_global(), 0x40, ADC_ADS1119, MGOS_ADS1119_GAIN_1, MGOS_ADS1X1X_SPS_20, MGOS_ADS1119_CM_SS, MGOS_ADS1119_VREF_EXT)`


```
if (!(ads = mgos_ads1119_create(mgos_i2c_get_global(), 0x40, ADC_ADS1119, MGOS_ADS1119_GAIN_1, MGOS_ADS1X1X_SPS_20, MGOS_ADS1119_CM_SS, MGOS_ADS1119_VREF_EXT))) {
  LOG(LL_ERROR, ("I2C Could not create ADS1119"));
}
else {      
  LOG(LL_INFO, ("I2C Have created ADS1119 device successfully"));
}
``` 

#### Reading values
To read either a single or differential value you need to supply the MUX value per 'Configuration Register Field Descriptions' in the TI datasheet.

The function is of the form `mgos_ads1119_read(struct mgos_ads1x1x* dev, uint8_t mux, int16_t* result)`.

For example to read single ended AIN3 & GND, that corresponds to a mux value of binary 110 / decimal 7, and would have a call like:
`int16_t res; mgos_ads1119_read(ads, 7, &res);`  

#### Example application

This example reads all the channels as single ended. As the 5th result element it reads the voltage offset value (AINP and AINN shorted to AVDD / 2) to enable calibration.  

```
#include "mgos.h"
#include "mgos_config.h"
#include "mgos_ads1x1x.h"

void timer_cb(void *data) {
  struct mgos_ads1x1x *d = (struct mgos_ads1x1x *)data;
  int16_t res[5];

  if (!d) return;

  for(int i=0; i<5; i++) {
    if (!mgos_ads1119_read(s_adc, i+3, &res[i])) {
      LOG(LL_ERROR, ("Could not read device"));
      return;
    }
  }
  LOG(LL_INFO, ("chan={%6d, %6d, %6d, %6d} offset=%d", res[0], res[1], res[2], res[3], res[4]));
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_ads1x1x *d = NULL;

  if (!(d = mgos_ads1119_create(mgos_i2c_get_global(), 0x40, ADC_ADS1119, MGOS_ADS1119_GAIN_1, MGOS_ADS1X1X_SPS_20, MGOS_ADS1119_CM_SS, MGOS_ADS1119_VREF_EXT))) {
    LOG(LL_ERROR, ("Could not create ADS1119"));
    return MGOS_APP_INIT_ERROR;
  }

  mgos_set_timer(100, true, timer_cb, d);

  return MGOS_APP_INIT_SUCCESS;
}

```

Notes:
- TI Datasheet: http://www.ti.com/lit/ds/symlink/ads1119.pdf
- Have observed that it will return a negative reading if a thermistor is unplugged, these values are currently simplified to -1.
- TODO: switch the mgos_msleep value between the START/SYNC command and taking the reading based on SPS


# Disclaimer

This project is not an official Google project. It is not supported by Google
and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
