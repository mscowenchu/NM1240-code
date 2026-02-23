#ifndef motor_functions_H
#define motor_functions_H


#include <stdio.h>	
#include "NM1240.h"
#include "system_initialize.h"
#include "system_parameter.h"
#include "variable_typedefine.h"
#include "PI_control.h"



//===Define for DAC Module======================================
#define DAC_chA_control 0x8000		//Input register and DAC register updated, output updated
#define DAC_chB_control 0x9000		//Input register and DAC register updated, output updated
#define DAC_chC_control 0xA000		//Input register and DAC register updated, output updated
#define DAC_chD_control 0xB000		//Input register and DAC register updated, output updated
#define DAC_PWD			0xF000
#define DAC_SCK		PE5
#define DAC_SDO		PE7
#define DAC_CS		PD6
#define	GO_BUSY0	((USPI1->PROTSTS & USPI_PROTSTS_BUSY_Msk) >> USPI_PROTSTS_BUSY_Pos)
#define SPI1_TX 	(USPI1->TXDAT)
extern uint16_t temp_data, tempA, tempB, tempC, tempD;
extern void DAC_Output_M0(void);
extern void DAC_Module_Check(void);
extern void DAC_Output_M0_1R(void);	//20180828
//=====================================================================

//===Define some GPIO pin with other name==========================================
#define LED_PIN			PF4		//For lighting LED in the demo board
#define testpin1		PC4		//For test or debug use
#define DIR_PIN			PA6		//PA6 connect to the switch of RUN, use it to set the motor direction command
//=================================================================================

//===Functions related to demo system by NM1240 ====================================
extern void PhaseCurrent_OC_Check(AMotor* Motor);

extern void Detect_Initial_Hall_Position_M0(void);

extern uint8_t Check_Rotor_FW_RW_M0(uint8_t u8_Last_Hall_Pos, uint8_t u8_Hall_Pos, uint8_t u8_Last_Direction);
extern void Detect_Initial_Hall_Position_M0(void);
extern void HallSpeedEstimator(AMotor* Motor);
extern void Fix_Phase_Angle_M0(int16_t i16_Shift_Angle);

extern void cmd_speed_slope_with_hall(AMotor* Motor);

extern void read_VSP_Check_ON_OFF_Motor(AMotor* Motor);
extern void read_UART_Check_ON_OFF_Motor(AMotor* Motor);
extern void read_VSP_to_speed_cmd(AMotor* Motor);
extern void read_VSP_to_Iq_cmd(AMotor* Motor);
extern void read_VSP_to_Vq_cmd(AMotor* Motor);
extern void read_VSP_to_duty_cmd(AMotor* Motor);


extern void delay_Xmsec(unsigned int z);	//delay time 

extern void Enable_CLKO_to_PA0(void);	//20180828

#endif
