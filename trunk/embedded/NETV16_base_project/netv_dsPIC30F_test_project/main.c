#include "NETV16_CANDriver.h"
#include "NETV16_Device.h"
#include "NETV16_Shared.h"
#include "NETV16_Memory.h"
#include <string.h>

//Prototypes:
void init_default_variables(void);
void update_variables(void);


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                        Main function                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{	
	NETV_MASK mask_in[2];
	NETV_FILTER filter_in[6];
	unsigned char canAddr = 0;
	BootConfig *bootConfig = NULL;
	unsigned int i = 0;

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


    //init mask
	for(i=0;i<2;i++){
		mask_in[i].mask_priority = 0;
		mask_in[i].mask_type = 0;
		mask_in[i].mask_cmd = 0;
		mask_in[i].mask_dest = 0xFF;
	}
	
	//init filter
	for(i=0;i<6;i++){
		filter_in[i].filter_priority = 0;
		filter_in[i].filter_type = 0;
		filter_in[i].filter_cmd = 0;
		filter_in[i].filter_dest = canAddr;
	}

	//init can
	netv_init_can_driver(filter_in,mask_in);


	//Main loop
	while (1)
	{
        //Right now will never come out of this function (blocking on serial port)
        netv_transceiver(canAddr);

		//Update shared variables
		update_variables();
	}

    return 0;
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

}
