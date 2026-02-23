/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2021/05/24 $
 * @brief    Demonstrate how to use NM1240 to control a fan motor by FOC algorithm with 3 Hall and 2 shunt_R.
 * @note
 * Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NM1240.h"
#include "system_initialize.h"
#include "motor_functions.h"
#include "protocol.h"

__IO int32_t t1, t2, t3, t4;  //For test use only.
__IO int32_t counter_flash_LED = 0;

int main()
{

/*-- Firstly, initialize the system clock of MCU -------------*/
    SYS_Init_Clock();  //Set MCU clock source from internal HIRC(60MHz)
    delay_Xmsec(200);  //Recommend to insert this delay when using ICE tool


    /*--Initialize MCU Functions which will be used in this demo system -----------------------------*/
    Initialize_FMC();               //Initialize FMC    --> Dataflash 512 bytes
    Initialize_USCI_UART();         //Initialize UART2  --> UART2_TX(PD7) & UART2_RX(PE0)
    Initialize_ECAP();              //Initialize ECAP   --> 3 Hall inputs: PF0~PF2 as IC0~IC2 to detect Hall edge
    Initialize_ADC();               //Initialize ADC    --> Eanble ADC, setup ADC and sample counter
    Initialize_OP1();               //Initialize OP1    --> Amplifying DC shunt current to I_OC
    Initialize_ACMP0(0x7FF);         //Initialize ACMP0  --> Compare I_OC (Over current), preset DAC0_value
    Initialize_Timer0();            //Initialize Timer0 --> Produce 1ms periodic interrupt for System
    Initialize_GPIO();              //Initialize GPIO   --> Setting GPIO I/O or MFP for this demo system
    Initialize_HDIV();              //Initialize HDIV   --> Enable build-in 32-bit Hardware Dividor (int32/int16)
    Initialize_Temperature_Sensor();
    
    #if P_SIX_STEP_CONTROL
        /*-- Initialize EPWM --> 6 PWM outputs: PWM output to off MOS, PWM start --*/
        Initialize_EPWM();  //Initialize EPWM  --> 6 PWM outputs: PWM output to off MOS, PWM start
    #endif

    #if P_FOC_CONTROL
        #ifdef P_2R_FOC
            /*-- Initialize EPWM --> 6 PWM outputs: PWM output to off MOS, PWM start --*/
            //Initialize_EPWM();  //Initialize EPWM  --> 6 PWM outputs: PWM output to off MOS, PWM start
        #endif

        #ifdef P_1R_FOC
            /*-- Initialize EPWM --> Set PWM as Asymmetric Mode, 6 PWM outputs: PWM output to off MOS, PWM start--*/
            //Initialize_EPWM_1R();
        #endif
    #endif
    //--------------------------------------------------------------------------------------------------------


    /*-- Set SPI1 to control the DAC module in this demo system------------------------------------*/
    /*  Force to delay a fixed time: Delay to start SPI0 because the SPI0 output(PD1/2)            */
    /*  share the same pins with Nu-Link ICE_CLK and ICE_DAT.                                      */
    #ifdef P_ENABLE_USCI1_SPI1_for_DAC
        delay_Xmsec(150);
        Initialize_USCI1_SPI1_for_DAC();  //Initialize SPI1  --> Enable SPI1 for setting external DAC
        DAC_Module_Check();
    #endif
//----------------------------------------------------------------------------------------------


    /*--- Initialize some system functions which the system needs before starting the motor. ------*/
    //  1. Initialize or check some system signals
    //  2. Check system is ready or not
    //  3. To Enable NVIC and to set priority are included in this function

    #if P_FOC_CONTROL
        #ifdef P_2R_FOC
            Initialize_Motor_System();
						/*-- Initialize EPWM --> 6 PWM outputs: PWM output to off MOS, PWM start --*/
            Initialize_EPWM();  //Initialize EPWM  --> 6 PWM outputs: PWM output to off MOS, PWM start
        #endif

        #ifdef P_1R_FOC
            Initialize_Motor_System_1R();
						/*-- Initialize EPWM --> Set PWM as Asymmetric Mode, 6 PWM outputs: PWM output to off MOS, PWM start--*/
            Initialize_EPWM_1R();
        #endif
    #endif

    #if P_SIX_STEP_CONTROL
        Initialize_Motor_System();
    #endif

    delay_Xmsec(100);
    //----------------------------------------------------------------------------------------------


    //-------------------------------------------------------------------------------------------------------
    //  Here, program starts to check the user interface to set the start/stop motor,
    //  direction and speed command in "TMR0_IRQHandler()"
    //
    //  1. Switch of "RUN": set low to command FW direction, set high to command RW direction
    //  2. Tune VR: Turn VR to start or stop the motor and the VR voltage is also the speed command
    //-------------------------------------------------------------------------------------------------------
    
    while(1)
    {
        if(MotorA.other.u8_flag_error_record != 0)
        {
            if(counter_flash_LED < 50)  //Flash error-LED for a while then resume system initial state
            {
                delay_Xmsec(70); //20180823
                LED_PIN ++;  //For test; Flash LED by PB7
                counter_flash_LED ++;  //counting how many times of flash error-LED
            }
            else
            {
                Stop_Motor_PWMOUT_OFF();  //Call it to resume system initial state
                MotorA.other.u8_flag_error_record = 0;  //Clear error record
                counter_flash_LED = 0;
            }
        }
        else
        {
            counter_flash_LED = 0;
        }
    }
}//--End of "main()"------------------------------------------------


//    LED_PIN = 1;  //for test  
//    //---Do Inv(PARK) and Inv(CLARK) and SVPWM operation to get net PWM duty0/2/4----
//    //---And Update the duty to EPWM->CMPDAT[0/2/4] in the function call------(4.28us)
//    Vdq_to_SVPWM(&MotorA, EPWM, C_PWM_FULL_SCALE, C_MAX_PWM_DUTY, 1700, 5789); //40us (20us)
//    Iuvw_to_Idq(&MotorA, sin_Q15, cos_Q15); //32us (22us)
//    LED_PIN = 0;  //for test

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
