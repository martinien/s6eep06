#ifndef _BSP_TEST_H_
#define _BSP_TEST_H_

#include <p24f16ka101.h>

#define FCY 30000000
#include "NETV16_Shared_test.h"

#define USE_EEPROM	//Comment for dsPIC33F

#define DEVID_BASE_ADDRESS 0x3FFFFE

//DEVICE MEMORY MAP INFORMATION	
#define EEPROM_BASE_ADDRESS 0x7FFE00
#define EEPROM_BASE_ADDRESS_LOW 0xFFFF
#define EEPROM_BASE_ADDRESS_HIGH 0x007F	

#endif
