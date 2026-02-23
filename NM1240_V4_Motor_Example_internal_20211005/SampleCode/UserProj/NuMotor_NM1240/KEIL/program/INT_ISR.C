#include <stdio.h>
#include "NM1240.h"
#include "system_initialize.h"
#include "system_parameter.h"
#include "variable_typedefine.h"
#include "motor_functions.h"
#include "PI_control.h"
#include "motor_FOC.h"
#include "motor_six_step.h"
#include "protocol.h"

//#define P_VQ_OPEN_LOOP_TEST      //For test, only loop Vq to SVPWM
//#define P_NO_MOTOER_OBSERVE_DAC    //For test, only connect Hall, no Motor, to observe some variables by DAC
#define C_Vq_OPEN_LOOP_TEST  4000  //For test, Value of Vq for VQ_OPEN_LOOP_TEST use


#define C_1ms_CNT_VALUE    1  //Timer0 INT one time per 1ms
#define C_10ms_CNT_VALUE  10
#define C_50ms_CNT_VALUE  50

//20180903
extern uint32_t ui32_duty0, ui32_duty2, ui32_duty4;
int16_t temp_vq = C_Vq_OPEN_LOOP_TEST * 2;

//==============TMR0_IRQHandler============================================================
#if P_FOC_CONTROL //if FOC control = 1,select this TMR0_IRQ setting
//***************************************
//Timer0 1ms interrupt subroutine for FOC contorl
//***************************************
void TMR0_IRQHandler(void)
{
    int32_t fw_item;
  
    /* Clear flags of TIF */
    TIMER0->INTSTS = TIMER_INTSTS_TIF_Msk;
  
    u16_timer0_int_counter = u16_timer0_int_counter + 1;

//---1ms-----------------------------------------
    if(u16_timer0_int_counter >= C_1ms_CNT_VALUE)  //1ms
    {
        u16_1ms_timer_counter ++;
        u16_timer0_int_counter -= C_1ms_CNT_VALUE;  
        u8_flag_1ms = 1;
    
        /*-- Update the cmd.i16_rotor_speed to follow C_SLOPE_rpm_Per_Sec (slope of speed command +/-) -----*/
        if(MotorA.cmd.u8_start == 0)
        {//--If motor has not started yet--
            Stop_Motor_PWMOUT_OFF();
        }
        else
        {//--If motor has started -----
            cmd_speed_slope_with_hall(&MotorA);  //Set the speed command follows the speed slope
        }

        /*-- Do Speed PI control every 1ms -----------------------------------------------------*/
        fw_item = 0;
        MotorA.info.i16_speed_error = MotorA.cmd.i16_rotor_speed - MotorA.info.i16_rotor_speed;
        MotorA.cmd.i16_Iq = PI_speed(&MotorA, &PI_Speed, fw_item);
        MotorA.cmd.i16_Id = 0;  //Set Id command = 0, Iq command is the output of speed controller---
        //----------------------------------------------------------------------------------------
    }//---end of 1ms loop---------

//---10ms------------------------------------------
    if(u16_1ms_timer_counter >= 10)              //10ms
    {
        u16_10ms_time_counter ++;
        u16_1ms_timer_counter -= 10;
        u8_flag_10ms = 1;

      /*-------------------------------------------------------------------------------------
      Read Command Input (In this demo system, the voltage of VR presents the command input)
      1. Read Direction Command from DIR_PIN(PA6) is wired to the switch of "RUN"
         FW: direction=1; Forward running
         RW: direction=0; Reversed running
      2. Read VAR voltage to decide Motor ON or OFF and Speed Command
      3. Set Id command as zero (User may set Id command as system required)
      --------------------------------------------------------------------------------------*/
        
        if((ACMP->STATUS & ACMP_STATUS_ACMPO0_Msk) == 0)  //Check if not Over_Current
        {
            if(u8_flag_VSP_control == 1)
            {
                if(MotorA.cmd.u8_start == 0)
                    MotorA.cmd.u8_direction = DIR_PIN;

                read_VSP_Check_ON_OFF_Motor(&MotorA);  //check if VSP signal is over the turn-on level
                read_VSP_to_speed_cmd(&MotorA);  //Convert the VSP voltage to rotor speed command
            }

            else if(u8_flag_UART_control == 1)
            {
                Command_update();
                read_UART_Check_ON_OFF_Motor(&MotorA);

                if(((MotorA.cmd.u8_direction == 0) && (MotorA.cmd.i16_rotor_speed_target > 0)) || ((MotorA.cmd.u8_direction == 1) && (MotorA.cmd.i16_rotor_speed_target < 0))) {
                    MotorA.cmd.i16_rotor_speed_target = -MotorA.cmd.i16_rotor_speed_target;
                }
            }
        }
    }

//---100ms------------------------------------------
    if(u16_10ms_time_counter >= 10)              //100ms
    {
        u16_100ms_time_counter ++;  
        u16_10ms_time_counter -= 10;
        u8_flag_100ms = 1;
        
    }

//---500ms------------------------------------------
    if(u16_100ms_time_counter >= 5)              //500ms
    {
        u16_500ms_time_counter ++;  
        u16_100ms_time_counter -= 5;
        u8_flag_500ms = 1;

        if(MotorA.other.u8_flag_error_record == 0)
        {
            LED_PIN ++;  //For test; Flash LED by PD4
        }
    }
}


#endif 
//===End of "TMR0_IRQHandler" for FOC======================================================


#if P_SIX_STEP_CONTROL //if P_SIX_STEP_CONTROL = 1,select this TMR_IRQ setting
//***************************************
//Timer0 1ms interrupt subroutine for Six_step contorl
//Two control modes in TMR0_IRQ , VSP & UART , and do someting different
//1. In UART control : 
//    a. update EPWM->CMPDAT[0] from MotorA.cmd.i16_Duty0 in P_SIX_STEP_OPEN_LOOP per 1ms
//    b. update CMD & INFO data per 10ms
//2. In VSP control :
//    a. Do Speed PI control and update EPWM->CMPDAT[0] in P_SIX_STEP_CLOSE_LOOP per 1ms
//    b. read duty cmd from VSP , in P_SIX_STEP_OPEN_LOOP per 10ms
//    c. read speed cmd from VSP , in P_SIX_STEP_CLOSE_LOOP per 10ms
//***************************************
void TMR0_IRQHandler(void)
{
    int32_t fw_item;
    int Close_loop_duty;
    uint32_t dDuty,cmd_duty;
  
    /* Clear flags of TIF */
    TIMER0->INTSTS = TIMER_INTSTS_TIF_Msk;  
  
    u16_timer0_int_counter = u16_timer0_int_counter + 1;

//---1ms-----------------------------------------  
    if(u16_timer0_int_counter >= C_1ms_CNT_VALUE)      //1ms
    {
        u16_1ms_timer_counter ++;
        u16_timer0_int_counter -= C_1ms_CNT_VALUE;  
        u8_flag_1ms = 1;
        /* Disable EADC INTERRUPT */
        ADC_DISABLE_INT(ADC, ADC_CTL_ADC0IEN_Msk);  //Before S/W ADC is active, Disable ADC0 to inhibit the EADC0 Interrupt triggered by S/W ADC

        /* SW read Idc current from OP1_O */
        MotorA.info.i16_IDC_Bus = SW_Get_I_BUS_From_Ishunt();  //SW_Trg_ADC1_PGA_ADC_Return_Reslut()  /* Define a readable name for I_shunt */
        
        /* Enable EADC INTERRUPT */
        ADC_ENABLE_INT(ADC, ADC_CTL_ADC0IEN_Msk);  //After S/W ADC is finished, Re-Enable EADC0 Interrupt

        //-- After SW reading, set ADC to the mode for measuring Iu and Iv by PWM trigger.
        Set_HW_ADC_Read_IuIv_by_Simu_Mode();  //Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period()
        
        /*-- Update the cmd.i16_rotor_speed to follow C_SLOPE_rpm_Per_Sec (slope of speed command +/-) -----*/
        if(MotorA.other.u8_CMD_Motor_Action != C_CMD_RUN)
        {//--If motor has not started yet--
            Stop_Motor_PWMOUT_OFF();
        }
        else
        {//--If motor has started -----
            cmd_speed_slope_with_hall(&MotorA);    //Set the speed command follows the speed slope
        }
    
    
        #if P_SIX_STEP_OPEN_LOOP
        /* If system controlled by UART,read Duty cmd by MotorA.cmd.i16_Duty0 */
        if(u8_flag_UART_control==1)
        {
            cmd_duty = MotorA.cmd.i16_Duty0*C_PWM_FULL_SCALE/100;
                        
            if(MotorA.cmd.u8_start == 1)
            {   
                if(MotorA.other.u8_flag_error_record != 0) //if system error , set EPWM->CMPDAT[0]=0;
                {
                    EPWM->CMPDAT[0]=0;
                    u16_Duty_UART = 0;
                }
                    
                if(EPWM->CMPDAT[0]<cmd_duty) //before PWM duty achieve duty command,PWM_duty=PWM_duty+ dDuty
                {
                    dDuty=dDuty_UART_1ms;
                    u16_Duty_UART += dDuty;
                    if(u16_Duty_UART >= cmd_duty)
                        u16_Duty_UART = cmd_duty;
                    EPWM->CMPDAT[0] = u16_Duty_UART ;
                }
                else 
                {
                    EPWM->CMPDAT[0] = cmd_duty ;
                }
            }
            else
            {
                EPWM->CMPDAT[0]=0;
                u16_Duty_UART = 0;
            }
        }

        #elif P_SIX_STEP_CLOSE_LOOP
        /*-- Do Speed PI control every 1ms -----------------------------------------------------*/
        //Speed close loop control
        fw_item = 0;

        MotorA.info.i16_speed_error = MotorA.cmd.i16_rotor_speed - MotorA.info.i16_rotor_speed;
        Close_loop_duty = PI_speed_six_step(&MotorA, &PI_Speed, fw_item) ; //PI controller are Q15 form
        Close_loop_duty = (Close_loop_duty * C_PWM_FULL_SCALE) >> 15 ; //Q15(0 ~ 32767) to (0 ~ C_PWM_FULL_SCALE)
        if(MotorA.cmd.u8_direction==0)
            Close_loop_duty = -Close_loop_duty; // if direction = 0，Close_loop_duty are Negative value，Convert to positive value

        if(Close_loop_duty > C_PWM_FULL_SCALE)
            Close_loop_duty = C_PWM_FULL_SCALE;
        else if(Close_loop_duty < 0)
            Close_loop_duty = 0;  
        EPWM->CMPDAT[0] = Close_loop_duty ;
            
        #endif
        //----------------------------------------------------------------------------------------
    
  }//---end of 1ms loop---------

  
//---10ms------------------------------------------
    if(u16_1ms_timer_counter >= 10)              //10ms
    {
        u16_10ms_time_counter ++;  
        u16_1ms_timer_counter -= 10;
        u8_flag_10ms = 1;


        /*-------------------------------------------------------------------------------------
        Read Command Input (In this demo system, the voltage of VR presents the command input)
        1. Read Direction Command from DIR_PIN(PC4) is wired to the switch of "RUN"
        FW: direction=1; Forward running
        RW: direction=0; Reversed running
        2. Read VAR voltage to decide Motor ON or OFF and Speed Command  
        3. If system controlled by UART , update CMD & INFO data per 10ms

    --------------------------------------------------------------------------------------*/

        //When system controlled by UART，update CMD & INFO data per 10ms
        if(u8_flag_UART_control==1)
        {
            Command_update();
            read_UART_Check_ON_OFF_Motor(&MotorA);
            
            if(((MotorA.cmd.u8_direction == 0) && (MotorA.cmd.i16_rotor_speed_target > 0)) || ((MotorA.cmd.u8_direction == 1) && (MotorA.cmd.i16_rotor_speed_target < 0))) {
                    MotorA.cmd.i16_rotor_speed_target = -MotorA.cmd.i16_rotor_speed_target;
            }
        }
        //--When system controlled by VSP , SW read VSP voltage for motor speed command------
        else if(u8_flag_VSP_control == 1)
        {
            if(MotorA.cmd.u8_start == 0)
                MotorA.cmd.u8_direction = DIR_PIN;

            /* Disable EADC INTERRUPT */
            ADC_DISABLE_INT(ADC, ADC_CTL_ADC0IEN_Msk);  //Before S/W ADC is active, Disable ADC0 to inhibit the EADC0 Interrupt triggered by S/W ADC

            MotorA.other.ui16_VSP_12bit_ADC = SW_Get_Speed_Command_From_VR();  //ADC1_SW_Read(ADC1_ADC1_P7_DAT)      /* Define a readable name for speed command */
            
            /* Enable EADC INTERRUPT */
            ADC_ENABLE_INT(ADC, ADC_CTL_ADC0IEN_Msk);  //After S/W ADC is finished, Re-Enable EADC0 Interrupt

            //-- After SW reading, set ADC to the mode for measuring Iu and Iv by PWM trigger.
            Set_HW_ADC_Read_IuIv_by_Simu_Mode();  //Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period()
      
            read_VSP_Check_ON_OFF_Motor(&MotorA);  //check if VSP signal is over the turn-on level
      
            #if P_SIX_STEP_OPEN_LOOP
                    
            read_VSP_to_duty_cmd(&MotorA);  //Convert the VSP voltage to rotor duty command , P_SIX_STEP_OPEN_LOOP ; Add by Mars
            
            #endif
          
            #if P_SIX_STEP_CLOSE_LOOP
                  
            read_VSP_to_speed_cmd(&MotorA);  //Convert the VSP voltage to rotor speed command , P_SIX_STEP_CLOSE_LOOP ; Add by Mars

            #endif    
        }
  
    }

//---100ms------------------------------------------
    if(u16_10ms_time_counter >= 10)              //100ms
    {
        u16_100ms_time_counter ++;  
        u16_10ms_time_counter -= 10;
        u8_flag_100ms = 1;

    }


//---500ms------------------------------------------
    if(u16_100ms_time_counter >= 5)              //500ms
    {
        u16_500ms_time_counter ++;  
        u16_100ms_time_counter -= 5;
        u8_flag_500ms = 1;
    
        if(MotorA.other.u8_flag_error_record == 0)
        {
            LED_PIN ++;  //For test; Flash LED by PD4
        }
    }
}
#endif
//===End of "TMR0_IRQHandler" for for P_SIX_STEP_CONTROL=====================================


//==============ADC0_IRQHandler===========================================================
#ifdef P_2R_FOC
#if P_FOC_CONTROL //if FOC control = 1,select this ADC0_IRQ setting

//---ADC0_IRQHandler()-----(ADC is triggered by PWM period for 2-shunt R)-------------------
// The ADC0 Interrupt will be triggered by periodic PWM center-time
// When the ADC0_Int is requested it means the Iu and Iv sensing data is ready in ADC->DAT0
// 0. Clear the flag ADC0IF.
// 1. Get the Iu and Iv from EAD->DAT[0] and re-construct Iu/Iv/Iw
// 2. Do Clark and Park operation to get info.Id and info.Iq
// 3. Do current PI control to get cmd.Vd and cmd.Vq
// 4. Do Invserse_Park, Inverse_Clark and SVPWM operation to get new PWM duty0/2/4
// 5. Update the new i16_hall_angle for the next ADC0_Int. 
//--------------------------------------------------------------------------


void ADC0_IRQHandler(void)
{
    int32_t sin_Q15, cos_Q15;
    int32_t fw_item;
    int32_t Hall_Accumulated_Theda_Q26, Hall_Step_Theda_Q26;

    /* Clear flag(ADC0IF) after conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* RE-CONSTRUCT 3-phase current Iu/v/w and setup sin/cos data */
    Re_Constuct_3_Phase_Current_2R();

    /*--Check Phase Current Over-current or not --*/
    //PhaseCurrent_OC_Check(&MotorA);  // YJS: temporarily marked
    //---End of Re-construct 3-phase current------------------


    /*------For TEST ONLY --------------------------------------*/
    /*
    #ifdef P_NO_MOTOER_OBSERVE_DAC
        cmd_speed_slope_with_hall(&MotorA);
        MotorA.cmd.i16_angle = MotorA.info.i16_hall_angle;
    #endif
    */
    //------------------------------------------------------------  


    //---DO FOC CURRENT CONTROL-----------------------------------------------
    
    //--Look up SIN Table according with hall_angle ----
    MotorA.cmd.i16_angle = MotorA.info.i16_hall_angle;  //Update the cmd.i16_angle
    sin_Q15 = SIN(MotorA.cmd.i16_angle);  //Look-up SIN table
    cos_Q15 = COS(MotorA.cmd.i16_angle);  //Look-up COS table
    MotorA.info.i32_sin_Q15 = sin_Q15;
    MotorA.info.i32_cos_Q15 = cos_Q15;

    /* Do CLARK and PARK to transfer Iu/v/w to Id/q (1.09us)*/
    Iuvw_to_Idq(&MotorA, sin_Q15, cos_Q15);


    //--Do Id and Iq CURRENT PI CONTROL----(Id + Iq : 2.85us)--
    //  MotorA.cmd.i16_Id = 0;    //Set Id command = 0, Iq command is the output of speed control---
    fw_item = 0;
    MotorA.info.i16_Id_error = MotorA.cmd.i16_Id - MotorA.info.i16_Id;
    MotorA.cmd.i16_Vd = PI_Id_current(&MotorA, &PI_Id, fw_item);

    fw_item = 0;
    MotorA.info.i16_Iq_error = MotorA.cmd.i16_Iq - MotorA.info.i16_Iq;
    MotorA.cmd.i16_Vq = PI_Iq_current(&MotorA, &PI_Iq, fw_item);


    //---For Current Open Loop Test Use------------------
    #ifdef P_VQ_OPEN_LOOP_TEST
    MotorA.cmd.i16_Vq = C_Vq_OPEN_LOOP_TEST * 2 ; //2000; //5000; //3000;
    MotorA.cmd.i16_Vd = -MotorA.other.ui16_VSP_12bit_ADC >> 2;  //More positive Vd less effeciency
    //MotorA.cmd.i16_Vd = 0;
    if(MotorA.cmd.u8_direction == 0)     
    {
        MotorA.cmd.i16_Vq = -MotorA.cmd.i16_Vq;
    }
    sin_Q15 = SIN(MotorA.cmd.i16_angle);
    cos_Q15 = COS(MotorA.cmd.i16_angle);
    MotorA.info.i32_sin_Q15 = sin_Q15;
    MotorA.info.i32_cos_Q15 = cos_Q15;
    #endif 
    //---End of Current Open Loop Test-------------------

    //---Do Inv(PARK) and Inv(CLARK) and SVPWM operation to get net PWM duty0/2/4----
    //---And Update the duty to EPWM->CMPDAT[0/2/4] in the function call------(5.12~6.24us)

    Vdq_to_SVPWM_2R(&MotorA, EPWM, C_PWM_FULL_SCALE, C_MAX_PWM_DUTY, sin_Q15, cos_Q15);

    //---End of FOC CURRENT CONTROL-------------------------------------------------------


    //---UPDATE the new hall angle in every PWM period according with the speed-------------
    Hall_Accumulated_Theda_Q26 = MotorA.other.i32_Hall_Accumulated_Theda_Q26;
    Hall_Step_Theda_Q26 = MotorA.other.i32_Hall_Step_Theda_Q26;

    Update_Hall_Angle(Hall_Accumulated_Theda_Q26, Hall_Step_Theda_Q26);


    /*-- For TEST ONLY --*/
    #ifdef P_NO_MOTOER_OBSERVE_DAC
    For_TEST_ONLY_Update_Hall_Angle(Hall_Accumulated_Theda_Q26, Hall_Step_Theda_Q26);
    #endif  
    //---End of Update new hall angle---(till here, total 10.8~11.9us)----------------------------

    //--Software read some analog signals by ADC -----------------------------------------------------------
    //1. Set ADC as SW trigger mode and read the data
    //2. After SW read ADC channels, it must set ADC as HW trigger by PWM0 for auto-converting phase current
    //------------------------------------------------------------------------------------------------------
    /* Disable EADC INTERRUPT */
    ADC_DISABLE_INT(ADC, ADC_CTL_ADC0IEN_Msk);  //Before S/W ADC is active, Disable ADC0 to inhibit the EADC0 Interrupt triggered by S/W ADC

    //--SW read VSP voltage for motor speed command------(3.05us)
    MotorA.other.ui16_VSP_12bit_ADC = SW_Get_Speed_Command_From_VR();  //ADC1_SW_Read(ADC1_ADC1_P7_DAT)  /* Define a readable name for speed command */

    //--SW read Idc current from ADC0_P4----(2.81us)
    MotorA.info.i16_IDC_Bus = SW_Get_I_BUS_From_Ishunt();  //SW_Trg_ADC0_P4_Return_Reslut()  /* Define a readable name for I_shunt */

    /* Enable EADC INTERRUPT */
    ADC_ENABLE_INT(ADC, ADC_CTL_ADC0IEN_Msk);  //After S/W ADC is finished, Re-Enable EADC0 Interrupt


    //-- After SW reading, set ADC to the mode for measuring Iu and Iv by PWM trigger.
    //--Set ADC is HW triggered by PWM after other SW read Idc---(1.19us)
    Set_HW_ADC_Read_IuIv_by_Simu_Mode();  //Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period()
    //--------------------------------------------------------------------------
//--(till here, total 18~19.1us)------------------------------------------------
    /* Transfer data to PC with UART */
    TX_data_to_PC(MotorA.info.i16_rotor_speed, C_MIN_CMD_SPEED_rpm);

    //--Debug use--------------------------------------------  
    #ifdef P_ENABLE_USCI1_SPI1_for_DAC
        DAC_Output_M0();
    #endif

}

#endif  //--End of "#if P_FOC_CONTROL"
#endif  //--End of "#ifdef P_2R_FOCL"
//===End of "ADC0_IRQHandler" for FOC=====================================================



#if P_SIX_STEP_CONTROL //if P_SIX_STEP_CONTROL = 1,select this ADC_IRQ setting

//---ADC0_IRQHandler()-----------------------
// In the six_step control mode , do nothing in ADC0_IRQ , just clear flag(ADC0IF)
//--------------------------------------------------------------------------
void ADC0_IRQHandler(void)
{
    /* Clear flag(ADC0IF) after conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* Transfer data to PC with UART */
    TX_data_to_PC(MotorA.info.i16_rotor_speed, C_MIN_CMD_SPEED_rpm);
}

#endif
//===End of "ADC0_IRQHandler" for P_SIX_STEP_CONTROL========================================


//==============CAP0_IRQHandler============================================================
#if P_FOC_CONTROL //if FOC control = 1,select this CAP0_IRQ setting
//---CAP0_IRQHandler()----(6.84~29.6us)-----------------------------------------------
//NM1240: ECAP Interrupt for MotorA
//(ECAP_P2, ECAP_P1, ECAP_P0)=(PF2, PF1, PF0)=(Hw, Hv, Hu)
//--------------------------------------------------------------------------
uint32_t test1, test2, test3, test4;

void ECAP_IRQHandler(void)    // Enhanced Input Capture Interrupt Subroutine
{
    uint32_t temp_ecap_sts;
    int32_t temp_angle;
    uint8_t Hall_Position, Last_Hall_Position, u8_CMD_Direction, u8_Rotor_Direction;

    Last_Hall_Position = MotorA.info.u8_Last_Hall_Position;  
    Hall_Position = (HALL_PORT_M0 & 0x07);  // Reserve PF2~PF0
    MotorA.info.u8_Hall_Position = Hall_Position;

    /* clear ECAP flag */
    temp_ecap_sts = ECAP_STS;  //reserve ECAP_STS
    ECAP_STS = ECAP_STS_Available_Bits;  //Clear ECAP all flags.
    MotorA.other.u8_flag_ECAP_overflow = 0;

    /*--Check Motor is FW or RW running --*/
    u8_Rotor_Direction = Check_Rotor_FW_RW_M0(Last_Hall_Position, Hall_Position, MotorA.info.u8_rotor_direction );
    u8_CMD_Direction = MotorA.cmd.u8_direction;
    MotorA.info.u8_rotor_direction = u8_Rotor_Direction;
    test1 = u8_Rotor_Direction;
    test2 = temp_ecap_sts;

    //---Check if ECAP Overflow (means rotor_speed=0)------------
    if(CAP_180_DEGREE)  //Use Re-Load mode and check overflow 判斷有沒有overflow
    {
        /* Check if IC0 has edge flag */
        if((temp_ecap_sts & ECAP_STS_CAPTF0_Msk) != 0) //b0=CAPTF0 for IC0
            MotorA.other.u8_flag_ECAP_overflow = 0;

        if((temp_ecap_sts & ECAP_STS_CAPOVF_Msk) != 0)  //---CAPOVF=1 ==>Hall sensors do not change for a long time-----
        {
            //--If take it as motor locked, do the following 3 lines; else -----
            //Hall_Stop_Flag = 1;
            //MotorA.cmd.stop = 1; MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP; MotorA.other.u8_Motor_Running_State = C_State_Initial;
            //Stop_Motor_PWMOUT_OFF();
            //--------------------------------------------------------------------
        
            MotorA.other.u8_flag_ECAP_overflow = 1;
            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
            HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            ECAP_CNT = C_CAP_Reload_Value;
            MotorA.other.u8_operation_state = C_OPEN_LOOP;
            MotorA.other.ui16_initial_hall_change_cnt = 0;
        }
    }
    else if(CAP_60_DEGREE)    //Use up-counting from 0 and check Compare-matched 判斷有沒有CAPCMPF (compare-matched)
    {
        /* Check if IC0/1/2 has edge flag */
        if((temp_ecap_sts & (ECAP_STS_CAPTF0_Msk | ECAP_STS_CAPTF1_Msk | ECAP_STS_CAPTF2_Msk)) != 0)
            MotorA.other.u8_flag_ECAP_overflow = 0;
      
        if((temp_ecap_sts  & ECAP_STS_CAPCMPF_Msk) != 0)  //---CAPCMPF==1 ==> if Hall sensor does not change for a long time-----
        {
            //--If take it as motor blocked, do the following 3 lines; else -----
            //Hall_Stop_Flag = 1;
            //MotorA.cmd.stop = 1; MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP; MotorA.other.u8_Motor_Running_State = C_State_Initial;
            //Stop_Motor_PWMOUT_OFF();
            //--------------------------------------------------------------------

            MotorA.other.u8_flag_ECAP_overflow = 1;
            MotorA.other.i32_Capture_Value_Q15 = (int)(((C_CAP_Compare_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
            HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            ECAP_CNT = 0;  //Reset ECAP Counter;
            MotorA.other.u8_operation_state = C_OPEN_LOOP; 
            MotorA.other.ui16_initial_hall_change_cnt = 0;    
            MotorA.info.i16_rotor_speed = 0;            
        }
    }
    //-------------------------------------------------------------------------  
    
    
    if(Hall_Position != HALL_NULL1_M0 && Hall_Position != HALL_NULL2_M0)
    {  
        //---Check hall changes > c_hall_change_times ==> Can enter sine-mode --
        if(MotorA.other.u8_operation_state == C_OPEN_LOOP)  
        {
            if(MotorA.other.u8_flag_ECAP_overflow == 0)  //Counting how many phase-change comes if without overflow
            {      
                if(CAP_180_DEGREE)  
                {    
                    if(Hall_Position == HALL_STATE1_FW_M0)  
                        MotorA.other.ui16_initial_hall_change_cnt ++;
                }
                else if(CAP_60_DEGREE)  
                {      
                    MotorA.other.ui16_initial_hall_change_cnt ++;
                }      
        
                if( MotorA.other.ui16_initial_hall_change_cnt >= C_HALL_CHANGE_TIMES) //Enter pure sine wave type
                {
                    MotorA.other.u8_operation_state = C_CLOSE_LOOP;
                }
            }
        }

    
        //--Update info.i16_hall_angle according with Hall_Position-----------------
        //If with overflow, then dont need to update hall_angle
        if(MotorA.other.u8_flag_ECAP_overflow == 0)
        {  
            if(u8_Rotor_Direction == 1)  //For Rotor Forward running
            {
                switch(Hall_Position)
                {
                    case HALL_STATE1_FW_M0:  // (Hw Hv Hu)
                        Trigger_update_micro_fig();  //For trigger microcosm waveform of GUI
                        
                        if(u8_CMD_Direction == 1)  //For Forward running
                            temp_angle = HALL_STATE1_F_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE3_R_ANGLE_M0 - C_30_Degree;;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;
        
                        /* If need, average temp_angle here. */
                        MotorA.info.i16_hall_angle = temp_angle;  //Max temp_angle=2^10-1
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;  //2^10 << 16 = 2^26

                        if(Last_Hall_Position == HALL_STATE6_FW_M0)
                        {
                            if(CAP_180_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed  
                            }
                            else if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }            
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                        
                    break;

                    case HALL_STATE2_FW_M0:

                        if(u8_CMD_Direction == 1)  //For Forward running
                            temp_angle = HALL_STATE2_F_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE2_R_ANGLE_M0 - C_30_Degree;;  //Update reverse info.hall_angle
            
                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        /* If need, average temp_angle here. */
                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE1_FW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE3_FW_M0:

                        if(u8_CMD_Direction == 1)  //For Forward running
                            temp_angle = HALL_STATE3_F_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE1_R_ANGLE_M0 - C_30_Degree;;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        /* If need, average temp_angle here. */
                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE2_FW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE4_FW_M0:
  
                        if(u8_CMD_Direction == 1)  //For Forward running
                            temp_angle = HALL_STATE4_F_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE6_R_ANGLE_M0 - C_30_Degree;;  //Update reverse info.hall_angle
                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        /* If need, average temp_angle here. */
                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE3_FW_M0)
                        {
                            if(CAP_180_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                            else if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE5_FW_M0:

                        if(u8_CMD_Direction == 1)  //For Forward running
                            temp_angle = HALL_STATE5_F_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE5_R_ANGLE_M0 - C_30_Degree;;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        /* If need, average temp_angle here. */
                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE4_FW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE6_FW_M0:

                        if(u8_CMD_Direction == 1)  //For Forward running
                            temp_angle = HALL_STATE6_F_ANGLE_M0;   //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE4_R_ANGLE_M0 - C_30_Degree;  //Update reverse info.hall_angle
  
                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        /* If need, average temp_angle here. */
                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE5_FW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    default:

                    break;
                }//---end of switch(Hall_Position)
            }//---end of "if(u8_Rotor_Direction == 1)" ---

            else  //For Rotor Backward running
            {
                switch(Hall_Position)
                {
                    case HALL_STATE1_RW_M0:  // (Hw Hv Hw)
                        Trigger_update_micro_fig();  //For trigger microcosm waveform of GUI
                        
                        if(u8_CMD_Direction == 0)  //For Forward running
                            temp_angle = HALL_STATE1_R_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE3_F_ANGLE_M0 + C_30_Degree;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;
  
                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE6_RW_M0)
                        {
                            if(CAP_180_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                            else if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE2_RW_M0:

                        if(u8_CMD_Direction == 0)  //For Forward running
                            temp_angle = HALL_STATE2_R_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE2_F_ANGLE_M0 + C_30_Degree;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;
                
                        if(Last_Hall_Position == HALL_STATE1_RW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE3_RW_M0:

                        if(u8_CMD_Direction == 0)  //For Forward running
                            temp_angle = HALL_STATE3_R_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE1_F_ANGLE_M0 + C_30_Degree;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE2_RW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE4_RW_M0:

                        if(u8_CMD_Direction == 0)  //For Forward running
                            temp_angle = HALL_STATE4_R_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE6_F_ANGLE_M0 + C_30_Degree;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE3_RW_M0)
                        {
                            if(CAP_180_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                            else if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE5_RW_M0:

                        if(u8_CMD_Direction == 0)  //For Forward running
                            temp_angle = HALL_STATE5_R_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE5_F_ANGLE_M0 + C_30_Degree;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE4_RW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    case HALL_STATE6_RW_M0:

                        if(u8_CMD_Direction == 0)  //For Forward running
                            temp_angle = HALL_STATE6_R_ANGLE_M0;  //Update new info.hall_angle
                        else
                            temp_angle = HALL_STATE4_F_ANGLE_M0 + C_30_Degree;  //Update reverse info.hall_angle

                        if(temp_angle > 1023)
                            temp_angle = temp_angle - 1024;
                        else if (temp_angle < 0)
                            temp_angle = temp_angle + 1024;

                        MotorA.info.i16_hall_angle = temp_angle;
                        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = temp_angle << 16;

                        if(Last_Hall_Position == HALL_STATE5_RW_M0)
                        {
                            if(CAP_60_DEGREE)
                            {
                                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
                            }
                        }
                        else
                        {
                            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                            HallSpeedEstimator(&MotorA);
                        }
                    break;

                    default:
                    break;
                }//---end of switch(Hall_Position)
            }//---end of "else if(u8_CMD_Direction == 0)"
        }//----End of "if(MotorA.other.u8_flag_ECAP_overflow == 0)"-----
        Last_Hall_Position = Hall_Position;
        MotorA.info.u8_Last_Hall_Position = Last_Hall_Position;
    }//--end of if "if(Hall_Position != HALL_NULL1_M0 && Hall_Position != HALL_NULL2_M0)"
} //----End of "CAP0_IRQHandler(void)"

#endif
//===End of "CAP0_IRQHandler" for FOC======================================================

#if P_SIX_STEP_CONTROL //if P_SIX_STEP_CONTROL,select this CAP0_IRQ setting
//---CAP0_IRQHandler()---------------------------------------------------
//NM1240: ECAP Interrupt for MotorA
//(ECAP_P2, ECAP_P1, ECAP_P0)=(PF2, PF1, PF0)=(Hw, Hv, Hu)
//--------------------------------------------------------------------------
uint32_t test1, test2, test3, test4;

void ECAP_IRQHandler(void)    // Enhanced Input Capture Interrupt Subroutine
{
    uint32_t  temp_ecap_sts;  
    uint8_t    Hall_Position, Last_Hall_Position, u8_CMD_Direction, u8_Rotor_Direction;

    Last_Hall_Position = MotorA.info.u8_Last_Hall_Position;  
    Hall_Position = (HALL_PORT_M0 & 0x07);  // Reserve PF2~PF0
    MotorA.info.u8_Hall_Position = Hall_Position;

    /* clear ECAP flag */
    temp_ecap_sts = ECAP_STS;          //reserve ECAP_STS
    ECAP_STS = ECAP_STS_Available_Bits;      //Clear ECAP all flags.
    MotorA.other.u8_flag_ECAP_overflow = 0;    //  

    /*--Check Motor is FW or RW running --*/
    u8_Rotor_Direction = Check_Rotor_FW_RW_M0(Last_Hall_Position, Hall_Position, MotorA.info.u8_rotor_direction );
    u8_CMD_Direction = MotorA.cmd.u8_direction;
    MotorA.info.u8_rotor_direction = u8_Rotor_Direction;
    test1 = u8_Rotor_Direction;
    test2 = temp_ecap_sts;

    //---Check if ECAP Overflow (means rotor_speed=0)------------------------------
  
    if(CAP_180_DEGREE)  //Use Re-Load mode and check overflow ?????overflow
    {
        /* Check if IC0/1/2 has edge flag */
        if((temp_ecap_sts & 0x01) != 0) //b0=CAPTF0 for IC0
            MotorA.other.u8_flag_ECAP_overflow = 0;      

        if((temp_ecap_sts & ECAP_STS_CAPOVF_Msk) != 0)  //---CAPOVF=1 ==>Hall sensors do not change for a long time-----
        {
            //--If take it as motor locked, do the following 3 lines; else -----
            //Hall_Stop_Flag = 1;
            //MotorA.cmd.stop = 1; MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP; MotorA.other.u8_Motor_Running_State = C_State_Initial;
            //Stop_Motor_PWMOUT_OFF();
            //--------------------------------------------------------------------

            MotorA.other.u8_flag_ECAP_overflow = 1;
            MotorA.other.i32_Capture_Value_Q15 = (int)(((0xFFFFFF-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
            HallSpeedEstimator(&MotorA);          //Update new info.i32_rotor_speed
            ECAP_CNT = C_CAP_Reload_Value;
            MotorA.other.u8_operation_state = C_OPEN_LOOP;
            MotorA.other.ui16_initial_hall_change_cnt = 0;  //20160111 added it.
            MotorA.info.i16_rotor_speed = 0;            //20170516 added it
        }
    }

    else if(CAP_60_DEGREE)    //Use up-counting from 0 and check Compare-matched 判斷有沒有CAPCMPF (compare-matched)
    {
        /* Check if IC0/1/2 has edge flag */
        if((temp_ecap_sts & 0x07) != 0)  //b2:b0=CAPTF[2:0]
        MotorA.other.u8_flag_ECAP_overflow = 0;      

        if((temp_ecap_sts  & ECAP_STS_CAPCMPF_Msk) != 0)  //---CAPCMPF==1 ==> if Hall sensor does not change for a long time-----
        {
            //--If take it as motor blocked, do the following 3 lines; else -----
            //Hall_Stop_Flag = 1;
            //MotorA.cmd.stop = 1; MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP; MotorA.other.u8_Motor_Running_State = C_State_Initial;
            //Stop_Motor_PWMOUT_OFF();
            //--------------------------------------------------------------------
            MotorA.other.u8_flag_ECAP_overflow = 1;
            MotorA.other.i32_Capture_Value_Q15 = (int)(((C_CAP_Compare_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
            HallSpeedEstimator(&MotorA);          //Update new info.i32_rotor_speed
            ECAP_CNT = 0;       //Reset ECAP Counter;
            MotorA.other.u8_operation_state = C_OPEN_LOOP; 
            MotorA.other.ui16_initial_hall_change_cnt = 0;    
            MotorA.info.i16_rotor_speed = 0;            
        }
    }
    //---End of Check if ECAP Overflow---------------------------------------------

    // Six step control,Bulid change phase function in CAP_IRQ
    // 1.Detect Direction_CMD,and change phase by hall sensor
    // 2.Do speed estimation
    #if P_SIX_STEP_CONTROL

        #if HALL_TYPE ==1
        if(u8_CMD_Direction==1)SIX_STEP_CHANGE_PHASE_FW(); //Forward mode
        if(u8_CMD_Direction==0)SIX_STEP_CHANGE_PHASE_RW(); //Recerse mode
        #endif
  
        #if HALL_TYPE ==0 
        if(u8_CMD_Direction==0)SIX_STEP_CHANGE_PHASE_FW(); //Forward mode
        if(u8_CMD_Direction==1)SIX_STEP_CHANGE_PHASE_RW(); //Recerse mode
        #endif

    #endif

    Last_Hall_Position = Hall_Position;  
    MotorA.info.u8_Last_Hall_Position = Last_Hall_Position;
  
} //----End of "CAP0_IRQHandler(void)"

#endif
//===End of "CAP0_IRQHandler" for P_SIX_STEP_CONTROL=========================================



//==============EPWM_IRQHandler (24.3us)==========================================
// In EPWM_IRQHandler, which is triggered by EPWM period event.
// 0. Clear the flag ADC0IF.
// 1. Call PWM_Period_INT_1R(); to do the following
/*
   1. Get the Iu and Iv from EAD->DAT[0] and re-construct Iu/Iv/Iw
   2. Do Clark and Park operation to get info.Id and info.Iq
   3. Do current PI control to get cmd.Vd and cmd.Vq
   4. Do Invserse_Park, Inverse_Clark and SVPWM operation to get new PWM duty0/2/4
   5. Update the new i16_hall_angle for the next ADC0_Int. 
*/
//--------------------------------------------------------------------------

extern void PWM_Period_INT_1R(void);
void EPWM_IRQHandler(void)
{
    if(EPWM->INTSTS & EPWM_INTSTS_PIF_Msk)
    {
        /* Clear channel 0 period interrupt flag */
        EPWM->INTSTS = EPWM_INTSTS_PIF_Msk;
    #ifdef P_1R_FOC
        #if P_FOC_CONTROL
            PWM_Period_INT_1R();  //20180903
        #endif
    #endif
    }
}


/*---------------------------------------------------------------------------------------------------- 
NOTE: In NM1240, when BRAKE0 is asserted, the EPWM counter will stop automatically. 
    If system needs the EPWM counting without stop, it MUST re-start EPWM counting by software
-----------------------------------------------------------------------------------------------------*/
void BRAKE0_IRQHandler(void)
{
    /*-- Turn OFF MOS first */
    Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Use MASK function to let PWM output to turn off MOS

    /* Clear Brake 0 interrupt flag */
    EPWM->INTSTS = EPWM_INTSTS_BRK0IF_Msk;  //Must clear BKF0 before re-start CNTEN0/2/4
  
    /* Clear Brake 0 locked flag*/
    EPWM->INTSTS |= EPWM_INTSTS_BRK0LOCK_Msk;
  
    /* Re-start EPWM Counting and comparing */
//  EPWM->CTL |= EPWM_CTL_CNTCLR_Msk;  //Reset EPWM Counter (不一定要 clear counter)
    EPWM->CTL |= 0x15;  //Re-start counting, Set CNTEN0/2/4 to START EPWM counting and comparing

    /*--Record some error information for system use */
    MotorA.other.u8_flag_error_record |= 0x01;  //bit0 for Idc_OC
    MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP;

    /*-------------------------------------------------------------------------------
    Here, User may add a procedure to handle error
    --------------------------------------------------------------------------------- */

    /*  In this demo system the Brake0 IRQ is triggerred by ACMP0 which occurs a Low-to-High */
    /*  edge change at ACMPO0                                  */
    ACMP->STATUS |= ACMP_STATUS_ACMPF0_Msk;  //Clear ACMPF0 (not necessary in this demo system)
}



//---PWM_Period_INT_1R()-----(Call this function in Periodic PWM Interrupt) (25us(16.7us))-------
// In EPWM_IRQHandler, which is triggered by EPWM period event, it will be called once.
// 0. Clear the flag ADC0IF.
// 1. Get the Iu and Iv from ADC->ADC0_DAT9/ADC->ADC1_DAT9 and re-construct Iu/Iv/Iw
// 2. Do Clark and Park operation to get info.Id and info.Iq
// 3. Do current PI control to get cmd.Vd and cmd.Vq
// 4. Do Invserse_Park, Inverse_Clark and SVPWM operation to get new PWM duty0/2/4
// 5. Update the new i16_hall_angle for the next ADC0_Int.
//--------------------------------------------------------------------------
#ifdef P_1R_FOC
#if P_FOC_CONTROL //if FOC control = 1
void PWM_Period_INT_1R(void)
{
    int32_t sin_Q15, cos_Q15;
    int32_t fw_item;
    int32_t Hall_Accumulated_Theda_Q26, Hall_Step_Theda_Q26;
    int32_t I_1st, I_2nd;

    /*--Store the two phase current ADC data (PGAO voltage) which are triggered in last period. --*/
    /* (先把上一次讀取的phase current ADC data暫存起來)*/
    I_1st = ADC->ADC0_DAT9;
    I_2nd = ADC->ADC1_DAT9;

    //--Software read some analog signals by ADC -------------------------------
    /*(設為software trigger : 讀取外部可變電阻訊號, 人機介面用)*/

    /*--SW read VSP voltage for motor speed command------(3.05us) */
    MotorA.other.ui16_VSP_12bit_ADC = SW_Get_Speed_Command_From_VR();  //ADC1_SW_Read(ADC1_ADC1_P7_DAT)      /* Define a readable name for speed command */  

    //--SW read Idc current from ADC0_P4----(2.93us)
    MotorA.info.i16_IDC_Bus = SW_Get_I_BUS_From_Ishunt();  //SW_Trg_ADC0_P4_Return_Reslut()  /* Define a readable name for I_shunt */

    /*--Set ADC is HW triggered by PWM falling edge for this PWM cycle use---(2.34~2.5us) --*/
    /*(切換成Hardware trigger: trigger source來自於 PWM0/2/4的edge) */
    Set_HW_ADC_Read_OP1_O_1R_by_Indpt_Mode();    //Set_ADC0_OP1_O_ADC1_OP1_O_Independent_2SH_Trg_by_Hardware()()
    //--------------------------------------------------------------------------


    //---RE-CONSTRUCT 3-phase current Iu/v/w and setup sin/cos data-----  
    Re_Constuct_3_Phase_Current_1R(I_1st, I_2nd);

    /*--Check Phase Current Over-current or not --*/
    PhaseCurrent_OC_Check(&MotorA);  // YJS: temporarily marked

    //---End of Re-construct 3-phase current-(function start to here: 11.2~11.3us)-----------------


    /*------For TEST ONLY --------------------------------------*/
    /*
    #ifdef P_NO_MOTOER_OBSERVE_DAC
    cmd_speed_slope_with_hall(&MotorA);
    MotorA.cmd.i16_angle = MotorA.info.i16_hall_angle;
    #endif
    */
    //------------------------------------------------------------  

    //---DO FOC CURRENT CONTROL-----------------------------------------------
    //--Look up SIN Table according with hall_angle ----
    MotorA.cmd.i16_angle = MotorA.info.i16_hall_angle;  //Update the cmd.i16_angle
    sin_Q15 = SIN(MotorA.cmd.i16_angle);  //Look-up SIN table
    cos_Q15 = COS(MotorA.cmd.i16_angle);  //Look-up COS table
    MotorA.info.i32_sin_Q15 = sin_Q15;
    MotorA.info.i32_cos_Q15 = cos_Q15;

    //--Do CLARK and PARK to transfer Iu/v/w to Id/q-------(1.13us)---------
    Iuvw_to_Idq(&MotorA, sin_Q15, cos_Q15);

    //--Do Id and Iq CURRENT PI CONTROL----(Id + Iq : 2.88us)--
    //  MotorA.cmd.i16_Id = 0;  //Set Id command = 0, Iq command is the output of speed control---
    fw_item = 0;
    MotorA.info.i16_Id_error = MotorA.cmd.i16_Id - MotorA.info.i16_Id;
    MotorA.cmd.i16_Vd = PI_Id_current(&MotorA, &PI_Id, fw_item);
  
    fw_item = 0;
    MotorA.info.i16_Iq_error = MotorA.cmd.i16_Iq - MotorA.info.i16_Iq;
    MotorA.cmd.i16_Vq = PI_Iq_current(&MotorA, &PI_Iq, fw_item);

    //---For Current Open Loop Test Use------------------
    #ifdef P_VQ_OPEN_LOOP_TEST
    MotorA.cmd.i16_Vq = temp_vq; //C_Vq_OPEN_LOOP_TEST * 2 ; //2000; //5000; //3000;
    //MotorA.cmd.i16_Vd = -MotorA.other.ui16_VSP_12bit_ADC >> 2;  //More positive Vd less effeciency
    MotorA.cmd.i16_Vd = 0;
    if(MotorA.cmd.u8_direction == 0)
    {
        MotorA.cmd.i16_Vq = -MotorA.cmd.i16_Vq;
    }
    sin_Q15 = SIN(MotorA.cmd.i16_angle);
    cos_Q15 = COS(MotorA.cmd.i16_angle);
    MotorA.info.i32_sin_Q15 = sin_Q15;
    MotorA.info.i32_cos_Q15 = cos_Q15;
    #endif 
    //------------------------------------------------------

    //---Do Inv(PARK) and Inv(CLARK) and SVPWM operation to get net PWM duty0/2/4----
    //---And Update the duty to EPWM->CMPDAT[0/2/4] in the function call---(6.52~7.72us)
    Vdq_to_SVPWM_1R(&MotorA, EPWM, C_PWM_FULL_SCALE, C_MAX_PWM_DUTY, sin_Q15, cos_Q15);
    //---End of FOC CURRENT CONTROL-------------------------------------------------------

    //---UPDATE the new hall angle in every PWM period according with the speed-------------
    Hall_Accumulated_Theda_Q26 = MotorA.other.i32_Hall_Accumulated_Theda_Q26;
    Hall_Step_Theda_Q26 = MotorA.other.i32_Hall_Step_Theda_Q26;

    Update_Hall_Angle(Hall_Accumulated_Theda_Q26, Hall_Step_Theda_Q26);  

    /*-- For TEST ONLY --*/
    #ifdef P_NO_MOTOER_OBSERVE_DAC
    For_TEST_ONLY_Update_Hall_Angle(Hall_Accumulated_Theda_Q26, Hall_Step_Theda_Q26);
    #endif

//---End of Update new hall angle---(till here, total 22.8~24.2us----------------------------

    /* Transfer data to PC with UART */
    TX_data_to_PC(MotorA.info.i16_rotor_speed, C_MIN_CMD_SPEED_rpm);

    //--Debug use--------------------------------------------  
    #ifdef P_ENABLE_USCI1_SPI1_for_DAC
    DAC_Output_M0_1R();    //20180904
    #endif
//--(till here, total  35us(23.5us)) (25us(16.7us) if not include DAC_Output_M0_1R()) ------------------------------------------------
//-------------------------------------------------------  
}
#endif  //---End of "#ifdef P_1R_FOC"
#endif  //---End of "#if P_FOC_CONTROL"
//===End of "PWM_Period_INT_1R"======================================================




//*****************************************
//HardFault interrupt subroutine
//*****************************************
//void HardFault_Handler(void)
//{
//  Stop_Motor_PWMOUT_OFF();
//  MotorA.sym.u8_Iphase_OC_error = 1;
//  MotorA.other.u8_flag_error_record |= 0x10;    //bit4 for Hardfault Error

//  while(1);
//}
//===End of "HardFault_Handler"=================


//*****************************************
//UART interrupt subroutine
// When the system is operating in UART mode , transmitting and receving data through protocols , in USCI0_IRQ
//*****************************************




//*****************************************
//HardFault interrupt subroutine
//*****************************************
//void HardFault_Handler(void)
//{
//  Stop_Motor_PWMOUT_OFF();
//  MotorA.sym.u8_Iphase_OC_error = 1;
//  MotorA.other.u8_flag_error_record |= 0x10;    //bit4 for Hardfault Error

//  while(1);
//}
//===End of "HardFault_Handler"=================
