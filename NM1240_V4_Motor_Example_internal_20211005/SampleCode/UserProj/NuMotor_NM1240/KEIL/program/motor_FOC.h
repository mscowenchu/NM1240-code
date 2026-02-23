#ifndef motor_foc_H
#define motor_foc_H


#include <stdio.h>  
#include "NM1240.h"
#include "system_initialize.h"
#include "system_parameter.h"
#include "variable_typedefine.h"


extern const int16_t sinTab[]; // 1024 int16-data; Angle range: 0~360 degree, Value range:-32767~32767

//===Functions related to FOC control ====================================
extern int16_t Cos(int16_t angle);
extern int16_t Sin(int16_t angle);
extern int16_t under_over_16_cp(int16_t angle);
extern void Iuvw_to_Idq(AMotor* Motor, int32_t sin, int32_t cos);
extern void Clark_Tranform(int32_t Iu, int32_t Iv, int32_t Iw, int32_t sin, int32_t cos);
extern void Vdq_to_SVPWM_2R(AMotor* Motor, EPWM_T* epwm, int32_t pwm_full_scale, int32_t pwm_max_duty, int32_t sin, int32_t cos);
extern void Vdq_to_SVPWM_1R(AMotor* Motor, EPWM_T* epwm, int32_t pwm_full_scale, int32_t pwm_max_duty, int32_t sin, int32_t cos);
//=================================================================================

static __INLINE int16_t COS(int16_t angle)
{
    angle += 256;
    if(angle >= 1024)
        angle -= 1024;
    return sinTab[angle];
}

static __INLINE int16_t SIN(int16_t angle)
{
    if(angle >= 1024)
    angle -= 1024;
    return sinTab[angle];
}


//---Update_Hall_Angle()-------------------------------------------------------
//  Update the new step theda in every PWM period according with the speed
//-----------------------------------------------------------------------------
static __INLINE void Update_Hall_Angle(int32_t Hall_Accumulated_Theda_Q26, int32_t Hall_Step_Theda_Q26)
{
    if (MotorA.other.u8_operation_state == C_CLOSE_LOOP)  //C_CLOSE_LOOP=2
    {
        Hall_Accumulated_Theda_Q26 += Hall_Step_Theda_Q26;

        if(Hall_Accumulated_Theda_Q26 > ((1<<26)-1))
            Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26 - (1<<26);
        else if (Hall_Accumulated_Theda_Q26 < 0)
            Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26 + (1<<26);
    
        /*--Note: If rotor speed is slow, user may use square-wave like to drive the motor
        User may implement own method to update the hall_angle at here            ----*/
    
        MotorA.info.i16_hall_angle = (signed short int) ((Hall_Accumulated_Theda_Q26 >> 16));
    }

    MotorA.other.i32_Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26;
    MotorA.other.i32_Hall_Step_Theda_Q26 = Hall_Step_Theda_Q26;
}


//---For_TEST_ONLY_Update_Hall_Angle()-------------------------------------------------------
// For Test Only
//  Update the new step theda in every PWM period according with the speed
//-----------------------------------------------------------------------------
static __INLINE void For_TEST_ONLY_Update_Hall_Angle(int32_t Hall_Accumulated_Theda_Q26, int32_t Hall_Step_Theda_Q26)
{
    Hall_Accumulated_Theda_Q26 += Hall_Step_Theda_Q26;

    if(Hall_Accumulated_Theda_Q26 > ((1<<26)-1))
        Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26 - (1<<26);
    else if (Hall_Accumulated_Theda_Q26 < 0)
        Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26 + (1<<26);
    
    MotorA.info.i16_hall_angle = (signed short int) ((Hall_Accumulated_Theda_Q26 >> 16));
  
    MotorA.other.i32_Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26;
    MotorA.other.i32_Hall_Step_Theda_Q26 = Hall_Step_Theda_Q26;
}


//---Update_Hall_Angle()-------------------------------------------------------
//  Update the new step theda in every PWM period according with the speed
//-----------------------------------------------------------------------------
static __INLINE void Re_Constuct_3_Phase_Current_2R(void)
{
    int32_t Iu, Iv, Iw;
  
    Iu = ADC->ADC1_DAT1;
    Iu = -( (Iu - MotorA.info.i16_Iu_ref_ADC) << 4);    //Iu in Q15 format now
    Iv = ADC->ADC0_DAT3;
    Iv = -( (Iv - MotorA.info.i16_Iv_ref_ADC) << 4);    //Iu in Q15 format now
    Iw = -Iu - Iv;  
    
    MotorA.info.i16_Iu = Iu;
    MotorA.info.i16_Iv = Iv;
    MotorA.info.i16_Iw = Iw;
}

//===Functions related to  ====================================

//---Re_Constuct_3_Phase_Current_1R()-------------------------------------------------------
//  According the ZONE number to re-construct the phase current
// Note: With a pre-condition that we suppose the maximum current voltage in the 1R system must not 
//    be over 2.5V. That is the used range of detected voltage is the same as 2R system (Vdd/2)
//  如果電壓2.5V當作 Maximum current boundary voltage, 那就 << 4 --> Same as 2R system
//  如果電壓5.0V當作 Maximum current boundary voltage, 那就 << 3 --> 可量測的電流範圍是2R的兩倍
//------------------------------------------------------------------------------------------
static __INLINE void Re_Constuct_3_Phase_Current_1R(int32_t I_1, int32_t I_2)
{
    int32_t Iu, Iv, Iw;
    int32_t I1_ref_ADC, I2_ref_ADC;
    I1_ref_ADC = MotorA.info.i16_Iu_ref_ADC;
    I2_ref_ADC = MotorA.info.i16_Iv_ref_ADC;
  
    switch(MotorA.cmd.u8_zone)  //Set ADC0_CH5 first, then ADC1_CH5
    {
        case 5:  //L M S = V W U, -Iu, Iv
            Iu = I_1;
            Iu = -( (Iu - I1_ref_ADC) << 3);  //In Q15 format now
            Iv = I_2;
            Iv = ( (Iv - I2_ref_ADC) << 3);  //In Q15 format now
            Iw = -Iu - Iv;   
        break;
    
        case 4:  //L M S = W V U, -Iu, Iw
            Iu = I_1;
            Iu = -( (Iu - I1_ref_ADC) << 3);  //In Q15 format now
            Iw = I_2;
            Iw = ( (Iw - I2_ref_ADC) << 3);  //In Q15 format now
            Iv = -Iu - Iw;  
        break;

        case 6:  //L M S = W U V, -Iv, Iw
            Iv = I_1;
            Iv = -( (Iv - I1_ref_ADC) << 3);  //In Q15 format now
            Iw = I_2;
            Iw = ( (Iw - I2_ref_ADC) << 3);  //In Q15 format now
            Iu = -Iv - Iw;  
        break;

        case 2:  //L M S = U W V, -Iv, Iu
            Iv = I_1;
            Iv = -( (Iv - I1_ref_ADC) << 3);  //In Q15 format now
            Iu = I_2;
            Iu = ( (Iu - I2_ref_ADC) << 3);  //In Q15 format now
            Iw = -Iv - Iu;  
        break;
    
        case 3:  //L M S = U V W, -Iw, Iu
            Iw = I_1;
            Iw = -( (Iw - I1_ref_ADC) << 3);  //In Q15 format now
            Iu = I_2;
            Iu = ( (Iu - I2_ref_ADC) << 3);  //In Q15 format now
            Iv = -Iw - Iu;  
        break;

        case 1:  //L M S = V U W, -Iw, Iv
            Iw = I_1;
            Iw = -( (Iw - I1_ref_ADC) << 3);  //In Q15 format now
            Iv = I_2;
            Iv = ( (Iv - I2_ref_ADC) << 3);  //In Q15 format now
            Iu = -Iw - Iv;  
        break;

        default:  //2. Temporally set ADC0/1 Trigger Source from EPWM0/2 Falling Edge
            Iu = I_1;
            Iu = -( (Iu - I1_ref_ADC) << 3);  //In Q15 format now
            Iw = I_2;
            Iw = ( (Iw - I2_ref_ADC) << 3);  //In Q15 format now
            Iv = -Iu - Iw;  
        break;
    }

//  MotorA.info.i16_Iu = Iu;
//  MotorA.info.i16_Iv = Iv;
//  MotorA.info.i16_Iw = Iw;
  
    MotorA.info.i16_Iu = (Iu + MotorA.info.i16_Iu) >> 1;
    MotorA.info.i16_Iv = (Iv + MotorA.info.i16_Iv) >> 1;
    MotorA.info.i16_Iw = (Iw + MotorA.info.i16_Iw) >> 1;
}
//===Functions related to  ====================================
#endif
