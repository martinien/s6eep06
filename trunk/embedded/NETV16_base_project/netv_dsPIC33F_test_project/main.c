#include "def.h"
#include "NETV16_SerialDriver.h"
#include "NETV16_Device.h"
#include "NETV16_Shared.h"
#include "NETV16_Memory.h"


//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =>

//Config fuses
//Refer to p33FJ32MC304.h for the details
_FWDT(FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR128 & WDTPOST_PS32768);
_FPOR(FPWRT_PWR1 & ALTI2C_OFF & HPOL_ON & LPOL_ON & PWMPIN_ON);
_FOSCSEL(FNOSC_FRCPLL & IESO_ON);
_FOSC(FCKSM_CSDCMD & OSCIOFNC_ON  & POSCMD_EC & IOL1WAY_OFF); 
_FGS(GCP_OFF & GSS_OFF & GWRP_OFF);
_FICD(ICS_PGD1 & JTAGEN_OFF);

//Prototypes:
void init_default_variables(void);
void update_variables(void);


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Constants and variables                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

////EEPROM Test (to remove)
//int newData, readData; // New data to write to EEPROM
//unsigned int newOffset;
//int table[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                        Main function                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{	
	unsigned char canAddr = 0;
	BootConfig *bootConfig = NULL;

	//Initial configuration
	setup_oscillator();
	config();
	
//	//EEPROM Test (to remove)
//	while(1)
//	{
//		newData += 1;
//		newOffset = 0;
//		//ee_word_write(newOffset, newData);
//		WriteMem(newOffset, table, 8);
//		WriteMem(0x10,&newData,1);
//		readData = ReadMem(0x10);
//	}

    //reading boot config and device configuration
    //MUST BE DONE BEFORE INITIALIZING NETV MODULE
    bootConfig = netv_get_boot_config();
    //dsPIC33F w/o eeprom:
    bootConfig->table_version = MODULE_TABLE_VERSION;
	bootConfig->project_id = MODULE_PROJECT_ID;
	bootConfig->code_version = MODULE_CODE_VERSION;
	//Set to default address
	bootConfig->module_id = 1;

    if (bootConfig)
    {
            //read configuration
            netv_read_boot_config(bootConfig);

            //safety
            bootConfig->module_state = BOOT_NORMAL;

            //verify if we have correct configuration
            //write it back if not updated
            if (bootConfig->table_version !=  MODULE_TABLE_VERSION
            || bootConfig->project_id != MODULE_PROJECT_ID
            || bootConfig->code_version != MODULE_CODE_VERSION)
            {
                    bootConfig->table_version = MODULE_TABLE_VERSION;
                    bootConfig->project_id = MODULE_PROJECT_ID;
                    bootConfig->code_version = MODULE_CODE_VERSION;

                    //Set to default address
                    bootConfig->module_id = 1;

                    //Writing back the boot config for the next version
         //           netv_write_boot_config(bootConfig);					//Not used now (no eeprom)

                    //set variables to zero
                    init_default_variables();
            }
    }

    //UPDATE NETV ADDRESS
   	canAddr = bootConfig->module_id;

	//Main loop
	while (1)
	{
        //Right now will never come out of this function (blocking on serial port)
        netv_transceiver(canAddr);
		update_variables();
	}

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                           Fonctions                                      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void setup_oscillator(void)
{
	//Internal 7.37MHz oscillator with PLL
	//36.85MIPS (27.1ns/inst.)
	
	//PLL Predivisor = 2 (N1 = 2)
	CLKDIVbits.PLLPRE4 = 0;
	CLKDIVbits.PLLPRE3 = 0;
	CLKDIVbits.PLLPRE2 = 0;
	CLKDIVbits.PLLPRE1 = 0;
	CLKDIVbits.PLLPRE0 = 0;
	// M = 40
	PLLFBD = 38;
	//N2 = 2
	CLKDIVbits.PLLPOST1 = 0;
	CLKDIVbits.PLLPOST0 = 0;	
	
	// the CPU will automatically switch when all is stable....
	while(!OSCCONbits.LOCK);  // Wait for PLL to lock
}

void config(void)
{
	//Define inputs/outputs
	define_io();		
	
	//Config peripherals:
	setup_usart1(); 		//PC interface (NetworkViewer)	
}

void netv_proc_message(NETV_MESSAGE *message)
{
    //Handle custom messages...
}

void init_default_variables(void)
{
    memset(&g_globalNETVVariables, 0, sizeof(GlobalNETVVariables));
}

void update_variables(void)
{
	g_globalNETVVariables.Count++;	//Sawtooth waveform

}
