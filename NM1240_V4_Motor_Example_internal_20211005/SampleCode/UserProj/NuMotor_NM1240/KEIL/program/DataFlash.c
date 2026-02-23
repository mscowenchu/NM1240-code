#include "NM1240.h"
#include "DataFlash.h"
#include "variable_typedefine.h"

/*
	=========DataFlash description=================================================================
	1. When the system is operating, enable data flash to storage system variables.
	2. Read data flash and determine if there is data stored in the data flash.
	3. If data flash is not empty, the initial data will be loaded from data flash.
	4. If data flash is empty, the initial data will be loaded from MCU.
	4. When MOTOR is stopped, users can use GUI to write the data to the data flash, read the data from the data flash or erase the data in the data flash.
	5. Users can define the variables to be written to the data flash by update_data_flash_buff_from_variable().
	6. Users can determine the default value of the variable, update_variable_from_data_flash_buff() is usually the same as update_data_flash_buff_from_variable().
	===============================================================================================
*/

uint32_t	au32Config[2]; //Store the contents of the config register

uint32_t	data_flash_buff[FLASH_DATA_AMOUNT] = { 0 }; //Temporary storage of data array

uint8_t		data_flash_empty_flag = 0; //If data flash is not empty, data_flash_empty_flag = 0
																			//If data flash is empty, data_flash_empty_flag = 1

//==Initialize_FMC()==========================================================
//1. Enable FMC ISP & CONFIG Update function.
//2. Enable Data flash.
//3. Set Data Flash Base Address.
void Initialize_FMC()
{
	/* Enable FMC ISP function */
    /* FMC_Open() */
	SYS_UnlockReg();
	FMC->ISPCTL |=  FMC_ISPCTL_ISPEN_Msk;	// ISP function Enabled.
	FMC->ISPCTL |=  FMC_ISPCTL_CFGUEN_Msk;  // ISP update User Configuration Enabled.

	au32Config[0] = FMC_Read(FMC_CONFIG_BASE);   //Read content of CONFIG 0
	au32Config[1] = FMC_Read(FMC_CONFIG_BASE + 4); //Read content of CONFIG 1

	if((au32Config[0] == 0xFFFFFFFF) && (au32Config[1] == 0xFFFFFFFF)) //Check CONFIG 0 & 1 , if not set , write set value
	{
		FMC_Erase(FMC_CONFIG_BASE);
		FMC_Write(FMC_CONFIG_BASE, 0xFFFFFFFE ); //Write setting to CONFIG 0 ¡A set DEFN = 0 (enable data flash)
//		au32Config[0] = FMC_Read(FMC_CONFIG_BASE);

		FMC_Erase(FMC_CONFIG_BASE + 4);
		FMC_Write(FMC_CONFIG_BASE + 4, DATA_FLASH_BASE);	//Write setting to CONFIG 1 ¡A set Data Flash Base Address
//		au32Config[1] = FMC_Read(FMC_CONFIG_BASE+4);
	}
	
	if((au32Config[0] != 0xFFFFFFFE ) && (au32Config[1] != DATA_FLASH_BASE)) //Check CONFIG 0 & 1 ,if it does not meet the set value , modify the set value
	{
		FMC_Erase(FMC_CONFIG_BASE);
		FMC_Write(FMC_CONFIG_BASE, 0xFFFFFFFE);
//		au32Config[0] = FMC_Read(FMC_CONFIG_BASE); //Write setting to CONFIG 0 ¡A set DEFN = 0 (enable data flash)
		
		FMC_Erase(FMC_CONFIG_BASE + 4);
		FMC_Write(FMC_CONFIG_BASE + 4, DATA_FLASH_BASE);	//Write setting to CONFIG 1 ¡A set Data Flash Base Address
//		au32Config[1] = FMC_Read(FMC_CONFIG_BASE+4);
	}
	
	SYS_LockReg();
	
	//Check the data flash whether is empty. If the data flash is not empty, the initial data will be loaded from the data flash.
	//read_data_flash_to_buff();
	//if( data_flash_empty_flag == 0 )
	//{
	//	update_variable_from_data_flash_buff();
	//}
}
//==end of Initialize_FMC()=======================================================




//==read_data_flash_to_buff()=================================================================
//1. Read the contents of the data flash and store it in the data_flash_buff[].
//2. Check the data flash whether is empty. If data flash is empty, data_flash_empty_flag = 1. 
void read_data_flash_to_buff()
{
	uint8_t i, data_flash_empty_amount = 0;
	
	SYS_UnlockReg();
	FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;
	
	for(i = 0; i < FLASH_DATA_AMOUNT; i++)
	{
		data_flash_buff[i] = FMC_Read(DATA_FLASH_BASE + i * 4);
		if(data_flash_buff[i] == 0xFFFFFFFF) data_flash_empty_amount++;	
	}
	
	if(data_flash_empty_amount == FLASH_DATA_AMOUNT) data_flash_empty_flag = 1;
	else data_flash_empty_flag = 0;
	
	FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
	SYS_LockReg();

}
//==end of read_data_flash_to_buff()==========================================================


//==write_data_flash_from_buff()=================================================================
//1. Variables will be moved to the data_flash_buff[].
//2. The data_flash_buff[] will be written to data flash.
void write_data_flash_from_buff()
{
	uint8_t i;
	
	SYS_UnlockReg();
	FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;

	FMC_Erase(DATA_FLASH_BASE);

	for(i = 0; i < FLASH_DATA_AMOUNT; i++)
	{
		FMC_Write(DATA_FLASH_BASE + i * 4, data_flash_buff[i]);
	}

	FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
	SYS_LockReg();

}
//==end of write_data_flash_from_buff()==========================================================

//==update_data_flash_buff_from_variable()=============================================================================
//1. Variables will be moved to the data flash buff.
//2. The value of data flash buff will be written to the data flash by write_data_flash_from_buff().
//3. Users can define the variables to be written to the data flash.
//4. The amount of data flash is defined by FLASH_DATA_AMOUNT.
//5. The size of data flash is defined by DATA_FLASH_SIZE. Maximum: 29.5 Kbytes
void update_data_flash_buff_from_variable()
{
	data_flash_buff[0]	= PI_Speed.kp_15;
	data_flash_buff[1]	= PI_Speed.ki_15;
	data_flash_buff[2]	= PI_Iq.kp_15;
	data_flash_buff[3]	= PI_Iq.ki_15;
	data_flash_buff[4]	= PI_Id.kp_15;
	data_flash_buff[5]	= PI_Id.ki_15;
	data_flash_buff[6]	= 0xFFFFFFFF;
	data_flash_buff[7]	= 0xFFFFFFFF;
	data_flash_buff[8]	= 0xFFFFFFFF;
	data_flash_buff[9]	= 0xFFFFFFFF;
	data_flash_buff[10] = 0xFFFFFFFF;
	data_flash_buff[11] = 0xFFFFFFFF;
	data_flash_buff[12] = 0xFFFFFFFF;
	data_flash_buff[13] = 0xFFFFFFFF;
	data_flash_buff[14] = 0xFFFFFFFF;
}
//==end of update_data_flash_buff_from_variable()======================================================================

//==update_variable_from_data_flash_buff()=============================================================================
//1. The data flash buff will be moved to variables.
//2. If the data flash is not empty, the initial data will be loaded from data flash when MCU is initializing.
//3. Users can determine the default value of the variable, usually the same as update_data_flash_buff_from_variable().
void update_variable_from_data_flash_buff()
{
	PI_Speed.kp_15													= data_flash_buff[0];
	PI_Speed.ki_15													= data_flash_buff[1];
	PI_Iq.kp_15															= data_flash_buff[2];
	PI_Iq.ki_15															= data_flash_buff[3];
	PI_Id.kp_15															= data_flash_buff[4];
	PI_Id.ki_15															= data_flash_buff[5];
}
//==end of pdate_variable_from_data_flash_buff()=======================================================================

