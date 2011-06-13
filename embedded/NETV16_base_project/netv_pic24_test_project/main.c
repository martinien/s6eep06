#include "def.h"
#include "NETV16_SerialDriver.h"
#include "NETV16_Device.h"
#include "NETV16_Shared.h"
#include "NETV16_Memory.h"


//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =>

//Config fuses
//Refer to p24f16ka101.h for the details
_FBS(BSS_OFF & BWRP_OFF);
_FGS(GCP_OFF & GWRP_OFF);
_FOSCSEL(FNOSC_FRCPLL & IESO_OFF);
_FOSC(FCKSM_CSDCMD & SOSCSEL_SOSCLP & POSCFREQ_HS & OSCIOFNC_ON & POSCMOD_NONE);
_FWDT(FWDTEN_OFF & WINDIS_OFF & WDTPS_PS32768 & FWPSA_PR128);
_FPOR(MCLRE_ON & BORV_V18 & I2C1SEL_PRI & PWRTEN_ON & BOREN_BOR0);
_FICD(BKBUG_ON & ICS_PGx2);
_FDS(DSWDTEN_OFF & DSBOREN_OFF & RTCOSC_LPRC & DSWDTOSC_SOSC & DSWDTPS_DSWDTPS6);

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
                    netv_write_boot_config(bootConfig);

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
	//Internal 8MHz oscillator with 4xPLL
	//16MIPS (62.5ns/inst.)
	OSCCONbits.COSC = 1;	//8MHz FRC w/PLL
	OSCCONbits.NOSC = 1;	//8MHz FRC w/PLL
	
	CLKDIVbits.DOZE = 0;	//CPU clock = peripheral clock (1:1)
	CLKDIVbits.RCDIV = 0;	//No division
	
	// the CPU will automatically switch when all is stable....
	while(!OSCCONbits.LOCK);  // Wait for PLL to lock
}

void config(void)
{
	//Disable ADC (see Errata, bug between RA2 and RB2...)
	AD1CON1bits.ADON = 0;
	AD1PCFG = 0xFFFF;		//All digital IO
	
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
	g_globalNETVVariables.Count++;
}
