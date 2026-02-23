//****************************************************************************************************************************************
// * File: svpwm.h
// * Version: V02.0
// * Date: 20190305
// * Brief: Functions for basic FOC operation.
// * MCU: NM1120, NM1200
// *
// * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
//****************************************************************************************************************************************

#include "NM1240.h"
#include "variable_typedefine.h"

//****************************************************************************************************************************************
//1. All the variables are in the range of Q15 format which range is from -32767 to +32767
//2. The function call needs a set of Motor Structure Variable. (Type define in AMotor. Refer to the example at the bottom of the file)
//****************************************************************************************************************************************


//====Iuvw_to_Idq()==================================================
//0. Pre-load Iu/Iv/Iw=Motor->info.i16_Iu/Iv/Iw
//1. Do Clark Transfer: Transfer Iu/v/w to Ialfa/beta
//2. Do Park Transfer: Transfer Ialfa/beta to Id/q
//3. Store the folloiwing variables
//   Motor->info.i16_Ialfa, Motor->info.i16_Ibeta, Motor->info.i16_Id, Motor->info.i16_Iq
//=======================================================================
extern void Iuvw_to_Idq(AMotor* Motor, int32_t sin, int32_t cos);


//====SVPWM_2R()==================================================
//1. Do Inv(Park)
//2. Do Inv(Clark)
//3. Do SVPWM for output Taoff, Tboff, Tcoff
//4. Store the folloiwing variables
//  Motor->cmd.i16_Valfa, Motor->cmd.i16_Vbeta, Motor->cmd.i16_Va, Motor->cmd.i16_Vb, Motor->cmd.i16_Vc, Motor->cmd.u8_zone      
//================================================================
extern void SVPWM_2R(int32_t Vd, int32_t Vq, int32_t sin, int32_t cos, int32_t pwm_full_scale, AMotor* Motor, uint16_t* zone, uint16_t* Taoff, uint16_t* Tboff, uint16_t* Tcoff);


//====SVPWM_1R()==================================================
//1. Do Inv(Park)
//2. Do Inv(Clark)
//3. Do SVPWM for output Taoff_Up, Tboff_Up, Tcoff_Up, Taoff_Down, Tboff_Down, Tcoff_Down
//4. Store the folloiwing variables
//  Motor->cmd.i16_Valfa, Motor->cmd.i16_Vbeta, Motor->cmd.i16_Va, Motor->cmd.i16_Vb, Motor->cmd.i16_Vc, Motor->cmd.u8_zone      
//================================================================
extern void SVPWM_1R(int32_t Vd, int32_t Vq, int32_t sin, int32_t cos, int32_t pwm_full_scale, uint16_t pwm_shift_CMP0, uint16_t pwm_shift_CMP1, uint16_t Max_PWM_Duty, 
      uint16_t PWM_Margine, AMotor* Motor, uint16_t* zone, uint16_t* Taoff_Up, uint16_t* Tboff_Up, uint16_t* Tcoff_Up, uint16_t* Taoff_Down, uint16_t* Tboff_Down, uint16_t* Tcoff_Down);


//=======================================================================================
// Example of Structure Motor Variable
//=======================================================================================
/*
typedef struct tag_MotorInfo // Real-time feedback information/signals
{  
    int16_t i16_Iu;         //Phase Current Iu (Ia)
    int16_t i16_Iv;         //Phase Current Iv (Ib)
    int16_t i16_Iw;         //Phase Current Iw (Ic)
    int16_t i16_Ialfa;      //Clark Ialfa
    int16_t i16_Ibeta;      //Clark Ibeta
    int16_t i16_Id;         //Park Id
    int16_t i16_Iq;         //Park Iq
}AMotorInfo;

typedef struct tag_MotorCommand // command
{
    //FOC outputs and be the commands for the next block
    int16_t i16_Valfa;
    int16_t i16_Vbeta;
    int16_t i16_Va;
    int16_t i16_Vb;
    int16_t i16_Vc;

  uint8_t  u8_zone;        //zone number in SVPWM operation  //20180903
}AMotorCommand;


//--motor struct------------------
typedef struct tag_Motor
{ 
    AMotorInfo      info;
    AMotorCommand    cmd;
}AMotor; 


//--Global motor variable struct
AMotor MotorA;
  
*/
