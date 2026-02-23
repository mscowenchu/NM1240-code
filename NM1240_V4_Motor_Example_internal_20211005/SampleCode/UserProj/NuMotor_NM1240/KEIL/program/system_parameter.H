#ifndef _system_parameter_h
#define _system_parameter_h

#include "NM1240.h"


//20180813
//=== Definition for Program Control ===================================================
/*-- FOC or SIX_STEP? (set 1 or 0) ---*/
#define P_FOC_CONTROL             1             // 1 : for P_FOC_CONTROL
#define P_SIX_STEP_CONTROL        0             // 1 : for P_SIX_STEP_CONTROL

#if P_SIX_STEP_CONTROL
    #define P_SIX_STEP_OPEN_LOOP  1             // 1 : select P_SIX_STEP_OPEN_LOOP
    #define P_SIX_STEP_CLOSE_LOOP 0             // 1 : select P_SIX_STEP_CLOSE_LOOP
#endif

/*---select either (P_1R_FOC) or (P_2R_FOC) with definition-------*/
#define P_1R_FOC                                //For the system is 1R FOC
//#define P_2R_FOC                                //For the system is 2R FOC

#ifdef P_2R_FOC
    #undef P_1R_FOC
#endif

//---For ECAP Parameter---
/*--select ECAP use 17 or 19 bits (17bit for lower speed system, 19bits for higher speed system)*/
#define P_USE_19BIT                             //means ECAP use 19 bits
//#define P_USE_17BIT                             //means ECAP use 17 bits


//---FOR DAC MODULE---
/*---Parameters for USCI1_SPI1-------------*/
//#define P_ENABLE_USCI1_SPI1_for_DAC             //With it, PE5(ICE_CLK) and PE7(ICE_DAT) will be SPI1 function
//========================================================================================


//---Parameters for MCU---------------------------------------------------------------
//#define SYSTEM_CLOCK 48000000                   //Unit: Hz ; for NM1240 , SYSTEM_CLOCK set 48000000 or 60000000
#define SYSTEM_CLOCK 60000000                   //Unit: Hz ; for NM1240 , SYSTEM_CLOCK set 48000000 or 60000000

//---Redefine some namming of MCU registers
#define HALL_PORT_M0              PF->PIN       //PB[2:0]=(Hw, Hv, Hu)
#define ECAP_STS                  ECAP->STS     //Re-define the ECAP Status register
#define ECAP_CNT                  ECAP->CNT     //Re-define the ECAP Counter register
#define ECAP_HLD0                 ECAP->HLD[0]  //Re-define the ECAP Hold register 0
#define ECAP_HLD1                 ECAP->HLD[1]  //Re-define the ECAP Hold register 1
#define ECAP_HLD2                 ECAP->HLD[2]  //Re-define the ECAP Hold register 2
#define ECAP_STS_Available_Bits   0x37          //bit5,4,x,2,1,0
//====================================================================================


//---Parameters for EPWM---------------------------------------------------------------
#define PWM_OUT_OFF_MOS           (~0x3F)                                   //Set the state of PWM[5:0] to turn-off MOS
//#define PWM_OUT_ON_MOS            0x3F                                      //Set the state of PWM[5:0] to turn-on MOS

#define C_MOTOR_PWM_FREQ          15000                                     //Unit :1 Hz
#define C_Tdead                   1                                         //Unit: us
#define C_EPWM_DEAD_TIME          (uint32_t)(C_Tdead*SYSTEM_CLOCK/1000000)  //Unit: 1us, MAX 4.25uS for 60MHz PWM clock
#define C_MOTOR_DEAD_TIME         C_EPWM_DEAD_TIME                          //Dead-time counter

#if P_FOC_CONTROL //C_PWM_FULL_SCALE for FOC
    #define C_PWM_FULL_SCALE      ((SYSTEM_CLOCK/2)/C_MOTOR_PWM_FREQ)       //with "/2" for center aligned mode for FOC
#endif

#if P_SIX_STEP_CONTROL  //C_PWM_FULL_SCALE for Six_step
    #define C_PWM_FULL_SCALE      (SYSTEM_CLOCK/C_MOTOR_PWM_FREQ)           // for edge aligned mode for Six_step
#endif

#define C_PWM_MARGINE             (uint32_t)(C_PWM_FULL_SCALE*0.065)          //Maximu PWM = Full*(1-Margine)
#define C_MAX_PWM_DUTY            (C_PWM_FULL_SCALE - C_PWM_MARGINE)

#define C_Tshift0                 3.1                                       //unit: us  //PWM Shift Time
#define C_Tshift1                 C_Tshift0                                 //unit: us  //PWM Shift Time
#define C_PWM_SHIFT_CMP0          (uint32_t)(C_Tshift0*SYSTEM_CLOCK/1000000)  //For CMP_DATA0.CMP
#define C_PWM_SHIFT_CMP1          (uint32_t)(C_Tshift1*SYSTEM_CLOCK/1000000)  //For CMP_DATA0.CMPU



//---Parameters for ADC -------------------------------------------------------------
#if SYSTEM_CLOCK == 48000000
    #define adc_delay_time_us     25 //35                                   //unit: 0.1us 
#endif
  
#if SYSTEM_CLOCK == 60000000
    #define adc_delay_time_us     31 // 31=25*60/48                         //unit: (0.1*48/60) us 
#endif

#define C_ADC_DELAY_TIME          (adc_delay_time_us*12/10)                 //Delay time = (4*C_ADC_DELAY_TIME)*(1/48) or (4*C_ADC_DELAY_TIME)*(1/60)us.


//===Parameter for system===========================================================
#define C_MOTOR_POLE              4                 //Unit: Number of motor pole
#define C_MAX_MOTOR_SPEED         3000              //Unit: rpm, maximum rotor speed.
#define C_SPEED_LOOP_TIME_ms      1                 //Unit: ms, Speed loop time
#define C_RPM_Do_Speed_Average    300               //Unit: rpm, When rotor speed is over it --> Do speed average.

//--For Motor Speed--------------------------------------------------
#define C_SPEED_LOOP_FREQ         1000              //Unit: ms  
#define C_SPEED_SLOPE_DELTA_RPM   4                 //Unit: rpm, it means a step which cmd.rotor_speed increase/decrease in a time  
#define C_SLOPE_rpm_Per_Sec       1000              //Unit: rpm/sec

//#define C_SPEED_SLOPE_CNT_TARGET  (C_MOTOR_PWM_FREQ*C_SPEED_SLOPE_DELTA_RPM)/C_SLOPE_rpm_Per_Sec    //How many PWM periods, cmd.rotor_speed +Delta_rpm
#define C_SPEED_SLOPE_CNT_TARGET  (C_SPEED_LOOP_FREQ*C_SPEED_SLOPE_DELTA_RPM)/C_SLOPE_rpm_Per_Sec   //How many speed loops, cmd.rotor_speed +Delta_rpm
//Note: Example: (1000*2)/400=5 --> This division must be divisible. (要湊成整除) --> Every 5ms cmd.rotor_speed += 2rpm

//Define the max/min cmd.speed_target by reading VR (variable resistor) in this demo board
#define C_MAX_CMD_SPEED_rpm       3000              //Unit: rpm
#define C_MIN_CMD_SPEED_rpm       200               //Unit: rpm
#define C_MAX_VSP_12b_ADC         4095              //12bit ADC value: 0~4095
#define C_MIN_VSP_12b_ADC         600               //12bit ADC value: 0~4095

#define C_VSP_CMD_SLOPE           ((C_MAX_CMD_SPEED_rpm - C_MIN_CMD_SPEED_rpm)*32768/(C_MAX_VSP_12b_ADC - C_MIN_VSP_12b_ADC))
#define C_VSP_DUTY_CMD_SLOPE      (C_PWM_FULL_SCALE*32768/(C_MAX_VSP_12b_ADC - C_MIN_VSP_12b_ADC))

//For Six step open loop control,before PWM duty achieve duty command,PWM_duty=PWM_duty+ dDuty

#define dDuty_VSP_10ms            50                //PWM_duty+=dDuty_VSP_10ms for VSP control , by 10ms , Maximum of dDuty =C_PWM_FULL_SCALE
#define dDuty_UART_1ms            10                //PWM_duty+=dDuty_UART_1ms for UART control , by 1ms , Maximum of dDuty =C_PWM_FULL_SCALE

//===Define the Phase Voltage (Va Vb Vc) vs Hall Signal(Hu Hv Hw)=================
// ABC: for Ia = sin(wt); Ib=sin(wt-2/3*pi); Ic=sin(wt+2/3*pi);
#define P_FW_IS_ABC                                 //If Motor phase peak voltage sequence is Va Vc Vb

//--For Motor Phase Peak Voltage Sequence is Va Vb Vc in Forward direction---------------------------
#ifdef P_FW_IS_ABC
    //--Hall_State = (Hw, Hv, Hu) = (PF2 PF1 PF0)-----
    #define HALL_STATE1_FW_M0     5                 //STATE1 is defined as Hu=PF0=IC0 has a rising edge change.
    #define HALL_STATE2_FW_M0     1
    #define HALL_STATE3_FW_M0     3
    #define HALL_STATE4_FW_M0     2
    #define HALL_STATE5_FW_M0     6
    #define HALL_STATE6_FW_M0     4
    //--Hall_State = (Hw, Hv, Hu) = (PF2 PF1 PF0)-----
    #define HALL_STATE1_RW_M0     3                 //STATE1 is defined as Hu=PF0=IC0 has a rising edge change.
    #define HALL_STATE2_RW_M0     1
    #define HALL_STATE3_RW_M0     5
    #define HALL_STATE4_RW_M0     4
    #define HALL_STATE5_RW_M0     6
    #define HALL_STATE6_RW_M0     2
#endif

#define HALL_NULL1_M0             0
#define HALL_NULL2_M0             7
#define PHASE_TYPE_M0             1                 //Type = 1 if Bemf_U=90 degree at Hu rising edge

#define C_30_Degree               85                // (30/360)*1024=85
#define SHIFT_180                 (1024*180/360)    //shift 180 degree

#define HALL_TYPE                 1                 //1: means the max(Iu) is at rising edge of Hu(IC0)
                                                    //0: means the min(Iu) is at rising edge of Hu(IC0)
                
#if (HALL_TYPE == 1)            
//---   (theda)of BEMF_U; Vq=Positive, theda=Increasing  -----
#define F_Delta_Angle_M0          0                                           //Observe the scope signal, the real phase angle and Hall edge
#define HALL_STATE1_F_ANGLE_M0    1024*( 90+F_Delta_Angle_M0)/360 + SHIFT_180 //256  0x100
#define HALL_STATE2_F_ANGLE_M0    1024*(150+F_Delta_Angle_M0)/360 + SHIFT_180 //426  0x1AA
#define HALL_STATE3_F_ANGLE_M0    1024*(210+F_Delta_Angle_M0)/360 + SHIFT_180 //597  0x255
#define HALL_STATE4_F_ANGLE_M0    1024*(270+F_Delta_Angle_M0)/360 + SHIFT_180 //768  0x300
#define HALL_STATE5_F_ANGLE_M0    1024*(330+F_Delta_Angle_M0)/360 + SHIFT_180 //938  0x3AA
#define HALL_STATE6_F_ANGLE_M0    1024*( 30+F_Delta_Angle_M0)/360 + SHIFT_180 //85   0x055

//---   (theda)of BEMF_U; Vq=Negative, theda=Decreasing   -----
#define R_Delta_Angle_M0          F_Delta_Angle_M0    //Usually, R_Delta_Angle_M0 = F_Delta_Angle_M0
#define HALL_STATE1_R_ANGLE_M0    1024*(-270+R_Delta_Angle_M0)/360  //Hall state1 absolute angle in reversed running.
#define HALL_STATE2_R_ANGLE_M0    1024*(-330+R_Delta_Angle_M0)/360
#define HALL_STATE3_R_ANGLE_M0    1024*( -30+R_Delta_Angle_M0)/360
#define HALL_STATE4_R_ANGLE_M0    1024*( -90+R_Delta_Angle_M0)/360
#define HALL_STATE5_R_ANGLE_M0    1024*(-150+R_Delta_Angle_M0)/360
#define HALL_STATE6_R_ANGLE_M0    1024*(-210+R_Delta_Angle_M0)/360
//==============================================================================

#endif 
//---End of "#if (HALL_TYPE == 1)"----------------------------------------------

#if (HALL_TYPE == 0)
//---   (theda)of BEMF_U + 180; Iq=Positive, theda=Increasing  -----
#define F_Delta_Angle_M0          0                                           //Observe the scope signal, the real phase angle leads 40 degree
#define HALL_STATE1_F_ANGLE_M0    1024*(270+F_Delta_Angle_M0)/360 + SHIFT_180 //Hall state1 absolute angle in forward running.
#define HALL_STATE2_F_ANGLE_M0    1024*(330+F_Delta_Angle_M0)/360 + SHIFT_180 
#define HALL_STATE3_F_ANGLE_M0    1024*( 30+F_Delta_Angle_M0)/360 + SHIFT_180  
#define HALL_STATE4_F_ANGLE_M0    1024*( 90+F_Delta_Angle_M0)/360 + SHIFT_180
#define HALL_STATE5_F_ANGLE_M0    1024*(150+F_Delta_Angle_M0)/360 + SHIFT_180
#define HALL_STATE6_F_ANGLE_M0    1024*(210+F_Delta_Angle_M0)/360 + SHIFT_180

//---   (theda)of BEMF_U; Iq=Negative, theda=Decreasing   -----
#define R_Delta_Angle_M0          F_Delta_Angle_M0                            //Usually, R_Delta_Angle_M0 = F_Delta_Angle_M0
#define HALL_STATE1_R_ANGLE_M0    1024*( -90+R_Delta_Angle_M0)/360            //Hall state1 absolute angle in reversed running.
#define HALL_STATE2_R_ANGLE_M0    1024*(-150+R_Delta_Angle_M0)/360
#define HALL_STATE3_R_ANGLE_M0    1024*(-210+R_Delta_Angle_M0)/360
#define HALL_STATE4_R_ANGLE_M0    1024*(-270+R_Delta_Angle_M0)/360
#define HALL_STATE5_R_ANGLE_M0    1024*(-330+R_Delta_Angle_M0)/360
#define HALL_STATE6_R_ANGLE_M0    1024*( -30+R_Delta_Angle_M0)/360
//==============================================================================
#endif
//---End of "#if (HALL_TYPE == 0)"----------------------------------------------



//**System Hardware Current & Voltage **************************
//Shunt R, Over_current, Over_voltage
//OP Gain for current amplifier
//************************************************************
#ifdef P_2R_FOC
    #define SHUNT_R_mOhm          100                                         //Shunt R, unit=mohm
		#define OP_Amp                5                                           //Times of OP-amp. 放大倍率
		#define C_1A_Q15              (SHUNT_R_mOhm*OP_Amp*ADC_FULL_RANGE*(32768/(ADC_FULL_RANGE/2))/1000/VDD) //Current_Gain, 1310 for 20mohm
#endif
#ifdef P_1R_FOC
    #define SHUNT_R_mOhm          100                                         //Shunt R, unit=mohm
		#define OP_Amp                6                                           //Times of OP-amp. 放大倍率
		#define C_1A_Q15              (SHUNT_R_mOhm*OP_Amp*ADC_FULL_RANGE*(32768/(ADC_FULL_RANGE/1))/1000/VDD) //Current_Gain, 1310 for 20mohm
#endif

#define C_IDC_OVER_CURRENT_1_0_A  5                                           //Phase Over Current Value in Amp. unit=A
//#define OP_Amp                    5                                           //Times of OP-amp. 放大倍率
#define ADC_FULL_RANGE            4096                                        //4096 for a 12-bit ADC
#define VDD                       5                                           //VDD voltage of MCU, unit=Volt
#define C_IDC_OC_OP_VOLTAGE       (C_PHASE_OC_1_0_A*SHUNT_R_mOhm*OP_Amp*ADC_FULL_RANGE/1000/VDD)  // 1638.4
//#define C_OP_UPPER_BOUND_VOLTAGE_Q15  (ADC_FULL_RANGE/2 + OC_OP_VOLTAGE)      // 3686
//#define C_OP_LOWER_BOUND_VOLTAGE_Q15  (ADC_FULL_RANGE/2 - OC_OP_VOLTAGE)      // 410
//#define C_1A_Q15                  (SHUNT_R_mOhm*OP_Amp*ADC_FULL_RANGE*(32768/(ADC_FULL_RANGE/2))/1000/VDD) //Current_Gain, 1310 for 20mohm
#define C_IDC_OC_Q15_MAX          (C_1A_Q15*C_IDC_OVER_CURRENT_1_0_A)         // 26207
#define C_IDC_OC_Q15_MIN          (-1*C_OC_Q15_MAX)                           //-26207

#define C_Max_Phase_Current       5                                           //Unit=A
#define C_MAX_Iq_CURRENT_LIMIT    (C_Max_Phase_Current * C_1A_Q15)            //Maximum Iq operating current (unit=A*current_gain)
#define C_Phase_OVER_CURRENT_1_0_A  5                                         //Phase Over Current Value in Amp. unit=A


#define C_MAX_I_CMD_0_1A          (C_OVER_CURRENT_1_0_A*10)                   //Maximum current command  //Unit: 0.1A
#define C_MAX_I_CMD_UPPER_Q15     (((C_MAX_I_CMD_0_1A+0)*C_1A_Q15)/10)        //Maximum value of Iq_cmd in Q15 format
#define C_MAX_I_CMD_LOWER_Q15     (((C_MAX_I_CMD_0_1A-0)*C_1A_Q15)/10)        //Maximum value of Iq_cmd in Q15 format

#define C_DCBUS_Voltage           24                                          //Unit: V, Vdc
#define C_DC_BUS_R1               10                                          //Unit: Kohm
#define C_DC_BUS_R2               56                                          //Unit: Kohm

/* DCV= Vdc*1000*R1/(R1+R2)/1000 */
#define C_DCBUS_DCV               (C_DCBUS_Voltage*1000*C_DC_BUS_R1/(C_DC_BUS_R1+C_DC_BUS_R2)/1000)
#define C_DCBUC_ADC_VAL           (4096*C_DCBUS_DCV/5)

#define C_Zero_I_ADC              2048                                        //12-bit ADC value at zero current. For OP is bias at 2.5V. 
#define C_Zero_I_Band             200                                         //Tolerance band 2048 +/- 200

#define C_MAX_Vq_Q15              31000                                       //Set Maximum value of Vq in Q15. square(31128)+square(10233)=sqaure(32767)
#define C_Max_Vd_Q15              10233                                       //Set Maximum value of Vd in Q15.

//20180820
#define C_1R_SHUNT_mOhm           100                                         //Unit: mOhm, One shunt resistor
#define C_Input_R_Ohm             1200                                        //Unit: Ohm, Input resistor at PGA Input
#define C_Pull_Up_R_Ohm           36000                                       //Unit: Ohm, Pull up resistor at PGA Input
#define C_PGA_In_Bias_Voltage     0.1613                                      //Unit: V, 5V*(1.2/37.2) = 0.1613
#define C_PGA_Gain                5                                           //Unit: Times
#define C_Zero_I_ADC_1R           660                                         //Unit: value of 12-bit ADC, C_PGA_In_Bias_Voltage*C_PGA_Gain/VDD*4096
//-------------------------------------------------------------------------------------------



//---Define the state number------------
/* other.u8_Motor_Running_State    //Denote motor is in which state.
0: Initial state      //When system stop drive. No PWM output
1: Motor halts        //When system will lock motor to call "brake_motor_PWM00_05" 
2: Motor in running   //When system will start driving 
*/
#define C_State_Initial           0
#define C_State_Motor_In_Halt     1
#define C_State_Motor_In_Run      2
      
/* other.u8_CMD_Motor_Action    //System use it to command motor to run or brake
0: CMD_STOP           //When system command to stop motor. "call Stop_Motor_PWMOUT_OFF()"
1: CMD_HALT           //When system command to lock motor. "call xxx" (not implemented yet)
2: CMD_RUN            //When system command to start motor.  (system receive any start signal)
3: CMD_BRAKE          //When system command to brake motor. (system receive any brake signal)
*/
#define C_CMD_STOP                0
#define C_CMD_HALT                1
#define C_CMD_RUN                 2
#define C_CMD_BRAKE               3

/* other.u8_opetation_state    //System use it to command motor to run or brake
0: C_OPEN_LOOP      //Motor started and still in the first several hall change
1:
2: C_CLOSE_LOOP     //Motor started and hall speed had could be estimated
*/
#define C_OPEN_LOOP               0
#define C_CLOSE_LOOP              2
  

//====================================================================================



//---Parameters for Id/Iq/Speed PI control------------------------------
//---Kp & Ki for motor current control-------
#define C_I_Kp                    (5000)/10         //5000 for several AMP of operation current; 50 for small motor with hundrend mA operation current
#define C_I_Ki                    (1000)/20         //1000 for several AMP of operation current; 10  for small motor with hundrend mA operation current

//---Kp & Ki for motor speed PI control-------
#define C_SP_Kp                   983040/8/2        //For with FOC
#define C_SP_Ki                   3276/10*2/2       //20151203: "/10" the speed response is smoother than "/5"


//-------------------------------------------------------------------------------------------




//---Parameters for ECAP---------------------------------------------------------------
#define C_HALL_CHANGE_TIMES       2                 //how many times of hall changes then enter sine-mode

//--Parameters for using ECAP to estimate motor speed----
//20180903: move this part to the beginning of this file

//--select every 180_deg or 60_deg to estimate motor speed once. (Only one could be 1)
#if P_FOC_CONTROL
    #define CAP_180_DEGREE        0                 //Define 1 means Input capture is latched at each 180 degree (at only every Hu change)
    #define CAP_60_DEGREE         1                 //Define 1: means input capture is latched every 60 degree (at every Hall change)
#endif

/* six_step control use CAP_60_DEGREE only */
#if P_SIX_STEP_CONTROL 
    #define CAP_180_DEGREE        0                 //Define 1 means Input capture is latched at each 180 degree (at only every Hu change)
    #define CAP_60_DEGREE         1                 //Define 1: means input capture is latched every 60 degree (at every Hall change)
#endif

#ifdef P_USE_19BIT
    #define C_CAP_Reload_Value    0xF80000          //Initial reload value of ECAP0 (0xF80000 means ECAP use 19 bits only)
    #define C_CAP_Compare_Value   0x80000           //Initial reload value of ECAP0 (0x80000 means ECAP use 19 bits only)
    #define C_CAPDIV              32                //Input capture pre-devider(CAP0_CTR1[14,13,12]), it could be 1, 4, 16 or 32
    #define C_CAP_CTL1_CAPDIV     3                 //C_CAP_CTL1_CAPDIV[14,13,12], 4=CLK/64, 3=CLK/32, 2=CLK/16 1=CLK/4
    #define C_CAP_SHIFT           4                 //Now capture is used as a 19-bit counter, right-shift 4 to be 15-bit
    #define C_CAP_GAIN            16                // C_CAP_GAIN = 2^C_CAP_SHIFT
//    yjs: Hall_Wr_EST_Constant = ((SYSTEM_CLOCK/C_CAP_GAIN*60*2/CAPDIV))/C_MOTOR_POLE_M0;  // = (72000000*60*2/DIV/Pole) >> C_CAP_SHIFT = 16875000/DIV/Pole =2109375 (if DIV=1, Pole=8)
#endif  //End of #ifdef USE_19BIT

#ifdef P_USE_17BIT 
    #define C_CAP_Reload_Value    0xFE0000          //Initial reload value of ECAP0 (0xF80000 means ECAP use 17 bits only) C_CAP_SHIFT=2
    #define C_CAP_Compare_Value   0x20000           //Initial reload value of ECAP0 (0x80000 means ECAP use 19 bits only)
    #define C_CAPDIV              128               //Input capture pre-devider(CAP0_CTR1[14,13,12]), it could be 1, 4, 16 or 32
    #define  C_CAP_CTL1_CAPDIV    7                 //C_CAP_CTL1_CAPDIV[14,13,12], 7=CLK/128, 4=CLK/64, 3=CLK/32, 2=CLK/16 1=CLK/4
    #define  C_CAP_SHIFT          2                 //Now capture is used as a 17-bit counter, right-shift 2 to be 15-bit
    #define  C_CAP_GAIN           4                 // C_CAP_GAIN = 2^C_CAP_SHIFT
//    yjs: Hall_Wr_EST_Constant = ((SYSTEM_CLOCK/C_CAP_GAIN*60*2/CAPDIV))/C_MOTOR_POLE_M0;  // = (72000000*60*2/DIV/Pole) >> C_CAP_SHIFT = 16875000/DIV/Pole =2109375 (if DIV=1, Pole=8)
#endif  //End of #ifdef USE_17BIT

//====================================================================================


//---Parameters for HDIV0---------------------------------------------------------------
static __INLINE int32_t HDIV0_DIV(int32_t dividend, int16_t divisor)
{
    HDIV0->DIVIDEND = dividend;
    HDIV0->DIVISOR = divisor;
    /* HDIV0 begin to calculate here. */
    return HDIV0->QUOTIENT;
}

static __INLINE int16_t HDIV0_MOD(int32_t dividend, int16_t divisor)
{
    HDIV0->DIVIDEND = dividend;
    HDIV0->DIVISOR = divisor;
    /* HDIV0 begin to calculate here. */
    return HDIV0->REM;
}
//====================================================================================


#endif
