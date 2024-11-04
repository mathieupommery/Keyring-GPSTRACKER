/*
 ***************************************************************************************************
 * This file is part of Sensors SDK:
 * https://www.we-online.com/sensors, https://github.com/WurthElektronik/Sensors-SDK_STM32
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE (license_terms_wsen_sdk.pdf)
 * LOCATED IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief WSEN-PADS example.
 *
 * Demonstrates basic usage of the PADS absolute pressure sensor connected via I2C.
 */

#include "WSEN_PADS_EXAMPLE.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i2c.h"

#include <platform.h>

#include "WSEN_PADS_2511020213301.h"

/* Comment/uncomment the following lines to disable/enable the examples for
 * each data type (see WE_padsExampleLoop() function). */
/* Note: The float example won't compile unless WE_USE_FLOAT is defined. */
#define PADS_EXAMPLE_ENABLE_FLOAT
#define PADS_EXAMPLE_ENABLE_INT

/* Sensor interface configuration */
WE_sensorInterface_t pads;

/* Sensor initialization function */

void PADS_init(void)
{
  /* Initialize sensor interface (i2c with PADS address, burst mode deactivated) */
  PADS_getDefaultInterface(&pads);
  pads.interfaceType = WE_i2c;
  pads.handle = &hi2c1;

  /* Wait for boot */
  HAL_Delay(50);
  while (WE_SUCCESS != WE_isSensorInterfaceReady(&pads))
  {
  }
  HAL_Delay(5);

  uint8_t deviceIdValue = 0;
  if (WE_SUCCESS == PADS_getDeviceID(&pads, &deviceIdValue))
  {
    if (deviceIdValue == PADS_DEVICE_ID_VALUE)
    {
    }
    else
    {
    }
  }
  else
  {
  }

  PADS_softReset(&pads, PADS_enable);
  PADS_state_t swReset;
  do
  {
    PADS_getSoftResetState(&pads, &swReset);
  } while (swReset);

}


float altitudecalc(int pression){

	return (float)(288.15/0.0065)*(1-pow((pression/101325),0.1903));



}

